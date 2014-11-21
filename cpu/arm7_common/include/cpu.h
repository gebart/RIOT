/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_arm7_common ARM7 Common Code
 * @ingroup     cpu
 * @brief       Common Code for all ARM7 implementations
 * @{
 *
 * @file
 * @brief       Common ARM7 cpu definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __CPU_H
#define __CPU_H

#include "cpu-conf.h"

/**
 * For downwards compatibility with old RIOT code.
 * TODO: remove once core was adjusted
 */
#include "irq.h"
#define eINT            enableIRQ
#define dINT            disableIRQ


/**
 * @brief Register a peripheral IRQ callback
 */
/******************************************************************************
** Function name:       install_irq
**
** Descriptions:        Install interrupt handler
** parameters:          Interrupt number, interrupt handler address,
**                      interrupt priority
** Returned value:      true or false, return false if IntNum is out of range
**
******************************************************************************/
int irq_install(int irq_number, void (*handler)(void), int prio);

/**
 * @brief Initialize the CPU, setup clock and interrupt vectors
 */
void cpu_init(void);

/** @} */
#endif /* __CPU_H */
