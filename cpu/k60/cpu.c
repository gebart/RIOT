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
 * @brief       Implementation of the CPU initialization
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <stdint.h>
#include "cpu.h"
#include "periph_conf.h"


static void cpu_clock_init(void);

extern void *_vector_rom[];

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* configure the vector table location to internal flash */
    SCB->VTOR = (uint32_t)_vector_rom;

    /* initialize the clock system */
    cpu_clock_init();

    /* set pendSV interrupt to lowest possible priority */
    NVIC_SetPriority(PendSV_IRQn, 0xff);
}

void core_clocks_init_early(void);
void core_clocks_init(void);

/**
 * @brief Configure the controllers clock system
 *
 * The clock initialization make the following assumptions:
 * - the external HSE clock from an external oscillator is used as base clock
 * - the internal PLL circuit is used for clock refinement
 *
 * Use the following formulas to calculate the needed values:
 *
 * SYSCLK = ((HSE_VALUE / CLOCK_PLL_M) * CLOCK_PLL_N) / CLOCK_PLL_P
 * USB, SDIO and RNG Clock =  ((HSE_VALUE / CLOCK_PLL_M) * CLOCK_PLL_N) / CLOCK_PLL_Q
 *
 * The actual used values are specified in the board's `periph_conf.h` file.
 *
 * NOTE: currently there is not timeout for initialization of PLL and other locks
 *       -> when wrong values are chosen, the initialization could stall
 */
static void cpu_clock_init(void)
{
    core_clocks_init_early();
    core_clocks_init();
}
