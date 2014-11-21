/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_arm7_common
 * @{
 *
 * @file
 * @brief       Implementation of ARM7 atomic functions
 *
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "arch/irq_arch.h"

#define I_Bit           0x80
#define F_Bit           0x40

#define SYS32Mode       0x1F
#define IRQ32Mode       0x12
#define FIQ32Mode       0x11

#define IRQ_MASK        0x00000080
#define INT_MODE         (FIQ32Mode | IRQ32Mode)

#define VIC_BASE_ADDR   0xFFFFF000
#define VIC_SIZE        (32)

static inline unsigned __get_cpsr(void)
{
    unsigned long retval;
    asm volatile(" mrs  %0, cpsr" : "=r"(retval) : /* no inputs */);
    return retval;
}

static inline void __set_cpsr(unsigned val)
{
    asm volatile(" msr  cpsr, %0" : /* no outputs */ : "r"(val));
}

unsigned int irq_arch_enable(void)
{
    unsigned _cpsr;

    _cpsr = __get_cpsr();
    __set_cpsr(_cpsr & ~IRQ_MASK);
    return _cpsr;
}

unsigned int irq_arch_disable(void)
{
    unsigned _cpsr;

    _cpsr = __get_cpsr();
    __set_cpsr(_cpsr | IRQ_MASK);
    return _cpsr;
}

void irq_arch_restore(unsigned int state)
{
    unsigned _cpsr;

    _cpsr = __get_cpsr();
    __set_cpsr((_cpsr & ~IRQ_MASK) | (state & IRQ_MASK));
}


int irq_arch_in(void)
{
    int retval;
    asm volatile("mrs  %0, cpsr" : "=r"(retval) : /* no inputs */);
    return (retval & INT_MODE) == 18;
}

int irq_install(int irq_number, void (*handler)(void), int prio)
{
    VICIntEnClr = (1 << irq_number);   /* disable Interrupt */

    if (irq_number >= VIC_SIZE) {
        return -1;
    }
    else {
        /* find first un-assigned VIC address for the handler */
        uint32_t *vect_addr = (uint32_t *)(VIC_BASE_ADDR + VICVectAddr0 + (irq_number * 4));
        uint32_t *vect_cntl = (uint32_t *)(VIC_BASE_ADDR + VICVectCntl0 + (irq_number * 4));
        *vect_addr = (uint32_t)handler;  /* set interrupt vector */
        *vect_cntl = prio;
        VICIntEnable = 1 << irq_number;  /* Enable Interrupt */
        return 0;
    }
}
