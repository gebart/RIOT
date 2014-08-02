/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_xmc1000
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/gpio.h"
#include "periph_conf.h"

typedef struct {
    void (*cb)(void);
} gpio_state_t;

static gpio_state_t config[GPIO_NUMOF];

int gpio_init_out(gpio_t dev, gpio_pp_t pullup)
{
    return 0; /* all OK */
}

int gpio_init_in(gpio_t dev, gpio_pp_t pullup)
{
    return 0; /* everything alright here */
}

int gpio_init_int(gpio_t dev, gpio_pp_t pullup, gpio_flank_t flank, void (*cb)(void))
{
    return 0;
}

int gpio_read(gpio_t dev)
{
    return 0;
}

int gpio_set(gpio_t dev)
{
    return 0;
}

int gpio_clear(gpio_t dev)
{
    return 0;
}

int gpio_toggle(gpio_t dev)
{
    if (gpio_read(dev)) {
        return gpio_clear(dev);
    } else {
        return gpio_set(dev);
    }
}

int gpio_write(gpio_t dev, int value)
{
    if (value) {
        return gpio_set(dev);
    } else {
        return gpio_clear(dev);
    }
}
