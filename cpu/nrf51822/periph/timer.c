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
 * @file        timer.c
 * @brief       Low-level timer driver implementation
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include "cpu.h"
#include "board.h"
#include "sched.h"
#include "thread.h"
#include "periph_conf.h"
#include "periph/timer.h"

/* guard file in case no timer is defined */
#if TIMER_0_EN | TIMER_1_EN | TIMER_2_EN

typedef struct {
    timer_cb_t cb;
    void *arg;
} timer_conf_t;

/** timer state memory allocation */
#if TIMER_0_EN
static timer_conf_t t1_conf[TIMER_0_CHANNELS];
#endif
#if TIMER_1_EN
static timer_conf_t t2_conf[TIMER_1_CHANNELS];
#endif
#if TIMER_2_EN
static timer_conf_t t3_conf[TIMER_2_CHANNELS];
#endif

/**
 * @brief timer static configuration
 */
static NRF_TIMER_Type *const timer[] = {
#if TIMER_0_EN
    TIMER_0_DEV,
#endif
#if TIMER_1_EN
    TIMER_1_DEV,
#endif
#if TIMER_2_EN
    TIMER_2_DEV
#endif
};

static timer_conf_t const* config[] = {
#if TIMER_0_EN
    t1_conf,
#endif
#if TIMER_1_EN
    t2_conf,
#endif
#if TIMER_2_EN
    t3_conf
#endif
};

int timer_init(periph_timer_t dev)
{
    NRF_TIMER_Type *timer;

    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            timer = TIMER_0_DEV;
            timer->POWER = 1;
            timer->BITMODE = TIMER_0_BITMODE;
            timer->PRESCALER = TIMER_0_PRESCALER;
            NVIC_SetPriority(TIMER_0_IRQ, TIMER_IRQ_PRIO);
            NVIC_EnableIRQ(TIMER_0_IRQ);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            timer = TIMER_1_DEV;
            timer->POWER = 1;
            timer->BITMODE = TIEMR_1_BITMODE;
            timer->PRESCALER = TIMER_1_PRESCALER;
            NVIC_SetPriority(TIMER_1_IRQ, TIMER_IRQ_PRIO);
            NVIC_EnableIRQ(TIMER_1_IRQ);
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            timer = TIMER_2_DEV;
            timer->POWER = 1;
            timer->BITMODE = TIMER_2_BITMODE;
            timer->PRESCALER = TIMER_2_PRESCALER;
            NVIC_SetPriority(TIMER_2_IRQ, TIMER_IRQ_PRIO);
            NVIC_EnableIRQ(TIMER_2_IRQ);
            break;
#endif
        default:
            return -1;
    }

    timer->TASKS_STOP = 1;

    timer->MODE = TIMER_MODE_MODE_Timer;        /* set the timer in Timer Mode. */
    timer->TASKS_CLEAR    = 1;                  /* clear the task first to be usable for later. */

    /* clear all compare channels */
    timer->SHORTS = (TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos);
    timer->SHORTS = (TIMER_SHORTS_COMPARE1_CLEAR_Enabled << TIMER_SHORTS_COMPARE1_CLEAR_Pos);
    timer->SHORTS = (TIMER_SHORTS_COMPARE2_CLEAR_Enabled << TIMER_SHORTS_COMPARE2_CLEAR_Pos);
    timer->SHORTS = (TIMER_SHORTS_COMPARE3_CLEAR_Enabled << TIMER_SHORTS_COMPARE3_CLEAR_Pos);

    /* start the timer */
    timer->TASKS_START = 1;

    return 0;
}

int timer_set_rel(periph_timer_t dev, uint8_t chan, unsigned int timeout, timer_cb_t cb, void *arg)
{
    uint32_t now;
    timer_read(dev, &now);
    return timer_set_abs(dev, chan, (now + timeout - 1), cb, arg);
}

int timer_set_abs(periph_timer_t dev, uint8_t chan, unsigned int value, timer_cb_t cb, void *arg)
{
    volatile NRF_TIMER_Type * timer;

    /* get timer base register address */
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            timer = TIMER_0_DEV;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            timer = TIMER_1_DEV;
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            timer = TIMER_2_DEV;
            break;
#endif
        default:
            return -1;
    }

    switch (chan) {
        case 0:
            timer->CC[0] = value;
            timer->INTENSET |= TIMER_INTENSET_COMPARE0_Msk;
            break;
        case 1:
            timer->CC[1] = value;
            timer->INTENSET |= TIMER_INTENSET_COMPARE1_Msk;
            break;
        case 2:
            timer->CC[2] = value;
            timer->INTENSET |= TIMER_INTENSET_COMPARE2_Msk;
            break;
        default:
            return -2;
    }

    return 1;
}

int timer_clear(periph_timer_t dev, int channel)
{
    NRF_TIMER_Type *timer;

    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            timer = TIMER_0_DEV;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            timer = TIMER_1_DEV;
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            timer = TIMER_2_DEV;
            break;
#endif
        default:
            return -1;
    }

    /* set timeout value */
    switch (channel) {
        case 0:
            timer->INTENCLR = TIMER_INTENCLR_COMPARE0_Msk;
            break;
        case 1:
            timer->INTENCLR = TIMER_INTENCLR_COMPARE1_Msk;
            break;
        case 2:
            timer->INTENCLR = TIMER_INTENCLR_COMPARE2_Msk;
            break;
        default:
            return -2;
    }

    return 1;
}

int timer_read(periph_timer_t dev, uint32_t *value)
{
    if (dev >= TIMER_NUMOF) {
        return -1;
    }
    timer[dev]->TASKS_CAPTURE[3] = 1;
    *value = timer[dev]->CC[3];
    return 0;
}

void timer_start(periph_timer_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            TIMER_0_DEV->TASKS_START = 1;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->TASKS_START = 1;
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            TIMER_2_DEV->TASKS_START = 1;
            break;
#endif
    }
}

void timer_stop(periph_timer_t dev) {
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            TIMER_0_DEV->TASKS_STOP = 1;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->TASKS_STOP = 1;
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            TIMER_2_DEV->TASKS_STOP = 1;
            break;
#endif
    }
}

void timer_irq_enable(periph_timer_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            NVIC_EnableIRQ(TIMER_0_IRQ);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            NVIC_EnableIRQ(TIMER_1_IRQ);
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            NVIC_EnableIRQ(TIMER_2_IRQ);
            break;
#endif
    }
}

void timer_irq_disable(periph_timer_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            NVIC_DisableIRQ(TIMER_0_IRQ);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            NVIC_DisableIRQ(TIMER_1_IRQ);
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            NVIC_DisableIRQ(TIMER_2_IRQ);
            break;
#endif
    }
}

void timer_reset(periph_timer_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            TIMER_0_DEV->TASKS_CLEAR = 1;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->TASKS_CLEAR = 1;
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            TIMER_2_DEV->TASKS_CLEAR = 1;
            break;
#endif
    }
}

#if TIMER_0_EN
void TIMER_0_ISR(void)
{
    for(int i = 0; i < TIMER_0_CHANNELS; i++){
        if(TIMER_0_DEV->EVENTS_COMPARE[i] == 1){
            TIMER_0_DEV->EVENTS_COMPARE[i] = 0;
            TIMER_0_DEV->INTENCLR = (1 << (16 + i));
            config[TIMER_0][i].cb(config[TIMER_0][i].arg);
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif

#if TIMER_1_EN
void TIMER_1_ISR(void)
{
    for(int i = 0; i < TIMER_1_CHANNELS; i++){
        if(TIMER_1_DEV->EVENTS_COMPARE[i] == 1){
            TIMER_1_DEV->EVENTS_COMPARE[i] = 0;
            TIMER_1_DEV->INTENCLR = (1 << (16 + i));
            config[TIMER_1][i]->cb(config[TIMER_1][i].arg);
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif

#if TIMER_2_EN
void TIMER_2_ISR(void)
{
    for(int i = 0; i < TIMER_2_CHANNELS; i++){
        if(TIMER_2_DEV->EVENTS_COMPARE[i] == 1){
            TIMER_2_DEV->EVENTS_COMPARE[i] = 0;
            TIMER_2_DEV->INTENCLR = (1 << (16 + i));
            config[TIMER_2][i]->cb(config[TIMER_2][i].arg);
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif

#endif /* TIMER_0_EN | TIMER_1_EN | TIMER_2_EN */
