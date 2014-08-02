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
 * @brief       Low-level timer driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdlib.h>

#include "cpu.h"
#include "board.h"
#include "periph_conf.h"
#include "periph/timer.h"


static inline void irq_handler(tim_t timer, TIM_TypeDef *dev);

typedef struct {
    void (*cb)(int);
} timer_conf_t;

/**
 * Timer state memory
 */
/* timer_conf_t config[TIMER_NUMOF]; */


int timer_init(tim_t dev, unsigned int ticks_per_us, void (*callback)(int))
{
    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    int now = timer_read(dev);
    return timer_set_absolute(dev, channel, now + timeout - 1);
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    return 0;
}

unsigned int timer_read(tim_t dev)
{
    return 0;
}

void timer_start(tim_t dev)
{
}

void timer_stop(tim_t dev)
{
}

void timer_irq_enable(tim_t dev)
{
}

void timer_irq_disable(tim_t dev)
{
}

void timer_reset(tim_t dev)
{
}
