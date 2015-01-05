/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernels hwtimer interface
 *              hwtimer uses Freescale Low Power Timer lptmr0.
 *              There are two clock sources supported and tested:
 *              LPO - 1kHz
 *              RTC - 32768kHz
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include "arch/hwtimer_arch.h"
#include "hwtimer_cpu.h"
#include "cpu-conf.h"
#include "thread.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define LPTIMER_IRQ_PRIO          1
#define LPTMR_MAXTICKS            (0x0000FFFF)

typedef struct {
    uint32_t counter32b;
    uint32_t cmr32b;
    uint32_t diff;
} hwtimer_stimer32b_t;

static hwtimer_stimer32b_t stimer;

/**
 * @brief Reference to the hwtimer callback
 */
void (*timeout_handler)(int);

inline static void hwtimer_start(void)
{
    LPTMR0->CSR |= LPTMR_CSR_TEN_MASK;
}

inline static void hwtimer_stop(void)
{
    LPTMR0->CSR &= ~LPTMR_CSR_TEN_MASK;
}

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu)
{
    timeout_handler = handler;
    LPTMR_Type *timer = LPTMR0;

    /* unlock lptrm0 */
    SIM->SCGC5 |= (SIM_SCGC5_LPTMR_MASK);
    /* set lptmr's IRQ priority */
    NVIC_SetPriority(LPTimer_IRQn, LPTIMER_IRQ_PRIO);
    /* reset lptmr */
    timer->CSR = 0;

    switch (LPTIMER_CLKSRC) {
        case LPTIMER_CLKSRC_MCGIRCLK:
            /* Select MCGIRCLK as clock source */
            timer->PSR = LPTMR_PSR_PRESCALE(LPTIMER_CLK_PRESCALE) | LPTMR_PSR_PCS(0);
            break;

        case LPTIMER_CLKSRC_OSCERCLK:
            /* Select OSCERCLK(4 MHz) as clock source */
            timer->PSR = LPTMR_PSR_PRESCALE(LPTIMER_CLK_PRESCALE) | LPTMR_PSR_PCS(3);
            break;

        case LPTIMER_CLKSRC_ERCLK32K:
            /* Select rtc oscillator output as clock source for ERCLK32K, */
            /* it needs functioning RTC module and driver. */
            SIM->SOPT1 &= ~(SIM_SOPT1_OSC32KSEL_MASK);
            SIM->SOPT1 |= SIM_SOPT1_OSC32KSEL(2);
            /* select ERCLK32K as clock source for lptmr0 */
            timer->PSR = LPTMR_PSR_PBYP_MASK | LPTMR_PSR_PCS(2);
            break;

        case LPTIMER_CLKSRC_LPO:
        default:
            /* select LPO as clock source (1 kHz)*/
            timer->PSR = LPTMR_PSR_PBYP_MASK | LPTMR_PSR_PCS(1);
    }

    LPTMR0->CMR = (uint16_t)(LPTMR_MAXTICKS);
    /* enable lptrm interrupt */
    timer->CSR = LPTMR_CSR_TIE_MASK;

    stimer.counter32b = 0;
    stimer.cmr32b = 0;
    stimer.diff = 0;

    hwtimer_arch_enable_interrupt();
    hwtimer_start();
}

void hwtimer_arch_enable_interrupt(void)
{
    NVIC_EnableIRQ(LPTimer_IRQn);
}

void hwtimer_arch_disable_interrupt(void)
{
    NVIC_DisableIRQ(LPTimer_IRQn);
}

void hwtimer_arch_set(unsigned long offset, short timer)
{
    (void)timer;
    LPTMR0->CNR = 42;
    stimer.counter32b += (uint32_t)LPTMR0->CNR;
    hwtimer_stop();

    stimer.cmr32b = stimer.counter32b + offset;
    stimer.diff = offset;

    if (stimer.diff > LPTMR_MAXTICKS) {
        stimer.diff = LPTMR_MAXTICKS;
    }

    DEBUG("cntr: %lu, cmr: %lu, diff: %lu\n", stimer.counter32b, stimer.cmr32b, stimer.diff);

    LPTMR0->CMR = (uint16_t)(stimer.diff);
    hwtimer_start();
}

void hwtimer_arch_set_absolute(unsigned long value, short timer)
{
    (void)timer;
    LPTMR0->CNR = 42;
    stimer.counter32b += (uint32_t)LPTMR0->CNR;
    hwtimer_stop();

    stimer.cmr32b = value;
    stimer.diff = stimer.cmr32b - stimer.counter32b;

    if (stimer.diff > LPTMR_MAXTICKS) {
        stimer.diff = LPTMR_MAXTICKS;
    }

    DEBUG("cntr: %lu, cmr: %lu, diff: %lu\n", stimer.counter32b, stimer.cmr32b, stimer.diff);

    LPTMR0->CMR = (uint16_t)(stimer.diff);
    hwtimer_start();
}

void hwtimer_arch_unset(short timer)
{
    LPTMR0->CNR = 42;
    stimer.counter32b += (uint32_t)LPTMR0->CNR;
    hwtimer_stop();
    stimer.diff = 0;
    stimer.cmr32b = 0;
    LPTMR0->CMR = (uint16_t)(LPTMR_MAXTICKS);
    hwtimer_start();

}

unsigned long hwtimer_arch_now(void)
{
    LPTMR0->CNR = 42;
    return (unsigned int)(((uint32_t)LPTMR0->CNR + stimer.counter32b));
}

void isr_lptmr0(void)
{
    stimer.counter32b += (uint32_t)LPTMR0->CMR;
    /* clear compare flag (w1c bit) */
    LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;

    if (stimer.diff) {
        if (stimer.cmr32b > stimer.counter32b) {
            hwtimer_arch_set_absolute(stimer.cmr32b, 0);
        }
        else {
            stimer.diff = 0;
            timeout_handler((short)0);
        }
    }
    else {
        hwtimer_arch_unset(0);
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
