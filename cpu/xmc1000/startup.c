/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_xmc1100
 * @{
 *
 * @file
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>


/**
 * memory markers as defined in the linker script
 */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;

/**
 * @brief functions for initializing the board, std-lib and kernel
 */
extern void board_init(void);
extern void kernel_init(void);
extern void __libc_init_array(void);

/**
 * @brief This function is the entry point after a system reset
 *
 * After a system reset, the following steps are necessary and carried out:
 * 1. load data section from flash to ram
 * 2. overwrite uninitialized data section (BSS) with zeros
 * 3. initialize the newlib
 * 4. initialize the board (sync clock, setup std-IO)
 * 5. initialize and start RIOTs kernel
 */
void reset_handler(void)
{
    uint32_t *dst;
    uint32_t *src = &_etext;

    /* load data section from flash to ram */
    for (dst = &_srelocate; dst < &_erelocate; ) {
        *(dst++) = *(src++);
    }

    /* default bss section to zero */
    for (dst = &_szero; dst < &_ezero; ) {
        *(dst++) = 0;
    }

    /* initialize the board and startup the kernel */
    board_init();
    /* initialize std-c library (this should be done after board_init) */
    __libc_init_array();
    /* startup the kernel */
    kernel_init();
}

/**
 * @brief Default handler is called in case no interrupt handler was defined
 */
void dummy_handler(void)
{
    while (1) {asm ("nop");}
}

void isr_nmi(void)
{
    while (1) {asm ("nop");}
}

void isr_mem_manage(void)
{
    while (1) {asm ("nop");}
}

void isr_debug_mon(void)
{
    while (1) {asm ("nop");}
}

void isr_hard_fault(void)
{
    while (1) {asm ("nop");}
}

void isr_bus_fault(void)
{
    while (1) {asm ("nop");}
}

void isr_usage_fault(void)
{
    while (1) {asm ("nop");}
}

/* Cortex-M specific interrupt vectors */
void isr_svc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_pendsv(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_systick(void)              __attribute__ ((weak, alias("dummy_handler")));

/* XMC1100 specific interrupt vector */
void isr_scu_0(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_scu_1(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_scu_2(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_eru0_0(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_eru0_1(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_eru0_2(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_eru0_3(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_usic0_0(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_usic0_1(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_usic0_2(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_usic0_3(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_usic0_4(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_usic0_5(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_vadc0_c0_0(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_vadc0_c0_1(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_ccu40_0(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_ccu40_1(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_ccu40_2(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_ccu40_3(void)              __attribute__ ((weak, alias("dummy_handler")));

/* interrupt vector table */
__attribute__ ((section(".vectors")))
const void *interrupt_vector[] = {
    /* Stack pointer */
    (void*) (&_estack),             /* pointer to the top of the empty stack */
    /* Cortex-M handlers */
    (void*) reset_handler,          /* entry point of the program */
    (void*) isr_nmi,                /* non maskable interrupt handler */
    (void*) isr_hard_fault,         /* if you end up here its not good */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) isr_svc,                /* system call interrupt */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) isr_pendsv,             /* pendSV interrupt, used for task switching in RIOT */
    (void*) isr_systick,            /* SysTick interrupt, not used in RIOT */
    /* XMC1100 specific peripheral handlers */
    (void*) isr_scu_0               /* Handler name for SR SCU_0 */
    (void*) isr_scu_1               /* Handler name for SR SCU_1 */
    (void*) isr_scu_2               /* Handler name for SR SCU_2 */
    (void*) isr_eru0_0              /* Handler name for SR ERU0_0 */
    (void*) isr_eru0_1              /* Handler name for SR ERU0_1 */
    (void*) isr_eru0_2              /* Handler name for SR ERU0_2 */
    (void*) isr_eru0_3              /* Handler name for SR ERU0_3 */
    (void*) (0UL)                   /* Not Available */
    (void*) (0UL)                   /* Not Available */
    (void*) isr_usic0_0             /* Handler name for SR USIC0_0 */
    (void*) isr_usic0_1             /* Handler name for SR USIC0_1 */
    (void*) isr_usic0_2             /* Handler name for SR USIC0_2 */
    (void*) isr_usic0_3             /* Handler name for SR USIC0_3 */
    (void*) isr_usic0_4             /* Handler name for SR USIC0_4 */
    (void*) isr_usic0_5             /* Handler name for SR USIC0_5 */
    (void*) isr_vadc0_c0_0          /* Handler name for SR VADC0_C0_0 */
    (void*) isr_vadc0_c0_1          /* Handler name for SR VADC0_C0_1 */
    (void*) (0UL)                   /* Not Available */
    (void*) (0UL)                   /* Not Available */
    (void*) (0UL)                   /* Not Available */
    (void*) (0UL)                   /* Not Available */
    (void*) isr_ccu40_0             /* Handler name for SR CCU40_0 */
    (void*) isr_ccu40_1             /* Handler name for SR CCU40_1 */
    (void*) isr_ccu40_2             /* Handler name for SR CCU40_2 */
    (void*) isr_ccu40_3             /* Handler name for SR CCU40_3 */
};
