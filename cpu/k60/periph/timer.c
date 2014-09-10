/*
 * Copyright (C) 2014 Eistec AB
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
static inline void irq_handler(tim_t timer, PIT_Type* pit, int channel);

/** Type for timer state */
typedef struct {
    void (*cb)(int);
} timer_conf_t;

/** Timer state memory */
timer_conf_t config[TIMER_NUMOF];


int timer_init(tim_t dev, unsigned int ticks_per_us, void (*callback)(int))
{
    int channel = 0;
    /*
     * The RIOT API is modelled after the STM32 hardware timers. On the K60 PIT
     * module we have channels that can be started and stopped independently,
     * but they are always running at F_BUS frequency and they are always
     * counting down, so we have to do a few workarounds here.
     *
     * If we wish to use a different frequency we can arrange so that one PIT
     * channel acts as a prescaler for the next channel, but that means that we
     * lose half of the timers available in hardware. Since there are only 4 PIT
     * channels available I decided to only allow fixed frequency timers,
     * i.e. ticks_per_us = F_BUS / 1000000.
     */
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
            base_freq = TIMER_1_BASE_FREQ;
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            /* enable timer peripheral clock */
            TIMER_2_CLKEN();
            /* set timer's IRQ priority */
            NVIC_SetPriority(TIMER_2_IRQ_CHAN, TIMER_IRQ_PRIO);
            /* select timer */
            pit = TIMER_2_DEV;
            channel = TIMER_2_CHANNEL;
            base_freq = TIMER_2_BASE_FREQ;
            break;
#endif
#if TIMER_3_EN
        case TIMER_3:
            /* enable timer peripheral clock */
            TIMER_3_CLKEN();
            /* set timer's IRQ priority */
            NVIC_SetPriority(TIMER_3_IRQ_CHAN, TIMER_IRQ_PRIO);
            /* select timer */
            pit = TIMER_3_DEV;
            channel = TIMER_3_CHANNEL;
            base_freq = TIMER_3_BASE_FREQ;
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            return -1;
    }
    if (ticks_per_us != (base_freq / 1000000ul)) {
        return -1;
    }

    /* set callback function */
    config[dev].cb = callback;

    /* Disable interrupt */
    pit->CHANNEL[channel].TCTRL &= ~(PIT_TCTRL_TIE_MASK);

    /* Load some bs-value, just so that timer_read will be always incrementing even without using timer_set(). */
    pit->CHANNEL[channel].LDVAL = PIT_LDVAL_TSV(0xFFFFFFFF);

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
        DEBUGGER_BREAK(BREAK_INVALID_PARAM);
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
#if TIMER_2_EN
        case TIMER_2:
            pit = TIMER_2_DEV;
            real_channel = TIMER_2_CHANNEL;
            break;
#endif
#if TIMER_3_EN
        case TIMER_3:
            pit = TIMER_3_DEV;
            real_channel = TIMER_3_CHANNEL;
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    /* The actual hardware timer always counts down to 0 and we can not write
     * to the counter value, we need to reload the timer with the new value
     * by resetting it with a new LDVAL parameter. */
    pit->CHANNEL[real_channel].LDVAL = PIT_LDVAL_TSV(value); /* Load timer value */
    pit->CHANNEL[real_channel].TCTRL &= ~(PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK); /* Disable interrupt, disable timer */
    pit->CHANNEL[real_channel].TFLG |= PIT_TFLG_TIF_MASK; /* Clear interrupt flag */
    pit->CHANNEL[real_channel].TCTRL |= PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK; /* Enable interrupt, enable timer */

    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    if (channel != 0) {
        DEBUGGER_BREAK(BREAK_INVALID_PARAM);
        return -1;
    }

    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            TIMER_0_DEV->CHANNEL[TIMER_0_CHANNEL].TCTRL &= ~(PIT_TCTRL_TIE_MASK); /* Disable interrupt */;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->CHANNEL[TIMER_1_CHANNEL].TCTRL &= ~(PIT_TCTRL_TIE_MASK); /* Disable interrupt */;
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            TIMER_2_DEV->CHANNEL[TIMER_2_CHANNEL].TCTRL &= ~(PIT_TCTRL_TIE_MASK); /* Disable interrupt */;
            break;
#endif
#if TIMER_3_EN
        case TIMER_3:
            TIMER_3_DEV->CHANNEL[TIMER_3_CHANNEL].TCTRL &= ~(PIT_TCTRL_TIE_MASK); /* Disable interrupt */;
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    return 0;
}

unsigned int timer_read(tim_t dev)
{
    /* The system expects timers to be UP-counting. The K60 only provides DOWN-
     * counting timers. We convert to an incrementing timer value in this function. */
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            return TIMER_0_DEV->CHANNEL[TIMER_0_CHANNEL].LDVAL - TIMER_0_DEV->CHANNEL[TIMER_0_CHANNEL].CVAL;
#endif
#if TIMER_1_EN
        case TIMER_1:
            return TIMER_1_DEV->CHANNEL[TIMER_1_CHANNEL].LDVAL - TIMER_1_DEV->CHANNEL[TIMER_1_CHANNEL].CVAL;
#endif
#if TIMER_2_EN
        case TIMER_2:
            return TIMER_2_DEV->CHANNEL[TIMER_2_CHANNEL].LDVAL - TIMER_2_DEV->CHANNEL[TIMER_2_CHANNEL].CVAL;
#endif
#if TIMER_3_EN
        case TIMER_3:
            return TIMER_3_DEV->CHANNEL[TIMER_3_CHANNEL].LDVAL - TIMER_3_DEV->CHANNEL[TIMER_3_CHANNEL].CVAL;
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
#if TIMER_2_EN
        case TIMER_2:
            TIMER_2_DEV->CHANNEL[TIMER_2_CHANNEL].TCTRL |= PIT_TCTRL_TEN_MASK;
            break;
#endif
#if TIMER_3_EN
        case TIMER_3:
            TIMER_3_DEV->CHANNEL[TIMER_3_CHANNEL].TCTRL |= PIT_TCTRL_TEN_MASK;
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
            TIMER_0_DEV->CHANNEL[TIMER_0_CHANNEL].TCTRL &= ~(PIT_TCTRL_TEN_MASK);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->CHANNEL[TIMER_1_CHANNEL].TCTRL &= ~(PIT_TCTRL_TEN_MASK);
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            TIMER_2_DEV->CHANNEL[TIMER_2_CHANNEL].TCTRL &= ~(PIT_TCTRL_TEN_MASK);
            break;
#endif
#if TIMER_3_EN
        case TIMER_3:
            TIMER_3_DEV->CHANNEL[TIMER_3_CHANNEL].TCTRL &= ~(PIT_TCTRL_TEN_MASK);
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
#if TIMER_2_EN
        case TIMER_2:
            NVIC_EnableIRQ(TIMER_2_IRQ_CHAN);
            break;
#endif
#if TIMER_3_EN
        case TIMER_3:
            NVIC_EnableIRQ(TIMER_3_IRQ_CHAN);
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
#if TIMER_2_EN
        case TIMER_2:
            NVIC_DisableIRQ(TIMER_2_IRQ_CHAN);
            break;
#endif
#if TIMER_3_EN
        case TIMER_3:
            NVIC_DisableIRQ(TIMER_3_IRQ_CHAN);
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
            TIMER_0_DEV->CHANNEL[TIMER_0_CHANNEL].TCTRL &= ~(PIT_TCTRL_TEN_MASK);
            TIMER_0_DEV->CHANNEL[TIMER_0_CHANNEL].TCTRL |= PIT_TCTRL_TEN_MASK;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->CHANNEL[TIMER_1_CHANNEL].TCTRL &= ~(PIT_TCTRL_TEN_MASK);
            TIMER_1_DEV->CHANNEL[TIMER_1_CHANNEL].TCTRL |= PIT_TCTRL_TEN_MASK;
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            TIMER_2_DEV->CHANNEL[TIMER_2_CHANNEL].TCTRL &= ~(PIT_TCTRL_TEN_MASK);
            TIMER_2_DEV->CHANNEL[TIMER_2_CHANNEL].TCTRL |= PIT_TCTRL_TEN_MASK;
            break;
#endif
#if TIMER_3_EN
        case TIMER_3:
            TIMER_3_DEV->CHANNEL[TIMER_3_CHANNEL].TCTRL &= ~(PIT_TCTRL_TEN_MASK);
            TIMER_3_DEV->CHANNEL[TIMER_3_CHANNEL].TCTRL |= PIT_TCTRL_TEN_MASK;
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

#if TIMER_0_EN
__attribute__ ((naked)) void TIMER_0_ISR(void)
{
    ISR_ENTER();
    irq_handler(TIMER_0, TIMER_0_DEV, TIMER_0_CHANNEL);
    ISR_EXIT();
}
#endif /* TIMER_0_EN */

#if TIMER_1_EN
__attribute__ ((naked)) void TIMER_1_ISR(void)
{
    ISR_ENTER();
    irq_handler(TIMER_1, TIMER_1_DEV, TIMER_1_CHANNEL);
    ISR_EXIT();
}
#endif /* TIMER_1_EN */

#if TIMER_2_EN
__attribute__ ((naked)) void TIMER_2_ISR(void)
{
    ISR_ENTER();
    irq_handler(TIMER_2, TIMER_2_DEV, TIMER_2_CHANNEL);
    ISR_EXIT();
}
#endif /* TIMER_2_EN */

#if TIMER_3_EN
__attribute__ ((naked)) void TIMER_3_ISR(void)
{
    ISR_ENTER();
    irq_handler(TIMER_3, TIMER_3_DEV, TIMER_3_CHANNEL);
    ISR_EXIT();
}
#endif /* TIMER_3_EN */

static inline void irq_handler(tim_t timer, PIT_Type* pit, int channel)
{
    if (pit->CHANNEL[channel].TFLG & PIT_TFLG_TIF_MASK) {
        /* Disable interrupt */
        pit->CHANNEL[channel].TCTRL &= ~PIT_TCTRL_TIE_MASK;
        /* Clear interrupt flag by writing 1 to it */
        pit->CHANNEL[channel].TFLG |= PIT_TFLG_TIF_MASK;
        /* We are kind of abusing the API here, we want to have more than one HW
         * timer available for the kernel, but each PIT channel is completely
         * individual and does not work like the timers in the STM32Fxxx's with
         * multiple timers and multiple channels per timer. When we call the
         * callback we supply the TIMER ID as the CHANNEL parameter. The reason
         * for this is that it is necessary in order for the hardware timer
         * library to be able to distinguish which of its timeouts has occurred.
         */
        config[timer].cb(channel);
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
