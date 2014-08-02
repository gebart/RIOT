/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_xmc2go
 * @{
 *
 * @file
 * @brief       Board specific implementations for the xmc2go board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "board.h"
#include "periph/uart.h"


static void leds_init(void);

void board_init(void)
{
    /* initialize the boards LEDs */
    leds_init();

    /* initialize the CPU */
    cpu_init();
}

/**
 * @brief Initialize the boards on-board LEDs (LED1 and LED2)
 *
 * The LED initialization is hard-coded in this function. As the LEDs are soldered
 * onto the board they are fixed to their CPU pins.
 *
 * The LEDs are connected to the following pins:
 * - LED1: P1.1
 * - LED2: P1.0
 */
static void leds_init(void)
{
    /* set pin function to push-pull general purpose output */
    LED_PORT->IOCR0 &= ~(PORT1_IOCR0_PC0_Msk | PORT1_IOCR0_PC1_Msk);
    LED_PORT->IOCR0 |= (0x10 << PORT1_IOCR0_PC0_Pos) | (0x10 << PORT1_IOCR0_PC1_Pos);

    /* turn LEDs off (high-active) */
    LED_PORT->OMR = (1 << LED1_PIN) | (1 << LED2_PIN);
}
