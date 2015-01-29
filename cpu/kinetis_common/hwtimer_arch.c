/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_mkw2x
 * @{
 *
 * @file
 * @brief       Implementation of the kernels hwtimer interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "arch/hwtimer_arch.h"
#include "board.h"
#include "periph/timer.h"
#include "thread.h"

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
    timer_init(TIMER_0, 1, &irq_handler);
}

void hwtimer_arch_enable_interrupt(void)
{
    timer_irq_enable(TIMER_0);
}

void hwtimer_arch_disable_interrupt(void)
{
    timer_irq_disable(TIMER_0);
}

void hwtimer_arch_set(unsigned long offset, short timer)
{
    timer_set(TIMER_0, timer, offset);
}

void hwtimer_arch_set_absolute(unsigned long value, short timer)
{
    timer_set_absolute(TIMER_0, timer, value);
}

void hwtimer_arch_unset(short timer)
{
    timer_clear(TIMER_0, timer);
}

unsigned long hwtimer_arch_now(void)
{
    return timer_read(TIMER_0);
}

void irq_handler(int channel)
{
    timeout_handler((short)(channel));
}
