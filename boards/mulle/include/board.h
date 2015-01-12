/*
 * Copyright (C) 2014 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    board_mulle Eistec Mulle
 * @ingroup     boards
 * @brief       Board specific files for Eistec Mulle IoT boards
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Eistec Mulle IoT board
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "cpu.h"
#include "periph_conf.h"

/* Use the on board RTC 32kHz clock for LPTMR clocking. */
#undef LPTIMER_CLKSRC
#define LPTIMER_CLKSRC LPTIMER_CLKSRC_ERCLK32K

/** Disable hardware watchdog, for debugging purposes, don't use this on production builds. */
#define DISABLE_WDOG    1

/**
 * @name Assign the first hardware timer.
 * This timer will be used to implement an absolute reference for hwtimer_now() et al.
 */
#define HW_TIMER            TIMER_0

/**
 * @name Number of subsequent channels of the PIT to assign to the RIOT hardware
 * timer library, starting after the HW_TIMER above.
 */
#define HW_TIMERS_COUNT      1

/**
 * @name Define UART device and baudrate for stdio
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * @name LEDs configuration
 * @{
 */

#define LED_RED_GPIO        GPIO_0
#define LED_RED_PORT        GPIO_0_DEV
#define LED_RED_PIN         GPIO_0_PIN
#define LED_YELLOW_GPIO     GPIO_1
#define LED_YELLOW_PORT     GPIO_1_DEV
#define LED_YELLOW_PIN      GPIO_1_PIN
#define LED_GREEN_GPIO      GPIO_2
#define LED_GREEN_PORT      GPIO_2_DEV
#define LED_GREEN_PIN       GPIO_2_PIN

/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_RED_ON          (BITBAND_REG(LED_RED_PORT->PSOR, LED_RED_PIN) = 1)
#define LED_RED_OFF         (BITBAND_REG(LED_RED_PORT->PCOR, LED_RED_PIN) = 1)
#define LED_RED_TOGGLE      (BITBAND_REG(LED_RED_PORT->PTOR, LED_RED_PIN) = 1)
#define LED_YELLOW_ON       (BITBAND_REG(LED_YELLOW_PORT->PSOR, LED_YELLOW_PIN) = 1)
#define LED_YELLOW_OFF      (BITBAND_REG(LED_YELLOW_PORT->PCOR, LED_YELLOW_PIN) = 1)
#define LED_YELLOW_TOGGLE   (BITBAND_REG(LED_YELLOW_PORT->PTOR, LED_YELLOW_PIN) = 1)
#define LED_GREEN_ON        (BITBAND_REG(LED_GREEN_PORT->PSOR, LED_GREEN_PIN) = 1)
#define LED_GREEN_OFF       (BITBAND_REG(LED_GREEN_PORT->PCOR, LED_GREEN_PIN) = 1)
#define LED_GREEN_TOGGLE    (BITBAND_REG(LED_GREEN_PORT->PTOR, LED_GREEN_PIN) = 1)
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

/**
 * RTC crystal load capacitance configuration bits.
 */
/* enable 12pF load capacitance, might need adjusting.. */
#define BOARD_RTC_LOAD_CAP_BITS (RTC_CR_SC8P_MASK | RTC_CR_SC4P_MASK)


#endif /* BOARD_H_ */
/** @} */
