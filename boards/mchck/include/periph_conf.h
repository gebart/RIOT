/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_mchck
 * @{
 *
 * @file
 * @name       Peripheral MCU configuration for the mchck board
 *
 * @author      Finn Wilke <finn.wilke@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 */

#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "k20_periph.h"

/**
 * @name Timer configuration
 * @{
 */
/*#define TIMER_NUMOF         (1)
#define TIMER_0_EN          1
#define TIMER_IRQ_PRIO      1*/
/** @} */


/**
 * @name UART configuration
 * @{
 */
#ifndef UART_NUMOF
#define UART_NUMOF 1
#endif

#if !defined(UART_0_EN) && !defined(UART_1_EN) && !defined(UART_2_EN)
#if UART_NUMOF >= 1
#define UART_0_EN 1
#else
#define UART_0_EN 0
#endif

#if UART_NUMOF >= 2
#define UART_1_EN 1
#else
#define UART_1_EN 0
#endif

#define UART_2_EN 0

#define UART_IRQ_PRIO 1

#ifndef K20_UART0_OUTPUT_PORT
#define K20_UART0_OUTPUT_PORT K20_UART0_OUTPUT_PORT_A
#endif
#endif
/** @} */

/**
 * @name GPIO configuration
 *
 * Just all the pins from https://github.com/mchck/mchck/wiki/Pinout top to bottom
 * @{
 */
#define GPIO_IRQ_PRIO 1

/* Left (bottom) side */
// PL1
// DP0

// PL2
// DM0

// PL3
// VFREF

// PL4
// UART0_PTA
#define GPIO_0_EN 1
#define GPIO_0_IRQ PORTA_IRQn
#define GPIO_0_CLKEN PORTA_CLKEN
#define GPIO_0_PORT PORTA
#define GPIO_0_DEV PTA
#define GPIO_0_PIN 1

// PL5
#define GPIO_1_EN 1
#define GPIO_1_IRQ PORTA_IRQn
#define GPIO_1_CLKEN PORTA_CLKEN
#define GPIO_1_PORT PORTA
#define GPIO_1_DEV PTA
#define GPIO_1_PIN 2

// PL6
#define GPIO_2_EN 1
#define GPIO_2_IRQ PORTA_IRQn
#define GPIO_2_CLKEN PORTA_CLKEN
#define GPIO_2_PORT PORTA
#define GPIO_2_DEV PTA
#define GPIO_2_PIN 4

// PL7
#define GPIO_3_EN 1
#define GPIO_3_IRQ PORTA_IRQn
#define GPIO_3_CLKEN PORTA_CLKEN
#define GPIO_3_PORT PORTA
#define GPIO_3_DEV PTA
#define GPIO_3_PIN 18

// PL8
#define GPIO_4_EN 1
#define GPIO_4_IRQ PORTA_IRQn
#define GPIO_4_CLKEN PORTA_CLKEN
#define GPIO_4_PORT PORTA
#define GPIO_4_DEV PTA
#define GPIO_4_PIN 19

// PL9
#define GPIO_5_EN 1
#define GPIO_5_IRQ PORTB_IRQn
#define GPIO_5_CLKEN PORTB_CLKEN
#define GPIO_5_PORT PORTB
#define GPIO_5_DEV PTB
#define GPIO_5_PIN 0

// PL10
#define GPIO_6_EN 1
#define GPIO_6_IRQ PORTB_IRQn
#define GPIO_6_CLKEN PORTB_CLKEN
#define GPIO_6_PORT PORTB
#define GPIO_6_DEV PTB
#define GPIO_6_PIN 1

// PL11
#define GPIO_7_EN 1
#define GPIO_7_IRQ PORTB_IRQn
#define GPIO_7_CLKEN PORTB_CLKEN
#define GPIO_7_PORT PORTB
#define GPIO_7_DEV PTB
#define GPIO_7_PIN 2

// PL12
#define GPIO_8_EN 1
#define GPIO_8_IRQ PORTB_IRQn
#define GPIO_8_CLKEN PORTB_CLKEN
#define GPIO_8_PORT PORTB
#define GPIO_8_DEV PTB
#define GPIO_8_PIN 3

// PL13
// GND

// PL14
// 3.3V

/* Right (top) side */
// PU1
#define GPIO_9_EN 1
#define GPIO_9_IRQ PORTD_IRQn
#define GPIO_9_CLKEN PORTD_CLKEN
#define GPIO_9_PORT PORTD
#define GPIO_9_DEV PTD
#define GPIO_9_PIN 7

// PU2
#define GPIO_10_EN 1
#define GPIO_10_IRQ PORTD_IRQn
#define GPIO_10_CLKEN PORTD_CLKEN
#define GPIO_10_PORT PORTD
#define GPIO_10_DEV PTD
#define GPIO_10_PIN 6

// PU3
#define GPIO_11_EN 1
#define GPIO_11_IRQ PORTD_IRQn
#define GPIO_11_CLKEN PORTD_CLKEN
#define GPIO_11_PORT PORTD
#define GPIO_11_DEV PTD
#define GPIO_11_PIN 5

// PU4
#define GPIO_12_EN 1
#define GPIO_12_IRQ PORTD_IRQn
#define GPIO_12_CLKEN PORTD_CLKEN
#define GPIO_12_PORT PORTD
#define GPIO_12_DEV PTD
#define GPIO_12_PIN 4

// PU5
#define GPIO_13_EN 1
#define GPIO_13_IRQ PORTD_IRQn
#define GPIO_13_CLKEN PORTD_CLKEN
#define GPIO_13_PORT PORTD
#define GPIO_13_DEV PTD
#define GPIO_13_PIN 3

// PU6
#define GPIO_14_EN 1
#define GPIO_14_IRQ PORTD_IRQn
#define GPIO_14_CLKEN PORTD_CLKEN
#define GPIO_14_PORT PORTD
#define GPIO_14_DEV PTD
#define GPIO_14_PIN 2

// PU7
#define GPIO_15_EN 1
#define GPIO_15_IRQ PORTD_IRQn
#define GPIO_15_CLKEN PORTD_CLKEN
#define GPIO_15_PORT PORTD
#define GPIO_15_DEV PTD
#define GPIO_15_PIN 1

// PU8
#define GPIO_16_EN 1
#define GPIO_16_IRQ PORTD_IRQn
#define GPIO_16_CLKEN PORTD_CLKEN
#define GPIO_16_PORT PORTD
#define GPIO_16_DEV PTD
#define GPIO_16_PIN 0

// PU9
#define GPIO_17_EN 1
#define GPIO_17_IRQ PORTC_IRQn
#define GPIO_17_CLKEN PORTC_CLKEN
#define GPIO_17_PORT PORTC
#define GPIO_17_DEV PTC
#define GPIO_17_PIN 7

// PU10
#define GPIO_18_EN 1
#define GPIO_18_IRQ PORTC_IRQn
#define GPIO_18_CLKEN PORTC_CLKEN
#define GPIO_18_PORT PORTC
#define GPIO_18_DEV PTC
#define GPIO_18_PIN 5

// PU11
#if !UART_1_EN == 0
#define GPIO_19_EN 1
#define GPIO_19_IRQ PORTC_IRQn
#define GPIO_19_CLKEN PORTC_CLKEN
#define GPIO_19_PORT PORTC
#define GPIO_19_DEV PTC
#define GPIO_19_PIN 3
#endif

// PU12
// GND

// PU13
#define GPIO_20_EN 1
#define GPIO_20_IRQ PORTC_IRQn
#define GPIO_20_CLKEN PORTC_CLKEN
#define GPIO_20_PORT PORTC
#define GPIO_20_DEV PTC
#define GPIO_20_PIN 1

// PU14
#if K20_UART0_PORT != K20_UART0_PORT_B
#define GPIO_21_EN 1
#define GPIO_21_IRQ PORTB_IRQn
#define GPIO_21_CLKEN PORTB_CLKEN
#define GPIO_21_PORT PORTB
#define GPIO_21_DEV PTB
#define GPIO_21_PIN 17
#endif // UART0

/* Right (top) inner row */
// PR1
#if !UART_1_EN == 0
#define GPIO_22_EN 1
#define GPIO_22_IRQ PORTC_IRQn
#define GPIO_22_CLKEN PORTC_CLKEN
#define GPIO_22_PORT PORTC
#define GPIO_22_DEV PTC
#define GPIO_22_PIN 4
#endif

// PR2
#define GPIO_23_EN 1
#define GPIO_23_IRQ PORTC_IRQn
#define GPIO_23_CLKEN PORTC_CLKEN
#define GPIO_23_PORT PORTC
#define GPIO_23_DEV PTC
#define GPIO_23_PIN 6

// PR3
#define GPIO_24_EN 1
#define GPIO_24_IRQ PORTC_IRQn
#define GPIO_24_CLKEN PORTC_CLKEN
#define GPIO_24_PORT PORTC
#define GPIO_24_DEV PTC
#define GPIO_24_PIN 2

// PR4
// 3.3V

// PR5
#define GPIO_25_EN 1
#define GPIO_25_IRQ PORTC_IRQn
#define GPIO_25_CLKEN PORTC_CLKEN
#define GPIO_25_PORT PORTC
#define GPIO_25_DEV PTC
#define GPIO_25_PIN 0

// PR6
#define GPIO_26_EN 1
#define GPIO_26_IRQ PORTB_IRQn
#define GPIO_26_CLKEN PORTB_CLKEN
#define GPIO_26_PORT PORTB
#define GPIO_26_DEV PTB
#define GPIO_26_PIN 16

/* Debug (bottom inner) header */
/* Note: Using these disables SWD so handle with care. Just enable the flag in
 * the Makefile by adding it to CFLAGS */
#ifdef MCHCK_ENABLE_SWD_GPIO
// SWD_CLK
#define GPIO_27_EN 1
#define GPIO_27_IRQ PORTA_IRQn
#define GPIO_27_CLKEN PORTA_CLKEN
#define GPIO_27_PORT PORTA
#define GPIO_27_DEV PTA
#define GPIO_27_PIN 0

// SWD_DIO
#define GPIO_28_EN 1
#define GPIO_28_IRQ PORTA_IRQn
#define GPIO_28_CLKEN PORTA_CLKEN
#define GPIO_28_PORT PORTA
#define GPIO_28_DEV PTA
#define GPIO_28_PIN 3

#define GPIO_NUMOF 29
#else
#define GPIO_NUMOF 27
#endif


// VSS
// GND

// VRAW
// VEXT/5V

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __PERIPH_CONF_H */
/** @} */
