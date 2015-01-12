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
 * @brief       Implementation of the CPU initialization
 *
 * @author      Finn Wilke <finn.wilke@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */


#include <stdlib.h>

#include <periph/gpio.h>
#include <periph/uart.h>
#include "board.h"

static void leds_init(void);


/**
 * @brief   Initialize the board
 */
void board_init(void)
{
    /* initialize the boards LEDs, this is done first for debugging purposes */
    leds_init();

    /* initialize the CPU */
    cpu_init();
}

/**
 * @brief Initialize the boards on-board LED
 *
 * The LED initialization is hard-coded in this function. As the LED is soldered
 * onto the board it is fixed to their CPU pins.
 *
 * The LED is connected to PTB16
 */
static void leds_init(void)
{
    gpio_init_out(LED_GPIO, GPIO_NOPULL);
}
