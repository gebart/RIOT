/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_k20
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Finn Wilke <finn.wilke@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <stdint.h>
#include "cpu.h"
#include "periph_conf.h"
#include "kinetis_sim.h"

static void cpu_clock_init(void);

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize the clock system */
    cpu_clock_init();
}


/**
 * @brief Configure the controllers clock system
 *
 * @note This currently only supports a fixed 48 Mhz internal clock source.
 *       This should at least have the option to support an external
 *       reference clock. As one wasn't available on the testing hardware this
 *       isn't implemented yet.
 *
 * "In FBI and FEI modes, setting C4[DMX32] bit is not recommended. If the
 * internal reference is trimmed to a frequency above 32.768 kHz, the
 * greater FLL multiplication factor could potentially push the
 * microcontroller system clock out of specification and damage the part."
 *
 * You have the option to do this even though it is kind of unsafe. This
 * relies on the internal clock being somewhat accurately trimmed. It is
 * supposed to be factory-trimmed to 32.768 kHz but if you want to be extra
 * careful measure it with your hardware yourself and apply trimming values
 * as necessary.
 *
 * If trimmed incorrectly this MAY push the clock up to a value of 39.0625 kHz.
 * For the k20_50 that was used as testing device for this port this would mean
 * about 57.18 Mhz with the 48 Mhz setting in the most extreme case when the
 * internal clock is the fastest possible within the specification AND trimmed
 * completely incorrectly to be even faster.
 *
 * If you want to set the FLL factor to the potentially unsafe 1464, define
 * K20_USE_UNSAFE_FLL_FACTOR_ON_INTERNAL_OSCILLATOR. The nominal target
 * frequency is calculated as follows:
 * 32.768 kHz * 1464 = 47.972352 Mhz
 *
 * If you want to be on the safe side make sure this option is not defined to
 * have a safer factor of 1280 which would lead to a nominal target frequency
 * of:
 * 32.768 kHz * 1280 = 41.94304 Mhz
 *
 */
static void cpu_clock_init(void)
{
    /**
     * Set the FLL to ~42 or 48 Mhz as configured:
     */
#ifdef K20_USE_UNSAFE_FLL_FACTOR_ON_INTERNAL_OSCILLATOR
    MCG->C4 = (01 << MCG_C4_DRST_DRS_SHIFT) | (1 << MCG_C4_DMX32_SHIFT);
#else
    MCG->C4 = (01 << MCG_C4_DRST_DRS_SHIFT)
#endif

    SIM->SOPT2 &= ~(0 << SIM_SOPT2_PLLFLLSEL_SHIFT);

    /**
     * Enable the MCGIRCLK and select the fast 4Mhz internal reference clock.
     * The MCGIRCLK is mainly used by the LPTMR. Don't do this when the LPTMR
     * source is set to a different clock source.
     */
#if (LPTIMER_CLKSRC == LPTIMER_CLKSRC_MCGIRCLK)
    MCG->C2 |= MCG_C2_IRCS_MASK;
    MCG->C1 |= MCG_C1_IRCLKEN_MASK;

    while(!(MCG->S & MCG_S_IRCST_MASK));
#endif
}


#ifndef K20_SUPPRESS_UNSAFE_FLL_FACTOR_WARNING
#warning "This MCU uses an internal oscillator with a frequency very close to the max specified value. If the internal oscillator is not correctly calibrated this MIGHT damage your hardware. Check /cpu/k20/cpu.c:cpu_clock_init() for further details on this matter. Define K20_SUPPRESS_UNSAFE_FLL_FACTOR_WARNING to supress this warning. Undefine K20_USE_UNSAFE_FLL_FACTOR_ON_INTERNAL_OSCILLATOR to use safe defaults."
#endif
