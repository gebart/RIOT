/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lpc2387
 * @{
 *
 * @file
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#define MODE_SYS        (0x1f)
#define MODE_UND        (0x1b)
#define MODE_SVC        (0x13)
#define MODE_ABT        (0x17)
#define MODE_IRQ        (0x12)
#define MODE_FIQ        (0x11)

#define DISABLE_IRQ     (0x80)
#define DISABLE_FIQ     (0x40)

/* text, data and bss locations */
extern uint32_t _etext;
extern uint32_t _data;
extern uint32_t _edata;
extern uint32_t __bss_start;
extern uint32_t __bss_end;

/* stacks */
extern uint32_t _stack_sys_start;
extern uint32_t _stack_und_start;
extern uint32_t _stack_svc_start;
extern uint32_t _stack_abt_start;
extern uint32_t _stack_irq_start;
extern uint32_t _stack_fiq_start;



void reset_handler(void)
{
    uint32_t *dst;
    uint32_t *src = &_etext;

    /* setup stacks */
    asm volatile("msr   CPSR, #MODE_SYS | DISABLE_FIQ | DISABLE_IRQ");
    asm volatile("ldr   sp, =_stack_sys_start");
    asm volatile("msr   CPSR, #MODE_UND | DISABLE_FIQ | DISABLE_IRQ");
    asm volatile("ldr   sp, =_stack_und_start");
    asm volatile("msr   CPSR, #MODE_SVC | DISABLE_FIQ | DISABLE_IRQ");
    asm volatile("ldr   sp, =_stack_svc_start");
    asm volatile("msr   CPSR, #MODE_ABT | DISABLE_FIQ | DISABLE_IRQ");
    asm volatile("ldr   sp, =_stack_abt_start");
    asm volatile("msr   CPSR, #MODE_IRQ | DISABLE_FIQ | DISABLE_IRQ");
    asm volatile("ldr   sp, =_stack_irq_start");
    asm volatile("msr   CPSR, #MODE_FIQ | DISABLE_FIQ | DISABLE_IRQ");
    asm volatile("ldr   sp, =_stack_fiq_start");

    /* load data section from flash to ram */
    for (dst = &_data; dst < &_edata; ) {
        *(dst++) = *(src++);
    }

    /* default bss section to zero */
    for (dst = &__bss_start; dst < &__bss_end; ) {
        *(dst++) = 0;
    }

    /* initialize the board and startup the kernel */
    board_init();
    /* initialize std-c library (this should be done after board_init) */
    __libc_init_array();
    /* startup the kernel */
    kernel_init();
}

void undef_handler(void)
{
    puts("##### UNDEF FAULT #####");
    while (1) {
        asm("nop");
    }
}

void prefetch_abort_handler(void)
{
    puts("##### PREFETCH ABORT #####");
    while (1) {
        asm("nop");
    }
}

void data_abort_handler(void)
{
    puts("##### DATA ABORT #####");
    while (1) {
        asm("nop");
    }
}

void default_handler(void)
{
    puts("##### DEFAULT HANDLER #####");
    while (1) {
        asm("nop");
    }
}

/* map interrupts to default handler in case they are not implemented */
void isr_swi(void)                 __attribute__ ((weak, alias("default_handler")));
void isr_irq(void)                 __attribute__ ((weak, alias("default_handler")));
void isr_fiq(void)                 __attribute__ ((weak, alias("default_handler")));

/* interrupt vector table */
__attribute__ ((section(".vectors")))
const void *interrupt_vector[] = {
    (void*) reset_handler,              /* entry point of the program */
    (void*) undef_handler,              /* error handler on undefined instruction called */
    (void*) isr_svc,                    /* software interrupt */
    (void*) prefetch_abort_handler,     /* prefetch abort handler */
    (void*) data_abort_handler,         /* data abort handler */
    (void*) (0UL),                      /* reserved */
    (void*) isr_irq,                    /* interrupt */
    (void*) isr_fiq,                    /* fast interrupt */
};
