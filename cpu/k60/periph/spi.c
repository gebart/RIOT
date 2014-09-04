/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_k60
 * @{
 *
 * @file        spi.c
 * @brief       Low-level SPI driver implementation
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "periph_conf.h"
#include "board.h"

#if SPI_NUMOF

#define ENABLE_DEBUG (0)
#include "debug.h"

/** Find the prescaler and scaler settings that will yield a clock frequency
 * as close as possible (but not above) the target frequency, given the module
 * runs at module_clock Hz.
 *
 * Hardware properties (Baud rate configuration):
 * Possible prescalers: 2, 3, 5, 7
 * Possible scalers: 2, 4, 6 (sic!), 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768
 *
 * SCK baud rate = (f_SYS/PBR) x [(1+DBR)/BR]
 *
 * where PBR is the prescaler, BR is the scaler, DBR is the Double BaudRate bit.
 *
 * \note We are not using the DBR bit because it may affect the SCK duty cycle.
 */
static int find_closest_scalers(unsigned int module_clock, unsigned int target_clock, uint32_t *settings)
{
    static const int num_scalers = 16;
    static const int num_prescalers = 4;
    static const unsigned int scalers[] = {2, 4, 6, 8, 16, 32, 64, 128, 256,
        512, 1024, 2048, 4096, 8192, 16384, 32768};
    static const unsigned int prescalers[] = {2, 3, 5, 7};

    int closest_frequency = -1;
    int closest_scaler = -1;
    int closest_prescaler = -1;

    /* Test all combinations until we arrive close to the target clock */
    for (int i = 0; i < num_prescalers; ++i)
    {
        for (int k = 0; k < num_scalers; ++k)
        {
            int freq = module_clock / (scalers[k] * prescalers[i]);
            if (freq <= target_clock)
            {
                /* Found closest lower frequency at this prescaler setting,
                 * compare to the best result */
                if (closest_frequency < freq)
                {
                    closest_frequency = freq;
                    closest_scaler = k;
                    closest_prescaler = i;
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

    /* Clear old timing values */
    (*settings) &= ~(SPI_CTAR_BR_MASK | SPI_CTAR_DT_MASK | SPI_CTAR_ASC_MASK |
        SPI_CTAR_CSSCK_MASK | SPI_CTAR_PBR_MASK | SPI_CTAR_PDT_MASK |
        SPI_CTAR_PASC_MASK | SPI_CTAR_PCSSCK_MASK | SPI_CTAR_DBR_MASK);

    /* Set new timings */
    (*settings) |= SPI_CTAR_BR(closest_scaler) | SPI_CTAR_PBR(closest_prescaler) |
        SPI_CTAR_ASC(closest_scaler) | SPI_CTAR_PASC(closest_prescaler) |
        SPI_CTAR_CSSCK(closest_scaler) | SPI_CTAR_PCSSCK(closest_prescaler) |
        SPI_CTAR_DT(closest_scaler) | SPI_CTAR_PDT(closest_prescaler);

    return 0;
}

int spi_init_master(spi_t dev, spi_conf_t conf, spi_speed_t speed)
{
    SPI_Type *spi;
    int module_clock;
    int target_clock;
    uint32_t ctar = 0;

    switch(dev) {
#if SPI_0_EN
        case SPI_0:
            spi = SPI_0_DEV;
            SPI_0_SCLK_PORT_CLKEN();
            SPI_0_MISO_PORT_CLKEN();
            SPI_0_MOSI_PORT_CLKEN();
            SPI_0_CLKEN();
            SPI_0_SCLK_PORT->PCR[SPI_0_SCLK_PIN] &= PORT_PCR_MUX_MASK;
            SPI_0_SCLK_PORT->PCR[SPI_0_SCLK_PIN] |= PORT_PCR_MUX(SPI_0_SCLK_PCR_MUX);
            SPI_0_MOSI_PORT->PCR[SPI_0_MOSI_PIN] &= PORT_PCR_MUX_MASK;
            SPI_0_MOSI_PORT->PCR[SPI_0_MOSI_PIN] |= PORT_PCR_MUX(SPI_0_MOSI_PCR_MUX);
            SPI_0_MISO_PORT->PCR[SPI_0_MISO_PIN] &= PORT_PCR_MUX_MASK;
            SPI_0_MISO_PORT->PCR[SPI_0_MISO_PIN] |= PORT_PCR_MUX(SPI_0_MISO_PCR_MUX);
            module_clock = SPI_0_FREQ;
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            spi = SPI_1_DEV;
            SPI_1_SCLK_PORT_CLKEN();
            SPI_1_MISO_PORT_CLKEN();
            SPI_1_MOSI_PORT_CLKEN();
            SPI_1_CLKEN();
            SPI_1_SCLK_PORT->PCR[SPI_1_SCLK_PIN] &= PORT_PCR_MUX_MASK;
            SPI_1_SCLK_PORT->PCR[SPI_1_SCLK_PIN] |= PORT_PCR_MUX(SPI_1_SCLK_PCR_MUX);
            SPI_1_MOSI_PORT->PCR[SPI_1_MOSI_PIN] &= PORT_PCR_MUX_MASK;
            SPI_1_MOSI_PORT->PCR[SPI_1_MOSI_PIN] |= PORT_PCR_MUX(SPI_1_MOSI_PCR_MUX);
            SPI_1_MISO_PORT->PCR[SPI_1_MISO_PIN] &= PORT_PCR_MUX_MASK;
            SPI_1_MISO_PORT->PCR[SPI_1_MISO_PIN] |= PORT_PCR_MUX(SPI_1_MISO_PCR_MUX);
            module_clock = SPI_1_FREQ;
            break;
#endif
        default:
            return -2;
    }

    switch(speed) {
        case SPI_SPEED_10MHZ:
            target_clock = 10000000;
            break;
        case SPI_SPEED_5MHZ:
            target_clock = 5000000;
            break;
        case SPI_SPEED_1MHZ:
            target_clock = 1000000;
            break;
        case SPI_SPEED_400KHZ:
            target_clock = 400000;
            break;
        case SPI_SPEED_100KHZ:
            target_clock = 100000;
            break;
        default:
            return -1;
    }
    if (target_clock > module_clock / 2)
    {
        /* Too fast, not possible with current module frequency */
        return -1;
    }
    /* for simplicity we are using the same values for baud rate, cs->sck delay
     * and sck->cs delay. This can probably be improved on a per slave device
     * basis. */
    if (find_closest_scalers(module_clock, target_clock, &ctar) < 0)
    {
        /* No solution found for the requested target frequency */
        return -1;
    }

    /* 8 bit per transfer, select clock scalers */
    spi->CTAR[0] = SPI_CTAR_FMSZ(7) | ctar;

    switch (conf) {
        case SPI_CONF_FIRST_RISING:
            spi->CTAR[0] &= ~(SPI_CTAR_CPHA_MASK | SPI_CTAR_CPOL_MASK);
            break;
        case SPI_CONF_SECOND_RISING:
            spi->CTAR[0] &= ~(SPI_CTAR_CPOL_MASK);
            spi->CTAR[0] |= SPI_CTAR_CPHA_MASK;
            break;
        case SPI_CONF_FIRST_FALLING:
            spi->CTAR[0] &= ~(SPI_CTAR_CPHA_MASK);
            spi->CTAR[0] |= SPI_CTAR_CPOL_MASK;
            break;
        case SPI_CONF_SECOND_FALLING:
            spi->CTAR[0] |= SPI_CTAR_CPHA_MASK | SPI_CTAR_CPOL_MASK;
            break;
        default:
            return -2;
    }

    /* Switch to master mode */
    spi->MCR |= SPI_MCR_MSTR_MASK;
    /* Disable halt, enable module, enable FIFOs */
    spi->MCR &= ~(SPI_MCR_HALT_MASK | SPI_MCR_MDIS_MASK | SPI_MCR_DIS_RXF_MASK | SPI_MCR_DIS_TXF_MASK);

    return 0;
}

int spi_init_slave(spi_t dev, spi_conf_t conf, char (*cb)(char))
{
    /* TODO */
    return 0;
}

int spi_transfer_byte(spi_t dev, char out, char *in)
{
    SPI_Type *spi;
    int transfered = 0;
    uint32_t pushr = 0;

    switch(dev) {
#if SPI_0_EN
        case SPI_0:
            spi = SPI_0_DEV;
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            spi = SPI_1_DEV;
            break;
#endif
        default:
            return -1;
    }

    /** @todo K60 SPI: Support transfer options in PUSHR (CTAS, PCS etc) */
    pushr = SPI_PUSHR_TXDATA(out);

    /* Wait for FIFO space */
    while ((spi->SR & SPI_SR_TFFF_MASK) == 0);
    spi->PUSHR = pushr;
    /* Clear the TFFF flag */
    spi->SR |= SPI_SR_TFFF_MASK;
    transfered++;

    /* Wait for RX FIFO to fill */
    while ((spi->SR & SPI_SR_RFDF_MASK) == 0);
    if (in != NULL) {
        /* Write the read byte */
        *in = spi->POPR;
        transfered++;
    }
    else {
        /* Discard FIFO entry */
        uint32_t tmp;
        tmp = spi->POPR;
        (void)tmp;
    }

    DEBUG("\nout: 0x%02x in: 0x%02x transfered: %u\n", out, *in, transfered);

    return transfered;
}

int spi_transfer_bytes(spi_t dev, char *out, char *in, unsigned int length)
{
    int transfered = 0;

    if (out != NULL) {
        DEBUG("out*: %p out: %x length: %x\n", out, *out, length);
        while (length--) {
            int ret = spi_transfer_byte(dev, *(out)++, 0);
            if (ret <  0) {
                return ret;
            }
            transfered += ret;
        }
    }
    if (in != NULL) {
        while (length--) {
            int ret = spi_transfer_byte(dev, 0, in++);
            if (ret <  0) {
                return ret;
            }
            transfered += ret;
        }
        DEBUG("in*: %p in: %x transfered: %x\n", in, *(in-transfered), transfered);
    }

    DEBUG("sent %x byte(s)\n", transfered);
    return transfered;
}

int spi_transfer_reg(spi_t dev, uint8_t reg, char out, char *in)
{
    spi_transfer_byte(dev, reg, NULL);
    return spi_transfer_byte(dev, out, in);
}

int spi_transfer_regs(spi_t dev, uint8_t reg, char *out, char *in, unsigned int length)
{
    spi_transfer_byte(dev, reg, NULL);
    return spi_transfer_bytes(dev, out, in, length);
}

void spi_poweron(spi_t dev)
{
    switch(dev) {
#if SPI_0_EN
        case SPI_0:
            SPI_0_CLKEN();
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            SPI_1_CLKEN();
            break;
#endif
    }
}

void spi_poweroff(spi_t dev)
{
    switch(dev) {
#if SPI_0_EN
        case SPI_0:
            SPI_0_CLKDIS();
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            SPI_1_CLKDIS();
            break;
#endif
    }
}

#endif /* SPI_NUMOF */
