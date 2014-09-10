/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_k60
 * @{
 *
 * @file
 * @brief       Implementation of the kernels hwtimer interface
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */

#include "arch/hwtimer_arch.h"
#include "board.h"
#include "periph/timer.h"
#include "thread.h"
#include "hwtimer_cpu.h"

/**
 * @brief Callback function that is given to the low-level timer
 *
 * @param[in] channel   the channel of the low-level timer that was triggered
 */
void irq_handler(int channel);

/**
 * @brief Hold a reference to the hwtimer callback
 */
void (*timeout_handler)(int);

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu)
{
    timeout_handler = handler;
    /* Initialize all channels that have been designated for HW timer library use. */
    for(int i = 0; i < HW_TIMER_COUNT; ++i)
    {
        timer_init(i, HWTIMER_SPEED/1000000, &irq_handler);
    }
}

void hwtimer_arch_enable_interrupt(void)
{
    DEBUG("hwtimer_arch_enable_interrupt(): not implemented\n");
    /* timer_irq_enable(HW_TIMER); */
}

void hwtimer_arch_disable_interrupt(void)
{
    DEBUG("hwtimer_arch_disable_interrupt(): not implemented\n");
    /* timer_irq_disable(HW_TIMER); */
}

void hwtimer_arch_set(unsigned long offset, short timer)
{
    timer_set(timer, 0, offset);
}

void hwtimer_arch_set_absolute(unsigned long value, short timer)
{
    timer_set_absolute(timer, 0, value);
}

void hwtimer_arch_unset(short timer)
{
    timer_clear(timer, 0);
}

unsigned long hwtimer_arch_now(void)
{
    return timer_read(HW_TIMER);
}

void irq_handler(int channel)
{
    timeout_handler((short)(channel));
}
