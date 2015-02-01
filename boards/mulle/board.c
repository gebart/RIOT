/*
 * Copyright (C) 2014 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_k60
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Mulle board
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */

#include <stddef.h> /* for NULL */
#include <stdio.h>
#include "board.h"
#include "cpu.h"
#include "periph/gpio.h"
#include "periph/uart.h"
#include "devicemap.h"

static void leds_init(void);
static void power_pins_init(void);

void board_init(void)
{
    /* initialize the boards LEDs, this is done first for debugging purposes */
    leds_init();

    LED_RED_ON;

    /* Set up clocks */
    SystemInit();

    /* Update SystemCoreClock global var */
    SystemCoreClockUpdate();

    /* initialize the CPU */
    cpu_init();

    LED_YELLOW_ON;

    devicemap_init();

    LED_GREEN_ON;

    /* Initialize power control pins */
    power_pins_init();

    /* Turn on Vperiph for peripherals */
    gpio_set(MULLE_POWER_VPERIPH);

    /* Turn on AVDD for reading voltages */
    gpio_set(MULLE_POWER_AVDD);
}

/**
 * @brief Initialize the boards on-board LEDs
 *
 * The LEDs are initialized here in order to be able to use them in the early
 * boot for diagnostics.
 *
 */
static void leds_init(void)
{
    /* The pin configuration can be found in board.h and periph_conf.h */
    gpio_init_out(LED_RED_GPIO, GPIO_NOPULL);
    gpio_init_out(LED_YELLOW_GPIO, GPIO_NOPULL);
    gpio_init_out(LED_GREEN_GPIO, GPIO_NOPULL);
}

static void power_pins_init(void)
{
    gpio_init_out(MULLE_POWER_AVDD, GPIO_NOPULL);
    gpio_init_out(MULLE_POWER_VPERIPH, GPIO_NOPULL);
    gpio_init_out(MULLE_POWER_VSEC, GPIO_NOPULL);
    gpio_clear(MULLE_POWER_AVDD);
    gpio_clear(MULLE_POWER_VPERIPH);
    gpio_clear(MULLE_POWER_VSEC);
}
