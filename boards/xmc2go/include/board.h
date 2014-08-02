/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    board_xmc2go XMC2Go
 * @ingroup     boards
 * @brief       Support for the XMC2Go board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the XMC2Go board.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __BOARD_H
#define __BOARD_H

#include "cpu.h"
#include "periph_conf.h"

/**
 * @name The nominal CPU core clock in this board
 */
#define F_CPU               CLOCK_CORE

/**
 * @name Assign the peripheral timer to be used as hardware timer
 */
#define HW_TIMER            TIMER_0

/**
 * @name Assign the UART interface to be used for stdio
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)
/** @} */

/**
 * @name LED pin definitions
 * @{
 */
#define LED_PORT            PORT1
#define LED1_PIN            (0)
#define LED1_PIN            (1)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED1_ON             (LED_PORT->OMR = (1 << LED1_PIN))
#define LED1_OFF            (LED_PORT->OMR = (1 << (LED1_PIN + 16)))
#define LED1_TOGGLE         (LED_PORT->OUT ^= (1 << LED1_PIN))
#define LED2_ON             (LED_PORT->OMR = (1 << (LED2_PIN + 16)))
#define LED2_OFF            (LED_PORT->OMR = (1 << LED2_PIN))
#define LED2_TOGGLE         (LED_PORT->OUT ^= (1 << LED2_PIN))

/* for compatibility to other boards */
#define LED_GREEN_ON        (LED1_ON)
#define LED_GREEN_OFF       (LED1_OFF)
#define LED_GREEN_TOGGLE    (LED1_TOGGLE)
#define LED_RED_ON          (LED2_ON)
#define LED_RED_OFF         (LED2_OFF)
#define LED_RED_TOGGLE      (LED2_TOGGLE)
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#endif /** __BOARD_H */
/** @} */
