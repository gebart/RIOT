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

#if HW_TIMERS_COUNT > HWTIMER_MAXTIMERS
#error HW_TIMERS_COUNT (board.h) > HWTIMER_MAXTIMERS (hwtimer_cpu.h)
#endif

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
    for(int i = 0; i < HW_TIMERS_COUNT; ++i)
    {
        timer_init(HW_TIMER + i, HWTIMER_SPEED/1000000, &irq_handler);
    }
}

void hwtimer_arch_enable_interrupt(void)
{
    /* Turn on interrupts on all channels, this is probably not desired... */
    for(int i = 0; i < HW_TIMERS_COUNT; ++i)
    {
        timer_irq_enable(HW_TIMER + 1 + i);
    }
}

void hwtimer_arch_disable_interrupt(void)
{
    /* Turn off interrupts on all channels, this is probably not desired... */
    for(int i = 0; i < HW_TIMERS_COUNT; ++i)
    {
        timer_irq_disable(HW_TIMER + 1 + i);
    }
}

void hwtimer_arch_set(unsigned long offset, short timer)
{
    timer_set(HW_TIMER + 1 + timer, 0, offset);
}

void hwtimer_arch_set_absolute(unsigned long value, short timer)
{
    /* We have to count backwards from the absolute time reference in order to
     * know what delay to set. */
    timer_set(HW_TIMER + 1 + timer, 0, value - hwtimer_arch_now());
}

void hwtimer_arch_unset(short timer)
{
    timer_clear(HW_TIMER + 1 + timer, 0);
}

unsigned long hwtimer_arch_now(void)
{
    /* Read the absolute reference timer */
    return timer_read(HW_TIMER);
}

void irq_handler(int channel)
{
    /* Translate back to hwtimer library timer numbers from physical hardware channel index */
    timeout_handler((short)(channel - HW_TIMER));
}
