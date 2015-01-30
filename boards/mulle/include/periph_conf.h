/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */


/**
 * @ingroup     board_mulle
 * @{
 *
 * @file
 * @name        Peripheral MCU configuration for the Eistec Mulle
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 */

#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name Clock system configuration
 * @{
 */

#include "config-clocks.h"

/** @} */

/**
 * @name Timer configuration
 * @{
 */
#define TIMER_NUMOF             (1U)
#define TIMER_0_EN              1
#define TIMER_1_EN              0
#define TIMER_IRQ_PRIO          1
#define TIMER_DEV               PIT
#define TIMER_MAX_VALUE         (0xffffffff)
#define TIMER_CLOCK             SystemBusClock
#define TIMER_CLKEN()           (BITBAND_REG(SIM->SCGC6, SIM_SCGC6_PIT_SHIFT) = 1)

// Timer 0 configuration
#define TIMER_0_PRESCALER_CH    0
#define TIMER_0_COUNTER_CH      1
#define TIMER_0_ISR             isr_pit1
#define TIMER_0_IRQ_CHAN        PIT1_IRQn

// Timer 1 configuration
#define TIMER_1_PRESCALER_CH    2
#define TIMER_1_COUNTER_CH      3
#define TIMER_1_ISR             isr_pit3
#define TIMER_1_IRQ_CHAN        PIT3_IRQn

/** @} */


/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           1
#define UART_1_EN           0
#define UART_2_EN           0
#define UART_3_EN           0
#define UART_4_EN           0
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          UART1
#define UART_0_CLKEN()      (BITBAND_REG(SIM->SCGC4, SIM_SCGC4_UART1_SHIFT) = 1)
#define UART_0_CLKDIS()     (BITBAND_REG(SIM->SCGC4, SIM_SCGC4_UART1_SHIFT) = 0)
#define UART_0_CLK          (SystemSysClock)
#define UART_0_IRQ_CHAN     UART1_RX_TX_IRQn
#define UART_0_ISR          isr_uart1_status
/* UART 0 pin configuration */
#define UART_0_PORT_CLKEN() (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTC_SHIFT) = 1)
#define UART_0_PORT         PORTC
#define UART_0_TX_PIN       4
#define UART_0_RX_PIN       3
/* Function number in pin multiplex, see K60 Sub-Family Reference Manual,
 * section 10.3.1 K60 Signal Multiplexing and Pin Assignments */
#define UART_0_AF           3
#define UART_0_TX_PCR_MUX   3
#define UART_0_RX_PCR_MUX   3

/** @} */


/**
 * @name ADC configuration
 * @{
 */
#define ADC_NUMOF           (0U)
#define ADC_0_EN            1
#define ADC_1_EN            1
#define ADC_MAX_CHANNELS    2

/** @} */


/**
 * @name PWM configuration
 * @{
 */
#define PWM_NUMOF           (0U)
#define PWM_0_EN            1
#define PWM_1_EN            1
#define PWM_MAX_CHANNELS    4

/** @} */


/**
 * @name SPI configuration
 * @{
 */
#define SPI_NUMOF           3
#define SPI_0_EN            1
#define SPI_1_EN            1
#define SPI_2_EN            1
#define SPI_3_EN            0
#define SPI_4_EN            0
#define SPI_5_EN            0
#define SPI_6_EN            0
#define SPI_7_EN            0

#define MULLE_PASTE_PARTS(left, index, right) MULLE_PASTE_PARTS2(left, index, right)
#define MULLE_PASTE_PARTS2(left, index, right) left##index##right

/* SPI 0 device config */
/* SPI_0 (in RIOT) is mapped to SPI0, CTAS=0 in hardware */
#define SPI_0_INDEX             0
#define SPI_0_CTAS              0
#define SPI_0_DEV               MULLE_PASTE_PARTS(SPI, SPI_0_INDEX, )
#define SPI_0_CLKEN()           (BITBAND_REG(SIM->SCGC6, SIM_SCGC6_SPI0_SHIFT) = 1)
#define SPI_0_CLKDIS()          (BITBAND_REG(SIM->SCGC6, SIM_SCGC6_SPI0_SHIFT) = 0)
#define SPI_0_IRQ               MULLE_PASTE_PARTS(SPI, SPI_0_INDEX, _IRQn)
#define SPI_0_IRQ_HANDLER       MULLE_PASTE_PARTS(isr_spi, SPI_0_INDEX, )
#define SPI_0_IRQ_PRIO          1
#define SPI_0_FREQ              SystemBusClock
/* SPI 0 pin configuration */
#define SPI_0_SCK_PORT          PORTD
#define SPI_0_SCK_PIN           1
#define SPI_0_SCK_PORT_CLKEN()  (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define SPI_0_SCK_AF            2
#define SPI_0_SIN_PORT          PORTD
#define SPI_0_SIN_PIN           3
#define SPI_0_SIN_PORT_CLKEN()  (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define SPI_0_SIN_AF            2
#define SPI_0_SOUT_PORT         PORTD
#define SPI_0_SOUT_PIN          2
#define SPI_0_SOUT_PORT_CLKEN() (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define SPI_0_SOUT_AF  2
#define SPI_0_PCS0_PORT         PORTD
#define SPI_0_PCS0_PIN          0
#define SPI_0_PCS0_PORT_CLKEN() (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define SPI_0_PCS0_AF           2
/* SPI chip select polarity */
#define SPI_0_PCS0_ACTIVE_LOW   1
#define SPI_0_PCS1_ACTIVE_LOW   1
#define SPI_0_PCS2_ACTIVE_LOW   1
#define SPI_0_PCS3_ACTIVE_LOW   1

/* SPI 1 device config */
/* SPI_1 (in RIOT) is mapped to SPI1, CTAS=0 in hardware */
#define SPI_1_INDEX             1
#define SPI_1_CTAS              0
#define SPI_1_DEV               MULLE_PASTE_PARTS(SPI, SPI_1_INDEX, )
#define SPI_1_CLKEN()           (BITBAND_REG(SIM->SCGC6, SIM_SCGC6_SPI1_SHIFT) = 1)
#define SPI_1_CLKDIS()          (BITBAND_REG(SIM->SCGC6, SIM_SCGC6_SPI1_SHIFT) = 0)
#define SPI_1_IRQ               MULLE_PASTE_PARTS(SPI, SPI_1_INDEX, _IRQn)
#define SPI_1_IRQ_HANDLER       MULLE_PASTE_PARTS(isr_spi, SPI_1_INDEX, )
#define SPI_1_IRQ_PRIO          1
#define SPI_1_FREQ              SystemBusClock
/* SPI 0 pin configuration */
#define SPI_1_SCK_PORT          PORTE
#define SPI_1_SCK_PIN           2
#define SPI_1_SCK_PORT_CLKEN()  (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTE_SHIFT) = 1)
#define SPI_1_SCK_AF            2
#define SPI_1_SIN_PORT          PORTE
#define SPI_1_SIN_PIN           3
#define SPI_1_SIN_PORT_CLKEN()  (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTE_SHIFT) = 1)
#define SPI_1_SIN_AF            2
#define SPI_1_SOUT_PORT         PORTE
#define SPI_1_SOUT_PIN          1
#define SPI_1_SOUT_PORT_CLKEN() (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTE_SHIFT) = 1)
#define SPI_1_SOUT_AF  2
#define SPI_1_PCS0_PORT         PORTE
#define SPI_1_PCS0_PIN          4
#define SPI_1_PCS0_PORT_CLKEN() (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTE_SHIFT) = 1)
#define SPI_1_PCS0_AF           2
/* SPI chip select polarity */
#define SPI_1_PCS0_ACTIVE_LOW   1
#define SPI_1_PCS1_ACTIVE_LOW   1
#define SPI_1_PCS2_ACTIVE_LOW   1
#define SPI_1_PCS3_ACTIVE_LOW   1

/* SPI 2 device config */
/* SPI_2 (in RIOT) is mapped to SPI0, CTAS=1 in hardware */
#define SPI_2_INDEX             0
#define SPI_2_CTAS              1
#define SPI_2_DEV               MULLE_PASTE_PARTS(SPI, SPI_2_INDEX, )
#define SPI_2_CLKEN()           (BITBAND_REG(SIM->SCGC6, SIM_SCGC6_SPI0_SHIFT) = 1)
#define SPI_2_CLKDIS()          (BITBAND_REG(SIM->SCGC6, SIM_SCGC6_SPI0_SHIFT) = 0)
#define SPI_2_IRQ               MULLE_PASTE_PARTS(SPI, SPI_2_INDEX, _IRQn)
/* #define SPI_2_IRQ_HANDLER       MULLE_PASTE_PARTS(isr_spi, SPI_2_INDEX, ) */
#define SPI_2_IRQ_PRIO          1
#define SPI_2_FREQ              SystemBusClock
/* SPI 2 pin configuration, must be the same as the other RIOT device using this
 * hardware module */
#define SPI_2_SCK_PORT          PORTD
#define SPI_2_SCK_PIN           1
#define SPI_2_SCK_PORT_CLKEN()  (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define SPI_2_SCK_AF            2
#define SPI_2_SIN_PORT          PORTD
#define SPI_2_SIN_PIN           3
#define SPI_2_SIN_PORT_CLKEN()  (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define SPI_2_SIN_AF            2
#define SPI_2_SOUT_PORT         PORTD
#define SPI_2_SOUT_PIN          2
#define SPI_2_SOUT_PORT_CLKEN() (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define SPI_2_SOUT_AF  2
#define SPI_2_PCS0_PORT         PORTD
#define SPI_2_PCS0_PIN          0
#define SPI_2_PCS0_PORT_CLKEN() (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define SPI_2_PCS0_AF           2
/* SPI chip select polarity */
#define SPI_2_PCS0_ACTIVE_LOW   1
#define SPI_2_PCS1_ACTIVE_LOW   1
#define SPI_2_PCS2_ACTIVE_LOW   1
#define SPI_2_PCS3_ACTIVE_LOW   1

/**
 * @name SPI delay timing configuration
 * @{ */
/* These values are necessary for communicating with the AT86RF212B when running
 * the MCU core at high clock frequencies. */
/* NB: The given values are the reciprocals of the time, in order to compute the
 * scalers using only integer math. */
#define SPI_0_TCSC_FREQ (5555555) /* It looks silly, but this is correct. 1/180e-9 */
#define SPI_0_TASC_FREQ (5454545) /* It looks silly, but this is correct. 1/183e-9 */
#define SPI_0_TDT_FREQ  (4000000) /* 1/250e-9 */

/* SPI_1 timings */
#define SPI_1_TCSC_FREQ (0)
#define SPI_1_TASC_FREQ (0)
#define SPI_1_TDT_FREQ  (0)

/* SPI_2 timings */
#define SPI_2_TCSC_FREQ (0)
#define SPI_2_TASC_FREQ (0)
#define SPI_2_TDT_FREQ  (0)

/** @} */

/** @} */


/**
 * @name I2C configuration
 * @{
 */
#define I2C_NUMOF           (0U)                                                /* TODO !!!!!!!*/
#define I2C_0_EN            0
#define I2C_0_EN            0

/* SPI 0 device configuration */
#define I2C_0_DEV
#define I2C_0_CLKEN()
#define I2C_0_ISR
#define I2C_0_IRQ
#define I2C_0_IRQ_PRIO
/* SPI 0 pin configuration */
#define I2C_0_PORT
#define I2C_0_PINS
#define I2C_0_PORT_CLKEN()
#define I2C_0_SCL_AFCFG()
#define I2C_0_SDA_AFCFG()

/* SPI 1 device configuration */
#define I2C_1_DEV
#define I2C_1_CLKEN()
#define I2C_1_ISR
#define I2C_1_IRQ
#define I2C_1_IRQ_PRIO
/* SPI 1 pin configuration */
#define I2C_1_PORT
#define I2C_1_PINS
#define I2C_1_PORT_CLKEN()
#define I2C_1_SCL_AFCFG()
#define I2C_1_SDA_AFCFG()
/** @} */


/**
 * @name GPIO configuration
 * @{
 */
#define GPIO_NUMOF          17
#define GPIO_0_EN           1
#define GPIO_1_EN           1
#define GPIO_2_EN           1
#define GPIO_3_EN           1
#define GPIO_4_EN           1
#define GPIO_5_EN           1
#define GPIO_6_EN           1
#define GPIO_7_EN           1
#define GPIO_8_EN           1
#define GPIO_9_EN           1
#define GPIO_10_EN          1
#define GPIO_11_EN          1
#define GPIO_12_EN          1
#define GPIO_13_EN          1
#define GPIO_14_EN          1
#define GPIO_15_EN          1
#define GPIO_16_EN          1
#define GPIO_17_EN          1
#define GPIO_IRQ_PRIO       1

/* GPIO channel 0 config */
/* Red LED */
#define GPIO_0_PORT         PORTC
#define GPIO_0_PORT_BASE    PORTC_BASE
#define GPIO_0_DEV          PTC
#define GPIO_0_PIN          15
#define GPIO_0_CLKEN()      (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTC_SHIFT) = 1)
#define GPIO_0_IRQ          PORTC_IRQn
#define GPIO_0_ISR          isr_portc_pin_detect

/* GPIO channel 1 config */
/* Yellow LED */
#define GPIO_1_PORT         PORTC
#define GPIO_1_PORT_BASE    PORTC_BASE
#define GPIO_1_DEV          PTC
#define GPIO_1_PIN          14
#define GPIO_1_CLKEN()      (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTC_SHIFT) = 1)
#define GPIO_1_IRQ          PORTC_IRQn
#define GPIO_1_ISR          isr_portc_pin_detect

/* GPIO channel 2 config */
/* Green LED */
#define GPIO_2_PORT         PORTC
#define GPIO_2_PORT_BASE    PORTC_BASE
#define GPIO_2_DEV          PTC
#define GPIO_2_PIN          13
#define GPIO_2_CLKEN()      (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTC_SHIFT) = 1)
#define GPIO_2_IRQ          PORTC_IRQn
#define GPIO_2_ISR          isr_portc_pin_detect

/* GPIO channel 3 config */
/* LIS3DH INT1 */
#define GPIO_3_PORT         PORTC
#define GPIO_3_PORT_BASE    PORTC_BASE
#define GPIO_3_DEV          PTC
#define GPIO_3_PIN          18
#define GPIO_3_CLKEN()      (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTC_SHIFT) = 1)
#define GPIO_3_IRQ          PORTC_IRQn
#define GPIO_3_ISR          isr_portc_pin_detect

/* GPIO channel 4 config */
/* LIS3DH INT2 */
#define GPIO_4_PORT         PORTC
#define GPIO_4_PORT_BASE    PORTC_BASE
#define GPIO_4_DEV          PTC
#define GPIO_4_PIN          17
#define GPIO_4_CLKEN()      (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTC_SHIFT) = 1)
#define GPIO_4_IRQ          PORTC_IRQn
#define GPIO_4_ISR          isr_portc_pin_detect

/* GPIO channel 5 config */
/* VSEC enable */
#define GPIO_5_PORT         PORTB
#define GPIO_5_PORT_BASE    PORTB_BASE
#define GPIO_5_DEV          PTB
#define GPIO_5_PIN          16
#define GPIO_5_CLKEN()      (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTB_SHIFT) = 1)
#define GPIO_5_IRQ          PORTB_IRQn
#define GPIO_5_ISR          isr_portb_pin_detect

/* GPIO channel 6 config */
/* AVDD enable */
#define GPIO_6_PORT         PORTB
#define GPIO_6_PORT_BASE    PORTB_BASE
#define GPIO_6_DEV          PTB
#define GPIO_6_PIN          17
#define GPIO_6_CLKEN()      (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTB_SHIFT) = 1)
#define GPIO_6_IRQ          PORTB_IRQn
#define GPIO_6_ISR          isr_portb_pin_detect

/* GPIO channel 7 config */
/* VPERIPH enable */
#define GPIO_7_PORT         PORTD
#define GPIO_7_PORT_BASE    PORTD_BASE
#define GPIO_7_DEV          PTD
#define GPIO_7_PIN          7
#define GPIO_7_CLKEN()      (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define GPIO_7_IRQ          PORTD_IRQn
#define GPIO_7_ISR          isr_portd_pin_detect

/* GPIO channel 8 config */
/* MC34673 enable */
#define GPIO_8_PORT         PORTB
#define GPIO_8_PORT_BASE    PORTB_BASE
#define GPIO_8_DEV          PTB
#define GPIO_8_PIN          23
#define GPIO_8_CLKEN()      (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTB_SHIFT) = 1)
#define GPIO_8_IRQ          PORTB_IRQn
#define GPIO_8_ISR          isr_portb_pin_detect

/* GPIO channel 9 config */
/* MC34673 CHG */
#define GPIO_9_PORT         PORTB
#define GPIO_9_PORT_BASE    PORTB_BASE
#define GPIO_9_DEV          PTB
#define GPIO_9_PIN          22
#define GPIO_9_CLKEN()      (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTB_SHIFT) = 1)
#define GPIO_9_IRQ          PORTB_IRQn
#define GPIO_9_ISR          isr_portb_pin_detect

/* GPIO channel 10 config */
/* MC34673 PPR */
#define GPIO_10_PORT        PORTB
#define GPIO_10_PORT_BASE   PORTB_BASE
#define GPIO_10_DEV         PTB
#define GPIO_10_PIN         9
#define GPIO_10_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTB_SHIFT) = 1)
#define GPIO_10_IRQ         PORTB_IRQn
#define GPIO_10_ISR         isr_portb_pin_detect

/* GPIO channel 11 config */
/* MC34673 FAST */
#define GPIO_11_PORT        PORTB
#define GPIO_11_PORT_BASE   PORTB_BASE
#define GPIO_11_DEV         PTB
#define GPIO_11_PIN         10
#define GPIO_11_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTB_SHIFT) = 1)
#define GPIO_11_IRQ         PORTB_IRQn
#define GPIO_11_ISR         isr_portb_pin_detect

/* GPIO channel 12 config */
/* AT86RF212 IRQ */
#define GPIO_12_PORT        PORTB
#define GPIO_12_PORT_BASE   PORTB_BASE
#define GPIO_12_DEV         PTB
#define GPIO_12_PIN         9
#define GPIO_12_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTB_SHIFT) = 1)
#define GPIO_12_IRQ         PORTB_IRQn
#define GPIO_12_ISR         isr_portb_pin_detect

/* GPIO channel 13 config */
/* AT86RF212 SLP_TR */
#define GPIO_13_PORT        PORTE
#define GPIO_13_PORT_BASE   PORTE_BASE
#define GPIO_13_DEV         PTE
#define GPIO_13_PIN         6
#define GPIO_13_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTE_SHIFT) = 1)
#define GPIO_13_IRQ         PORTE_IRQn
#define GPIO_13_ISR         isr_porte_pin_detect

/* GPIO channel 14 config */
/* AT86RF212 SS */
#define GPIO_14_PORT        PORTD
#define GPIO_14_PORT_BASE   PORTD_BASE
#define GPIO_14_DEV         PTD
#define GPIO_14_PIN         4
#define GPIO_14_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define GPIO_14_IRQ         PORTD_IRQn
#define GPIO_14_ISR         isr_portd_pin_detect

/* GPIO channel 15 config */
/* LIS3DH CS */
#define GPIO_15_PORT        PORTD
#define GPIO_15_PORT_BASE   PORTD_BASE
#define GPIO_15_DEV         PTD
#define GPIO_15_PIN         0
#define GPIO_15_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define GPIO_15_IRQ         PORTD_IRQn
#define GPIO_15_ISR         isr_portd_pin_detect

/* GPIO channel 16 config */
/* FM25L04B CS */
#define GPIO_16_PORT        PORTD
#define GPIO_16_PORT_BASE   PORTD_BASE
#define GPIO_16_DEV         PTD
#define GPIO_16_PIN         6
#define GPIO_16_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define GPIO_16_IRQ         PORTD_IRQn
#define GPIO_16_ISR         isr_portd_pin_detect

/* GPIO channel 17 config */
/* M25P16 CS */
#define GPIO_17_PORT        PORTD
#define GPIO_17_PORT_BASE   PORTD_BASE
#define GPIO_17_DEV         PTD
#define GPIO_17_PIN         5
#define GPIO_17_CLKEN()     (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT) = 1)
#define GPIO_17_IRQ         PORTD_IRQn
#define GPIO_17_ISR         isr_portd_pin_detect

/** @} */


/**
* @name RTC configuration
* @{
*/
#define RTC_NUMOF           (1U)
#define RTC_DEV             RTC
#define RTC_UNLOCK()        (BITBAND_REG(SIM->SCGC6, SIM_SCGC6_RTC_SHIFT) = 1)
/**
 * RTC crystal load capacitance configuration bits.
 */
/* enable 12pF load capacitance, might need adjusting.. */
#define RTC_LOAD_CAP_BITS   (RTC_CR_SC8P_MASK | RTC_CR_SC4P_MASK)
/** @} */

/**
 * @name Random Number Generator configuration
 * @{
 */
#define RANDOM_NUMOF            (1U)
#define RANDOM_CLKEN()          (BITBAND_REG(SIM->SCGC3, SIM_SCGC3_RNGA_SHIFT) = 1)
#define RANDOM_CLKDIS()         (BITBAND_REG(SIM->SCGC3, SIM_SCGC3_RNGA_SHIFT) = 0)
#define RANDOM_RNGA_BASE        (RNG)

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __PERIPH_CONF_H */
/** @} */
