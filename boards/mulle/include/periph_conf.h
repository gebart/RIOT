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
#define TIMER_NUMOF         (2U)
#define TIMER_0_EN          1
#define TIMER_1_EN          1
#define TIMER_IRQ_PRIO      1

/* Timer 0 configuration */
#define TIMER_0_DEV         PIT
#define TIMER_0_CHANNELS    1
#define TIMER_0_MAX_VALUE   (0xffffffff)
#define TIMER_0_CLKEN()     (SIM->SCGC6 |= SIM_SCGC6_PIT_MASK)
#define TIMER_0_CLOCK       SystemBusClock
#define TIMER_0_ISR         isr_pit0
#define TIMER_0_IRQ_CHAN    PIT0_IRQn

/* Timer 1 configuration */
#define TIMER_1_DEV         PIT
#define TIMER_1_CHANNELS    1
#define TIMER_1_MAX_VALUE   (0xffffffff)
#define TIMER_1_CLKEN()     (SIM->SCGC6 |= SIM_SCGC6_PIT_MASK)
#define TIMER_1_CLOCK       SystemBusClock
#define TIMER_1_ISR         isr_pit1
#define TIMER_1_IRQ_CHAN    PIT1_IRQn

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
#define UART_0_CLKEN()      (SIM->SCGC4 |= SIM_SCGC4_UART1_MASK)
#define UART_0_CLKDIS()     (SIM->SCGC4 &= ~(SIM_SCGC4_UART1_MASK))
#define UART_0_CLK          (SystemSysClock)
#define UART_0_IRQ_CHAN     UART1_RX_TX_IRQn
#define UART_0_ISR          isr_uart1_status
/* UART 0 pin configuration */
#define UART_0_PORT_CLKEN() (SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK)
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

/* ADC 0 configuration */
#define ADC_0_DEV           ADC1
#define ADC_0_CHANNELS      2
#define ADC_0_CLKEN()       (RCC->APB2ENR |= RCC_APB2ENR_ADC1EN)
#define ADC_0_CLKDIS()      (RCC->APB2ENR &= ~(RCC_APB2ENR_ADC1EN))
#define ADC_0_PORT          GPIOA
#define ADC_0_PORT_CLKEN()  (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN)
/* ADC 0 channel 0 pin config */
#define ADC_0_CH0           1
#define ADC_0_CH0_PIN       1
/* ADC 0 channel 1 pin config */
#define ADC_0_CH1           4
#define ADC_0_CH1_PIN       4

/* ADC 0 configuration */
#define ADC_1_DEV           ADC2
#define ADC_1_CHANNELS      2
#define ADC_1_CLKEN()       (RCC->APB2ENR |= RCC_APB2ENR_ADC2EN)
#define ADC_1_CLKDIS()      (RCC->APB2ENR &= ~(RCC_APB2ENR_ADC2EN))
#define ADC_1_PORT          GPIOC
#define ADC_1_PORT_CLKEN()  (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN)
/* ADC 0 channel 0 pin config */
#define ADC_1_CH0           11
#define ADC_1_CH0_PIN       1
/* ADC 0 channel 1 pin config */
#define ADC_1_CH1           12
#define ADC_1_CH1_PIN       2
/** @} */


/**
 * @name PWM configuration
 * @{
 */
#define PWM_NUMOF           (0U)
#define PWM_0_EN            1
#define PWM_1_EN            1
#define PWM_MAX_CHANNELS    4

/* PWM 0 device configuration */
#define PWM_0_DEV           TIM1
#define PWM_0_CHANNELS      4
#define PWM_0_CLK           (168000000U)
#define PWM_0_CLKEN()       (RCC->APB2ENR |= RCC_APB2ENR_TIM1EN)
#define PWM_0_CLKDIS()      (RCC->APB2ENR &= ~RCC_APB2ENR_TIM1EN)
/* PWM 0 pin configuration */
#define PWM_0_PORT          GPIOE
#define PWM_0_PORT_CLKEN()  (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN)
#define PWM_0_PIN_CH0       9
#define PWM_0_PIN_CH1       11
#define PWM_0_PIN_CH2       13
#define PWM_0_PIN_CH3       14
#define PWM_0_PIN_AF        1

/* PWM 1 device configuration */
#define PWM_1_DEV           TIM3
#define PWM_1_CHANNELS      3
#define PWM_1_CLK           (84000000U)
#define PWM_1_CLKEN()       (RCC->APB1ENR |= RCC_APB1ENR_TIM3EN)
#define PWM_1_CLKDIS()      (RCC->APB1ENR &= ~RCC_APB1ENR_TIM3EN)
/* PWM 1 pin configuration */
#define PWM_1_PORT          GPIOB
#define PWM_1_PORT_CLKEN()  (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)
#define PWM_1_PIN_CH0       4
#define PWM_1_PIN_CH1       5
#define PWM_1_PIN_CH2       0
#define PWM_1_PIN_CH3       1
#define PWM_1_PIN_AF        2
/** @} */


/**
 * @name SPI configuration
 * @{
 */
#define SPI_NUMOF           1
#define SPI_0_EN            1
#define SPI_1_EN            0

/* SPI 0 device config */
#define SPI_0_DEV               SPI0
#define SPI_0_CLKEN()           (SIM->SCGC6 |= SIM_SCGC6_SPI0_MASK)
#define SPI_0_CLKDIS()          (SIM->SCGC6 &= ~(SIM_SCGC6_SPI0_MASK))
#define SPI_0_IRQ               SPI0_IRQn
#define SPI_0_ISR               isr_spi0
#define SPI_0_IRQ_PRIO          1
#define SPI_0_FREQ              SystemBusClock
/* SPI 0 pin configuration */
#define SPI_0_SCK_PIN           1
#define SPI_0_SCK_PORT          PORTD
#define SPI_0_SCK_PORT_CLKEN()  (SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK)
#define SPI_0_SCK_AF            2
#define SPI_0_SIN_PIN           3
#define SPI_0_SIN_PORT          PORTD
#define SPI_0_SIN_PORT_CLKEN()  (SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK)
#define SPI_0_SIN_AF            2
#define SPI_0_SOUT_PIN          2
#define SPI_0_SOUT_PORT         PORTD
#define SPI_0_SOUT_PORT_CLKEN() (SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK)
#define SPI_0_SOUT_AF  2
#define SPI_0_PCS0_PIN          0
#define SPI_0_PCS0_PORT         PORTD
#define SPI_0_PCS0_PORT_CLKEN() (SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK)
#define SPI_0_PCS0_AF           2
/* SPI chip select polarity */
#define SPI_0_PCS0_ACTIVE_LOW   1
#define SPI_0_PCS1_ACTIVE_LOW   1
#define SPI_0_PCS2_ACTIVE_LOW   1
#define SPI_0_PCS3_ACTIVE_LOW   1

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

/** @} */

/** @} */


/**
 * @name I2C configuration
 * @{
 */
#define I2C_NUMOF           (0U)                                                /* TODO !!!!!!! */
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
#define GPIO_NUMOF          15
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
#define GPIO_15_EN          0
#define GPIO_IRQ_PRIO       1

/* GPIO channel 0 config */
/* Red LED */
#define GPIO_0_PORT         PORTC
#define GPIO_0_DEV          PTC
#define GPIO_0_PIN          15
#define GPIO_0_CLKEN()      (SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK)
#define GPIO_0_IRQ          PORTC_IRQn
#define GPIO_0_ISR          isr_portc_pin_detect

/* GPIO channel 1 config */
/* Yellow LED */
#define GPIO_1_PORT         PORTC
#define GPIO_1_DEV          PTC
#define GPIO_1_PIN          14
#define GPIO_1_CLKEN()      (SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK)
#define GPIO_1_IRQ          PORTC_IRQn
#define GPIO_1_ISR          isr_portc_pin_detect

/* GPIO channel 2 config */
/* Green LED */
#define GPIO_2_PORT         PORTC
#define GPIO_2_DEV          PTC
#define GPIO_2_PIN          13
#define GPIO_2_CLKEN()      (SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK)
#define GPIO_2_IRQ          PORTC_IRQn
#define GPIO_2_ISR          isr_portc_pin_detect

/* GPIO channel 3 config */
/* LIS3DH INT1 */
#define GPIO_3_PORT         PORTC
#define GPIO_3_DEV          PTC
#define GPIO_3_PIN          18
#define GPIO_3_CLKEN()      (SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK)
#define GPIO_3_IRQ          PORTC_IRQn
#define GPIO_3_ISR          isr_portc_pin_detect

/* GPIO channel 4 config */
/* LIS3DH INT2 */
#define GPIO_4_PORT         PORTC
#define GPIO_4_DEV          PTC
#define GPIO_4_PIN          17
#define GPIO_4_CLKEN()      (SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK)
#define GPIO_4_IRQ          PORTC_IRQn
#define GPIO_4_ISR          isr_portc_pin_detect

/* GPIO channel 5 config */
/* VSEC enable */
#define GPIO_5_PORT         PORTB
#define GPIO_5_DEV          PTB
#define GPIO_5_PIN          16
#define GPIO_5_CLKEN()      (SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK)
#define GPIO_5_IRQ          PORTB_IRQn
#define GPIO_5_ISR          isr_portb_pin_detect

/* GPIO channel 6 config */
/* AVDD enable */
#define GPIO_6_PORT         PORTB
#define GPIO_6_DEV          PTB
#define GPIO_6_PIN          17
#define GPIO_6_CLKEN()      (SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK)
#define GPIO_6_IRQ          PORTB_IRQn
#define GPIO_6_ISR          isr_portb_pin_detect

/* GPIO channel 7 config */
/* VPERIPH enable */
#define GPIO_7_PORT         PORTD
#define GPIO_7_DEV          PTD
#define GPIO_7_PIN          7
#define GPIO_7_CLKEN()      (SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK)
#define GPIO_7_IRQ          PORTD_IRQn
#define GPIO_7_ISR          isr_portd_pin_detect

/* GPIO channel 8 config */
/* MC34673 enable */
#define GPIO_8_PORT         PORTB
#define GPIO_8_DEV          PTB
#define GPIO_8_PIN          23
#define GPIO_8_CLKEN()      (SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK)
#define GPIO_8_IRQ          PORTB_IRQn
#define GPIO_8_ISR          isr_portb_pin_detect

/* GPIO channel 9 config */
/* MC34673 CHG */
#define GPIO_9_PORT         PORTB
#define GPIO_9_DEV          PTB
#define GPIO_9_PIN          22
#define GPIO_9_CLKEN()      (SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK)
#define GPIO_9_IRQ          PORTB_IRQn
#define GPIO_9_ISR          isr_portb_pin_detect

/* GPIO channel 10 config */
/* MC34673 PPR */
#define GPIO_10_PORT        PORTB
#define GPIO_10_DEV         PTB
#define GPIO_10_PIN         9
#define GPIO_10_CLKEN()     (SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK)
#define GPIO_10_IRQ         PORTB_IRQn
#define GPIO_10_ISR         isr_portb_pin_detect

/* GPIO channel 11 config */
/* MC34673 FAST */
#define GPIO_11_PORT        PORTB
#define GPIO_11_DEV         PTB
#define GPIO_11_PIN         10
#define GPIO_11_CLKEN()     (SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK)
#define GPIO_11_IRQ         PORTB_IRQn
#define GPIO_11_ISR         isr_portb_pin_detect

/* GPIO channel 12 config */
/* AT86RF212 IRQ */
#define GPIO_12_PORT        PORTB
#define GPIO_12_DEV         PTB
#define GPIO_12_PIN         9
#define GPIO_12_CLKEN()     (SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK)
#define GPIO_12_IRQ         PORTB_IRQn
#define GPIO_12_ISR         isr_portb_pin_detect

/* GPIO channel 13 config */
/* AT86RF212 SLP_TR */
#define GPIO_13_PORT        PORTE
#define GPIO_13_DEV         PTE
#define GPIO_13_PIN         6
#define GPIO_13_CLKEN()     (SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK)
#define GPIO_13_IRQ         PORTE_IRQn
#define GPIO_13_ISR         isr_porte_pin_detect

/* GPIO channel 14 config */
/* AT86RF212 SS */
#define GPIO_14_PORT        PORTD
#define GPIO_14_DEV         PTD
#define GPIO_14_PIN         4
#define GPIO_14_CLKEN()     (SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK)
#define GPIO_14_IRQ         PORTD_IRQn
#define GPIO_14_ISR         isr_portd_pin_detect

/* GPIO channel 15 config */
/* Not configured */
#define GPIO_15_PORT        PORTB
#define GPIO_15_DEV         PTB
#define GPIO_15_PIN         1
#define GPIO_15_CLKEN()     (SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK)
#define GPIO_15_IRQ         PORTB_IRQn
#define GPIO_15_ISR         isr_portb_pin_detect

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

#endif /* __PERIPH_CONF_H */
/** @} */
