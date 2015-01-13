/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <stdio.h>

#include "board.h"
#include "cpu.h"
#include "periph/spi.h"
#include "periph_conf.h"
#include "thread.h"
#include "sched.h"
#include "vtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @ingroup     cpu_kinetis_common
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */

/* guard this file in case no SPI device is defined */
#if SPI_NUMOF

typedef struct {
    char(*cb)(char data);
} spi_state_t;

static inline void irq_handler_transfer(SPI_Type *spi, spi_t dev);

static spi_state_t spi_config[SPI_NUMOF];


/**
 * @brief Helper function for finding optimal baud rate scalers.
 *
 * Find the prescaler and scaler settings that will yield a clock frequency
 * as close as possible (but not above) the target frequency, given the module
 * runs at module_clock Hz.
 *
 * Hardware properties (Baud rate configuration):
 * Possible prescalers: 2, 3, 5, 7
 * Possible scalers: 2, 4, 6 (sic!), 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768
 *
 * SCK baud rate = (f_BUS/PBR) x [(1+DBR)/BR]
 *
 * where PBR is the prescaler, BR is the scaler, DBR is the Double BaudRate bit.
 *
 * @note We are not using the DBR bit because it may affect the SCK duty cycle.
 *
 * @param module_clock Module clock frequency (e.g. F_BUS)
 * @param target_clock Desired baud rate
 * @param closest_prescaler pointer where to write the optimal prescaler index.
 * @param closest_scaler pointer where to write the optimal scaler index.
 *
 * @return 0 on success, <0 on error.
 */
static int find_closest_baudrate_scalers(const uint32_t module_clock, const uint32_t target_clock, uint8_t *closest_prescaler, uint8_t *closest_scaler)
{
  uint8_t i;
  uint8_t k;
  int freq;
  static const uint8_t num_scalers = 16;
  static const uint8_t num_prescalers = 4;
  static const int br_scalers[16] = {
        2,     4,     6,     8,    16,    32,    64,   128,
      256,   512,  1024,  2048,  4096,  8192, 16384, 32768
  };
  static const int br_prescalers[4] = {2, 3, 5, 7};

  int closest_frequency = -1;

  /* Test all combinations until we arrive close to the target clock */
  for (i = 0; i < num_prescalers; ++i)
  {
    for (k = 0; k < num_scalers; ++k)
    {
      freq = module_clock / (br_scalers[k] * br_prescalers[i]);
      if (freq <= target_clock)
      {
        /* Found closest lower frequency at this prescaler setting,
         * compare to the best result */
        if (closest_frequency < freq)
        {
          closest_frequency = freq;
          *closest_scaler = k;
          *closest_prescaler = i;
        }
        break;
      }
    }
  }
  if (closest_frequency < 0)
  {
    /* Error, no solution found, this line is never reachable with current
     * hardware settings unless a _very_ low target clock is requested.
     * (scaler_max * prescaler_max) = 229376 => target_min@100MHz = 435 Hz*/
    return -1;
  }

  return 0;
}

/**
 * @brief Helper function for finding optimal delay scalers.
 *
 * Find the prescaler and scaler settings that will yield a delay timing
 * as close as possible (but not shorter than) the target delay, given the
 * module runs at module_clock Hz.
 *
 * Hardware properties (delay configuration):
 * Possible prescalers: 1, 3, 5, 7
 * Possible scalers: 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536
 *
 * delay = (1/f_BUS) x prescaler x scaler
 *
 * Because we want to do this using only integers, the target_freq parameter is
 * the reciprocal of the delay time.
 *
 * @param module_clock Module clock frequency (e.g. F_BUS)
 * @param target_freq Reciprocal (i.e. 1/t [Hz], frequency) of the desired delay time.
 * @param closest_prescaler pointer where to write the optimal prescaler index.
 * @param closest_scaler pointer where to write the optimal scaler index.
 *
 * @return 0 on success, <0 on error.
 */
static int find_closest_delay_scalers(const uint32_t module_clock, const uint32_t target_freq, uint8_t *closest_prescaler, uint8_t *closest_scaler) {
  uint8_t i;
  uint8_t k;
  int freq;
  int prescaler;
  int scaler;
  static const uint8_t num_scalers = 16;
  static const uint8_t num_prescalers = 4;

  int closest_frequency = -1;

  /* Test all combinations until we arrive close to the target clock */
  for (i = 0; i < num_prescalers; ++i)
  {
    for (k = 0; k < num_scalers; ++k)
    {
      prescaler = (i * 2) + 1;
      scaler = (1 << (k + 1)); /* 2^(k+1) */
      freq = module_clock / (prescaler * scaler);
      if (freq <= target_freq)
      {
        /* Found closest lower frequency at this prescaler setting,
         * compare to the best result */
        if (closest_frequency < freq)
        {
          closest_frequency = freq;
          *closest_scaler = k;
          *closest_prescaler = i;
        }
        break;
      }
    }
  }
  if (closest_frequency < 0)
  {
    /* Error, no solution found, this line is never reachable with current
     * hardware settings unless a _very_ low target clock is requested.
     * (scaler_max * prescaler_max) = 458752 */
    return -1;
  }

  return 0;
}

int spi_init_master(spi_t dev, spi_conf_t conf, spi_speed_t speed)
{
    SPI_Type *spi_dev;
    uint8_t br_prescaler = 0xff;
    uint8_t br_scaler = 0xff;
    uint8_t prescaler_tmp = 0xff;
    uint8_t scaler_tmp = 0xff;
    uint32_t ctar = 0;
    uint32_t br_desired;
    uint32_t module_clock;
    uint32_t tcsc_freq;
    uint32_t tasc_freq;
    uint32_t tdt_freq;

    switch (speed) {
        case SPI_SPEED_100KHZ:
            br_desired = 100000;
            break;

        case SPI_SPEED_400KHZ:
            br_desired = 400000;
            break;

        case SPI_SPEED_1MHZ:
            br_desired = 1000000;
            break;

        case SPI_SPEED_5MHZ:
            br_desired = 5000000;
            break;

        case SPI_SPEED_10MHZ:
            br_desired = 10000000;
            break;

        default:
            return -2;
    }

    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            spi_dev = SPI_0_DEV;
            module_clock = SPI_0_FREQ;
            tcsc_freq = SPI_0_TCSC_FREQ;
            tasc_freq = SPI_0_TASC_FREQ;
            tdt_freq = SPI_0_TDT_FREQ;
            /* enable clocks */
            SPI_0_CLKEN();
            SPI_0_SCK_PORT_CLKEN();
            SPI_0_SOUT_PORT_CLKEN();
            SPI_0_SIN_PORT_CLKEN();
            /* Set PORT to AF mode */
            SPI_0_SCK_PORT->PCR[SPI_0_SCK_PIN] = PORT_PCR_MUX(SPI_0_SCK_AF);
            SPI_0_SOUT_PORT->PCR[SPI_0_SOUT_PIN] = PORT_PCR_MUX(SPI_0_SOUT_AF);
            SPI_0_SIN_PORT->PCR[SPI_0_SIN_PIN] = PORT_PCR_MUX(SPI_0_SIN_AF);
            break;
#endif /* SPI_0_EN */

        default:
            return -1;
    }
    /* Find baud rate scaler and prescaler settings */
    if (find_closest_baudrate_scalers(module_clock, br_desired,
                                      &br_prescaler, &br_scaler) < 0) {
        /* Desired baud rate is too low to be reachable at current module clock frequency. */
        return -2;
    }
    ctar |= SPI_CTAR_PBR(br_prescaler) | SPI_CTAR_BR(br_scaler);

    /* Find the other delay divisors */
    /* tCSC */
    if (tcsc_freq == 0) {
      /* Default to same as baud rate if set to zero. */
      tcsc_freq = br_desired;
    }
    if (find_closest_delay_scalers(module_clock, tcsc_freq,
          &prescaler_tmp, &scaler_tmp) < 0) {
        /* failed to find a solution */
        return -2;
    }
    ctar |= SPI_CTAR_PCSSCK(prescaler_tmp) | SPI_CTAR_CSSCK(scaler_tmp);

    /* tASC */
    if (tasc_freq == 0) {
      /* Default to same as baud rate if set to zero. */
      tasc_freq = br_desired;
    }
    if (find_closest_delay_scalers(module_clock, tasc_freq,
          &prescaler_tmp, &scaler_tmp) < 0) {
        /* failed to find a solution */
        return -2;
    }
    ctar |= SPI_CTAR_PASC(prescaler_tmp) | SPI_CTAR_ASC(scaler_tmp);

    /* tDT */
    if (tdt_freq == 0) {
      /* Default to same as baud rate if set to zero. */
      tdt_freq = br_desired;
    }
    if (find_closest_delay_scalers(module_clock, tdt_freq,
          &prescaler_tmp, &scaler_tmp) < 0) {
        /* failed to find a solution */
        return -2;
    }
    ctar |= SPI_CTAR_PDT(prescaler_tmp) | SPI_CTAR_DT(scaler_tmp);

    /* set speed for 8-bit access */
    spi_dev->CTAR[0] = SPI_CTAR_FMSZ(7)
                       | ctar;
    /* set speed for 16-bit access */
    spi_dev->CTAR[1] = SPI_CTAR_FMSZ(15)
                       | ctar;

    /* Set clock polarity and phase. */
    switch (conf) {
        case SPI_CONF_FIRST_RISING:
            spi_dev->CTAR[0] &= ~(SPI_CTAR_CPHA_MASK | SPI_CTAR_CPOL_MASK);
            spi_dev->CTAR[1] &= ~(SPI_CTAR_CPHA_MASK | SPI_CTAR_CPOL_MASK);
            break;
        case SPI_CONF_SECOND_RISING:
            spi_dev->CTAR[0] &= ~(SPI_CTAR_CPOL_MASK);
            spi_dev->CTAR[0] |= SPI_CTAR_CPHA_MASK;
            spi_dev->CTAR[1] &= ~(SPI_CTAR_CPOL_MASK);
            spi_dev->CTAR[1] |= SPI_CTAR_CPHA_MASK;
            break;
        case SPI_CONF_FIRST_FALLING:
            spi_dev->CTAR[0] &= ~(SPI_CTAR_CPHA_MASK);
            spi_dev->CTAR[0] |= SPI_CTAR_CPOL_MASK;
            spi_dev->CTAR[1] &= ~(SPI_CTAR_CPHA_MASK);
            spi_dev->CTAR[1] |= SPI_CTAR_CPOL_MASK;
            break;
        case SPI_CONF_SECOND_FALLING:
            spi_dev->CTAR[0] |= SPI_CTAR_CPHA_MASK | SPI_CTAR_CPOL_MASK;
            spi_dev->CTAR[1] |= SPI_CTAR_CPHA_MASK | SPI_CTAR_CPOL_MASK;
            break;
        default:
            return -2;
    }

    /* enable SPI */
    spi_dev->MCR = SPI_MCR_MSTR_MASK
                   | SPI_MCR_DOZE_MASK
                   | SPI_MCR_CLR_TXF_MASK
                   | SPI_MCR_CLR_RXF_MASK;

    spi_dev->RSER = (uint32_t)0;

    return 0;
}

int spi_init_slave(spi_t dev, spi_conf_t conf, char(*cb)(char data))
{
    SPI_Type *spi_dev;

    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            spi_dev = SPI_0_DEV;
            /* enable clocks */
            SPI_0_CLKEN();
            SPI_0_PCS0_PORT_CLKEN();
            SPI_0_SCK_PORT_CLKEN();
            SPI_0_SOUT_PORT_CLKEN();
            SPI_0_SIN_PORT_CLKEN();
            /* Set PORT to AF mode */
            SPI_0_PCS0_PORT->PCR[SPI_0_PCS0_PIN] = PORT_PCR_MUX(SPI_0_PCS0_AF);
            SPI_0_SCK_PORT->PCR[SPI_0_SCK_PIN] = PORT_PCR_MUX(SPI_0_SCK_AF);
            SPI_0_SOUT_PORT->PCR[SPI_0_SOUT_PIN] = PORT_PCR_MUX(SPI_0_SOUT_AF);
            SPI_0_SIN_PORT->PCR[SPI_0_SIN_PIN] = PORT_PCR_MUX(SPI_0_SIN_AF);
            break;
#endif /* SPI_0_EN */

        default:
            return -1;
    }

    /* set speed */
    spi_dev->CTAR[0] = SPI_CTAR_SLAVE_FMSZ(7);

    /* Set clock polarity and phase. */
    switch (conf) {
        case SPI_CONF_FIRST_RISING:
            spi_dev->CTAR[0] &= ~(SPI_CTAR_CPHA_MASK | SPI_CTAR_CPOL_MASK);
            spi_dev->CTAR[1] &= ~(SPI_CTAR_CPHA_MASK | SPI_CTAR_CPOL_MASK);
            break;
        case SPI_CONF_SECOND_RISING:
            spi_dev->CTAR[0] &= ~(SPI_CTAR_CPOL_MASK);
            spi_dev->CTAR[0] |= SPI_CTAR_CPHA_MASK;
            spi_dev->CTAR[1] &= ~(SPI_CTAR_CPOL_MASK);
            spi_dev->CTAR[1] |= SPI_CTAR_CPHA_MASK;
            break;
        case SPI_CONF_FIRST_FALLING:
            spi_dev->CTAR[0] &= ~(SPI_CTAR_CPHA_MASK);
            spi_dev->CTAR[0] |= SPI_CTAR_CPOL_MASK;
            spi_dev->CTAR[1] &= ~(SPI_CTAR_CPHA_MASK);
            spi_dev->CTAR[1] |= SPI_CTAR_CPOL_MASK;
            break;
        case SPI_CONF_SECOND_FALLING:
            spi_dev->CTAR[0] |= SPI_CTAR_CPHA_MASK | SPI_CTAR_CPOL_MASK;
            spi_dev->CTAR[1] |= SPI_CTAR_CPHA_MASK | SPI_CTAR_CPOL_MASK;
            break;
        default:
            return -2;
    }

    /* enable SPI */
    spi_dev->MCR = SPI_MCR_DOZE_MASK
                   | SPI_MCR_PCSIS(SPI_0_PCS0_ACTIVE_LOW << 0)
                   | SPI_MCR_CLR_TXF_MASK
                   | SPI_MCR_CLR_RXF_MASK;

    spi_dev->RSER = (uint32_t)0;

    return 0;
}

int spi_transfer_byte(spi_t dev, char out, char *in)
{
    SPI_Type *spi_dev;

    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            spi_dev = SPI_0_DEV;
            break;
#endif

        default:
            return -1;
    }

    while (!(spi_dev->SR & SPI_SR_TFFF_MASK));

    /* The chip select lines are expected to be controlled via software in RIOT. */
    spi_dev->PUSHR = SPI_PUSHR_CTAS(0)
                     | SPI_PUSHR_EOQ_MASK
                     | SPI_PUSHR_TXDATA(out);

    while (!(spi_dev->SR & SPI_SR_RXCTR_MASK));

    spi_dev->SR = SPI_SR_EOQF_MASK;

    if (in != NULL) {
        *in = (char)spi_dev->POPR;
    }
    else {
        spi_dev->POPR;
    }

    return 1;
}

int spi_transfer_bytes(spi_t dev, char *out, char *in, unsigned int length)
{
    SPI_Type *spi_dev;

    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            spi_dev = SPI_0_DEV;
            break;
#endif

        default:
            return -1;
    }

    if (out == NULL || in == NULL) {
        return -1;
    }

    int i, trans_bytes = 0;

    for (i = 0; i < (length - 1); i++) {
        while (!(spi_dev->SR & SPI_SR_TFFF_MASK));

        spi_dev->PUSHR = SPI_PUSHR_CTAS(0)
                         | SPI_PUSHR_CONT_MASK
                         | SPI_PUSHR_TXDATA(out[i]);

        while (!(spi_dev->SR & SPI_SR_RXCTR_MASK));

        in[i] = (char)spi_dev->POPR;

        trans_bytes++;
    }

    while (!(spi_dev->SR & SPI_SR_TFFF_MASK));

    spi_dev->PUSHR = SPI_PUSHR_CTAS(0)
                     | SPI_PUSHR_EOQ_MASK
                     | SPI_PUSHR_TXDATA(out[i]);

    while (!(spi_dev->SR & SPI_SR_RXCTR_MASK));

    spi_dev->SR = SPI_SR_EOQF_MASK;
    in[i] = (char)spi_dev->POPR;
    trans_bytes++;

    return trans_bytes++;
}

int spi_transfer_reg(spi_t dev, uint8_t reg, char out, char *in)
{
    SPI_Type *spi_dev;

    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            spi_dev = SPI_0_DEV;
            break;
#endif

        default:
            return -1;
    }

    while (!(spi_dev->SR & SPI_SR_TFFF_MASK));

    spi_dev->PUSHR = SPI_PUSHR_CTAS(1)
                     | SPI_PUSHR_EOQ_MASK
                     | SPI_PUSHR_TXDATA((uint16_t)(reg << 8) | (uint16_t)out);

    while (!(spi_dev->SR & SPI_SR_RXCTR_MASK));

    spi_dev->SR = SPI_SR_EOQF_MASK;

    if (in != NULL) {
        *in = (char)spi_dev->POPR;
    }
    else {
        spi_dev->POPR;
    }

    return 2;
}

int spi_transfer_regs(spi_t dev, uint8_t reg, char *out, char *in, unsigned int length)
{
    SPI_Type *spi_dev;

    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            spi_dev = SPI_0_DEV;
            break;
#endif

        default:
            return -1;
    }

    if (out == NULL || in == NULL) {
        return -1;
    }

    int i, trans_bytes = 0;

    while (!(spi_dev->SR & SPI_SR_TFFF_MASK));

    spi_dev->PUSHR = SPI_PUSHR_CTAS(1)
                     | SPI_PUSHR_CONT_MASK
                     | SPI_PUSHR_TXDATA((uint16_t)(reg << 8) | (uint16_t)out[0]);

    while (!(spi_dev->SR & SPI_SR_RXCTR_MASK));

    spi_dev->SR = SPI_SR_EOQF_MASK;
    in[0] = (char)spi_dev->POPR;
    trans_bytes++;


    for (i = 1; i < (length - 1); i++) {
        while (!(spi_dev->SR & SPI_SR_TFFF_MASK));

        spi_dev->PUSHR = SPI_PUSHR_CTAS(0)
                         | SPI_PUSHR_CONT_MASK
                         | SPI_PUSHR_TXDATA(out[i]);

        while (!(spi_dev->SR & SPI_SR_RXCTR_MASK));

        in[i] = (char)spi_dev->POPR;

        trans_bytes++;
    }

    while (!(spi_dev->SR & SPI_SR_TFFF_MASK));

    spi_dev->PUSHR = SPI_PUSHR_CTAS(0)
                     | SPI_PUSHR_EOQ_MASK
                     | SPI_PUSHR_TXDATA(out[i]);

    while (!(spi_dev->SR & SPI_SR_RXCTR_MASK));

    spi_dev->SR = SPI_SR_EOQF_MASK;
    in[i] = (char)spi_dev->POPR;
    trans_bytes++;

    return trans_bytes++;
}

void spi_transmission_begin(spi_t dev, char reset_val)
{

    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            SPI_0_DEV->PUSHR = SPI_PUSHR_CTAS(0)
                               | SPI_PUSHR_EOQ_MASK
                               | SPI_PUSHR_TXDATA(reset_val);
            break;
#endif
    }
}

void spi_poweron(spi_t dev)
{
    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            SPI_0_CLKEN();
            break;
#endif
    }
}

void spi_poweroff(spi_t dev)
{
    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            while (SPI_0_DEV->SR & SPI_SR_EOQF_MASK);

            SPI_0_CLKDIS();
            break;
#endif
    }
}

static inline void irq_handler_transfer(SPI_Type *spi, spi_t dev)
{

    if (spi->SR & SPI_SR_RFDF_MASK) {
        char data;
        data = (char)spi->POPR;
        data = spi_config[dev].cb(data);
        spi->PUSHR = SPI_PUSHR_CTAS(0)
                     | SPI_PUSHR_EOQ_MASK
                     | SPI_PUSHR_TXDATA(data);
    }

    /* see if a thread with higher priority wants to run now */
    if (sched_context_switch_request) {
        thread_yield();
    }
}

#if SPI_0_EN
void SPI_0_IRQ_HANDLER(void)
{
    irq_handler_transfer(SPI_0_DEV, SPI_0);
}
#endif

#endif /* SPI_NUMOF */
