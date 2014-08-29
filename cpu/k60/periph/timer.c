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
 * @brief       Low-level timer driver implementation
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se
 *
 * @}
 */

#include <stdlib.h>

#include "cpu.h"
#include "board.h"
#include "sched.h"
#include "thread.h"
#include "periph_conf.h"
#include "periph/timer.h"

/** Unified IRQ handler for all timers */
static inline void irq_handler(tim_t timer, TIM_TypeDef *dev);

/** Type for timer state */
typedef struct {
    void (*cb)(int);
} timer_conf_t;

/** Timer state memory */
timer_conf_t config[TIMER_NUMOF];


int timer_init(tim_t dev, unsigned int ticks_per_us, void (*callback)(int))
{
    /*
     * We are not using timer channels of the RIOT API since we already need to
     * use channel chaining in order to reach the correct timer frequency on
     * the K60.
     *
     * All PIT timers run at F_SYS on the K60, this is why we need to use two
     * timers in order to set a custom frequency for the timer...
     */
    int channel = 0;
    int psc_channel = 0;
    unsigned int base_freq = 0;
    PIT_Type* pit;

    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            /* enable timer peripheral clock */
            TIMER_0_CLKEN();
            /* set timer's IRQ priority */
            NVIC_SetPriority(TIMER_0_IRQ_CHAN, TIMER_IRQ_PRIO);
            /* select timer */
            pit = TIMER_0_DEV;
            channel = TIMER_0_CHANNEL;
            psc_channel = TIMER_0_PSC_CHANNEL;
            base_freq = TIMER_0_BASE_FREQ;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            /* enable timer peripheral clock */
            TIMER_1_CLKEN();
            /* set timer's IRQ priority */
            NVIC_SetPriority(TIMER_1_IRQ_CHAN, TIMER_IRQ_PRIO);
            /* select timer */
            pit = TIMER_1_DEV;
            channel = TIMER_1_CHANNEL;
            psc_channel = TIMER_1_PSC_CHANNEL;
            base_freq = TIMER_1_BASE_FREQ;
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    /* set callback function */
    config[dev].cb = callback;

    /* Set up timer chaining */
    pit->CHANNEL[channel].TCTRL |= PIT_TCTRL_CHN_MASK;

    /* Disable interrupt on prescaler timer */
    pit->CHANNEL[psc_channel].TCTRL &= PIT_TCTRL_TIE_MASK;

    /* Set prescaler frequency */
    pit->CHANNEL[psc_channel].LDVAL = PIT_LDVAL_TSV(base_freq / (ticks_per_us * 1000000));

    /* enable the timer's interrupt */
    timer_irq_enable(dev);

    /* start the timer */
    timer_start(dev);

    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    return timer_set_absolute(dev, channel, timeout);
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    /* The K60 timer architecture is a bit different than the STM32 that this
     * API seem to be modelled around. */
    /* A side effect of this function is that the timer is always restarted
     * after setting a new value. */
    PIT_Type* pit;
    if (channel != 0) {
        return -1;
    }

    int real_channel;

    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            pit = TIMER_0_DEV;
            real_channel = TIMER_0_CHANNEL;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            pit = TIMER_1_DEV;
            real_channel = TIMER_1_CHANNEL;
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    /* The actual hardware timer always counts down to 0 and we can not write
     * to the counter value, we need to reload the timer with the new value
     * by resetting it with a new LDVAL parameter. */
    pit->CHANNEL[real_channel].LDVAL = PIT_LDVAL_TSV(timeout); /* Load timer value */
    pit->CHANNEL[real_channel].TCTRL &= PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK; /* Disable interrupt, disable timer */
    pit->CHANNEL[real_channel].TFLG |= PIT_TFLG_TIF_MASK; /* Clear interrupt flag */
    pit->CHANNEL[real_channel].TCTRL |= PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK; /* Enable interrupt, enable timer */

    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    PIT_Type* pit;

    if (channel != 0) {
        return -1;
    }

    int real_channel;

    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            pit = TIMER_0_DEV;
            real_channel = TIMER_0_CHANNEL;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            pit = TIMER_1_DEV;
            real_channel = TIMER_1_CHANNEL;
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    pit->CHANNEL[real_channel].TCTRL &= PIT_TCTRL_TIE_MASK; /* Disable interrupt */

    return 0;
}

unsigned int timer_read(tim_t dev)
{
    /* The system expects timers to be UP-counting. The K60 only provides DOWN-
     * counting timers. We convert to an incrementing timer value in this function. */
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            return TIMER_0_DEV[TIMER_0_CHANNEL].LDVAL - TIMER_0_DEV[TIMER_0_CHANNEL].CVAL;
#endif
#if TIMER_1_EN
        case TIMER_1:
            return TIMER_1_DEV[TIMER_1_CHANNEL].LDVAL - TIMER_1_DEV[TIMER_1_CHANNEL].CVAL;
#endif
        case TIMER_UNDEFINED:
        default:
            return -1;
    }
}

void timer_start(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            TIMER_0_DEV->CHANNEL[TIMER_0_CHANNEL].TCTRL |= PIT_TCTRL_TEN_MASK;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->CHANNEL[TIMER_1_CHANNEL].TCTRL |= PIT_TCTRL_TEN_MASK;
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

void timer_stop(tim_t dev)
{
    /* Note: Re-enabling the timer after stopping it has the side effect of
     * resetting the counter value as well. */
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            TIMER_0_DEV->CHANNEL[TIMER_0_CHANNEL].TCTRL &= ~PIT_TCTRL_TEN_MASK;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->CHANNEL[TIMER_1_CHANNEL].TCTRL &= ~PIT_TCTRL_TEN_MASK;
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

void timer_irq_enable(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            NVIC_EnableIRQ(TIMER_0_IRQ_CHAN);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            NVIC_EnableIRQ(TIMER_1_IRQ_CHAN);
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

void timer_irq_disable(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            NVIC_DisableIRQ(TIMER_0_IRQ_CHAN);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            NVIC_DisableIRQ(TIMER_1_IRQ_CHAN);
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

void timer_reset(tim_t dev)
{
    /* The counter register is not writable, we must reset by disabling and then
     * enable the timer again. */
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            TIMER_0_DEV->CHANNEL[TIMER_0_CHANNEL].TCTRL &= ~PIT_TCTRL_TEN_MASK;
            TIMER_0_DEV->CHANNEL[TIMER_0_CHANNEL].TCTRL |= PIT_TCTRL_TEN_MASK;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->CHANNEL[TIMER_1_CHANNEL].TCTRL &= ~PIT_TCTRL_TEN_MASK;
            TIMER_1_DEV->CHANNEL[TIMER_1_CHANNEL].TCTRL |= PIT_TCTRL_TEN_MASK;
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

__attribute__ ((naked)) void TIMER_0_ISR(void)
{
    ISR_ENTER();
    irq_handler(TIMER_0, TIMER_0_DEV, TIMER_0_CHANNEL);
    ISR_EXIT();
}

__attribute__ ((naked)) void TIMER_1_ISR(void)
{
    ISR_ENTER();
    irq_handler(TIMER_1, TIMER_1_DEV, TIMER_1_CHANNEL);
    ISR_EXIT();
}

static inline void irq_handler(tim_t timer, PIT_Type* pit, int channel)
{
    if (pit->CHANNEL[channel].TFLG & PIT_TFLG_TIF_MASK) {
        /* Disable interrupt */
        pit->CHANNEL[channel].TCTRL &= ~PIT_TCTRL_TIE_MASK;
        /* Clear interrupt flag by writing 1 to it */
        pit->CHANNEL[channel].TFLG |= PIT_TFLG_TIF_MASK;
        config[timer].cb(0);
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
