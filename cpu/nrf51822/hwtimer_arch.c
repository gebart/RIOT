/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_nrf51822
 * @{
 *
 * @file        hwtimer_arch.c
 * @brief       Implementation of the kernels hwtimer interface
 *
 * The hardware timer implementation uses a direct mapping to the low-level UART driver.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>

#include "arch/hwtimer_arch.h"
#include "board.h"
#include "periph/timer.h"
#include "thread.h"


static void irq_handler(void *arg);
static void(*timeout_handler)(int channel);


void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu)
{
    timeout_handler = handler;
    timer_init(HW_TIMER);
}

void hwtimer_arch_enable_interrupt(void)
{
    timer_irq_enable(HW_TIMER);
}

void hwtimer_arch_disable_interrupt(void)
{
    timer_irq_disable(HW_TIMER);
}

void hwtimer_arch_set(unsigned long offset, short timer)
{
    int chan = (int)timer;
    timer_set_rel(HW_TIMER, timer, offset, &irq_handler, (void*)chan);
}

void hwtimer_arch_set_absolute(unsigned long value, short timer)
{
    int chan = (int)timer;
    timer_set_abs(HW_TIMER, timer, value, &irq_handler, (void*)chan);
}

void hwtimer_arch_unset(short timer)
{
    timer_clear(HW_TIMER, timer);
}

unsigned long hwtimer_arch_now(void)
{
    uint32_t now;
    timer_read(HW_TIMER, &now);
    return (unsigned int)now;
}

void irq_handler(void *arg)
{
    int res = (int)arg;
    timeout_handler((short)res);
}
