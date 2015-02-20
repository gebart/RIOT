/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_lpc1768
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
#include "sched.h"
#include "thread.h"
#include "periph/gpio.h"
#include "periph_conf.h"

/* guard file in case no GPIO devices are defined */
#if GPIO_NUMOF

int gpio_init_out(gpio_t dev, gpio_pp_t pullup)
{
    return -1;
}

int gpio_init_in(gpio_t dev, gpio_pp_t pullup)
{
    return -1;
}

int gpio_init_int(gpio_t dev, gpio_pp_t pullup, gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    return -1;
}

void gpio_irq_enable(gpio_t dev)
{

}

void gpio_irq_disable(gpio_t dev)
{

}

int gpio_read(gpio_t dev)
{
    return -1;
}

void gpio_set(gpio_t dev)
{

}

void gpio_clear(gpio_t dev)
{

}

void gpio_toggle(gpio_t dev)
{

}

void gpio_write(gpio_t dev, int value)
{

}

#endif /* GPIO_NUMOF */
