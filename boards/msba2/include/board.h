/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_msba2 MSB-A2
 * @ingroup     boards
 * @brief       Support for the ScatterWeb MSB-A2 board
 * @{
 *
 * @file
 * @brief       Basic definitions for the MSB-A2 board
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __BOARD_H
#define __BOARD_H

#include <stdint.h>

#include "cpu.h"
#include "bitarithm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Define the nominal CPU core clock in this board
 */
#define F_CPU               (72000000UL)

/**
 * @name Define the boards stdio
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * @name Assign the hardware timer
 */
#define HW_TIMER            TIMER_0

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_RED_PIN         (BIT25)
#define LED_GREEN_PIN       (BIT26)

#define LED_GREEN_OFF       (FIO3SET = LED_GREEN_PIN)
#define LED_GREEN_ON        (FIO3CLR = LED_GREEN_PIN)
#define LED_GREEN_TOGGLE    (FIO3PIN ^= LED_GREEN_PIN)
#define LED_RED_OFF         (FIO3SET = LED_RED_PIN)
#define LED_RED_ON          (FIO3CLR = LED_RED_PIN)
#define LED_RED_TOGGLE      (FIO3PIN ^= LED_RED_PIN)
/* @} */

/**
 * @brief Define the type for specifying a radio packet's length
 */
typedef uint8_t radio_packet_length_t;

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /** __BOARD_H */
/** @} */
