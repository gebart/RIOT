/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <stdint.h>
#include "cpu.h"

/**
 * @ingroup     cpu_k60
 * @{
 *
 * @file
 * @brief       Implementation of K60 CPU initialization.
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 * @}
 */

extern void *_vector_rom[];

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* configure the vector table location to internal flash */
    SCB->VTOR = (uint32_t)_vector_rom;

    /* set pendSV interrupt to lowest possible priority */
    NVIC_SetPriority(PendSV_IRQn, 0xff);
}
/** @} */
