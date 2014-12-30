/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kw0x
 * @{
 *
 * @file
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include <stdint.h>
#include "cpu-conf.h"

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
 * 0. disable the Watchdog Timer
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

    /* unlock and disable the COP WDOG */
    SIM->COPC = (uint32_t)0x00u;

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

void isr_hard_fault(void)
{
    while (1) {asm ("nop");}
}

/* Cortex-M specific interrupt vectors */
void isr_svc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_pendsv(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_systick(void)              __attribute__ ((weak, alias("dummy_handler")));

/* MKW01Z128 specific interrupt vector */
void isr_dma0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_dma2(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_dma3(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_ftfa(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_lvd_lvw(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_llwu(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c1(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_spi0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_spi1(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_uart0(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_uart1(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_uart2(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_adc0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_cmp0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_tpm0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_tpm1(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_tpm2(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_rtc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_rtc_seconds(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_pit(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_i2s0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_dac0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_tsi0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_mcg(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_lptmr0(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_porta(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_portc_portd(void)          __attribute__ ((weak, alias("dummy_handler")));


/* interrupt vector table */
__attribute__ ((section(".vectors")))
const void *interrupt_vector[] = {
    /* Stack pointer */
    (void*) (&_estack),             /* pointer to the top of the empty stack */
    /* Cortex-M4 handlers */
    (void*) reset_handler,          /* entry point of the program */
    (void*) isr_nmi,                /* non maskable interrupt handler */
    (void*) isr_hard_fault,         /* if you end up here its not good */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_svc,                /* system call interrupt */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_pendsv,             /* pendSV interrupt, used for task switching in RIOT */
    (void*) isr_systick,            /* SysTick interrupt, not used in RIOT */
    /* MKW01Z128 specific peripheral handlers */
    (void*) isr_dma0,		    /* DMA channel 0 transfer complete */
    (void*) isr_dma1,		    /* DMA channel 1 transfer complete */
    (void*) isr_dma2,		    /* DMA channel 2 transfer complete */
    (void*) isr_dma3,		    /* DMA channel 3 transfer complete */
    (void*) dummy_handler,          /* Reserved interrupt 4 */
    (void*) isr_ftfa,		    /* FTFA command complete */
    (void*) isr_lvd_lvw,            /* Low-Voltage detect and warning */
    (void*) isr_llwu,		    /* Low leakage wakeup */
    (void*) isr_i2c0,		    /* Inter-integrated circuit 0 */
    (void*) isr_i2c1,		    /* Inter-integrated circuit 1 */
    (void*) isr_spi0,		    /* Serial peripheral Interface 0 */
    (void*) isr_spi1,		    /* Serial peripheral Interface 1 */
    (void*) isr_uart0,	            /* UART0 receive/transmit/error interrupt */
    (void*) isr_uart1,	            /* UART1 receive/transmit/error interrupt */
    (void*) isr_uart2,	            /* UART2 receive/transmit/error interrupt */
    (void*) isr_adc0,		    /* Analog-to-digital converter 0 */
    (void*) isr_cmp0,		    /* Comparator 0 */
    (void*) isr_tpm0, 		    /* Timer module 0 status interrupt */
    (void*) isr_tpm1, 		    /* Timer module 1 status interrupt */
    (void*) isr_tpm2, 		    /* Timer module 2 status interrupt */
    (void*) isr_rtc,		    /* Real time clock */
    (void*) isr_rtc_seconds,	    /* Real time clock seconds */
    (void*) isr_pit,		    /* Periodic interrupt timer channel 0 */
    (void*) isr_i2s0,	            /* Integrated interchip sound 0 transmit interrupt */
    (void*) dummy_handler,          /* Reserved interrupt */
    (void*) isr_dac0,		    /* Digital-to-analog converter 0 */
    (void*) isr_tsi0,		    /* TSI0 interrupt */
    (void*) isr_mcg,		    /* Multipurpose clock generator */
    (void*) isr_lptmr0,		    /* Low power timer interrupt */
    (void*) dummy_handler,          /* Reserved interrupt 29 */
    (void*) isr_porta,		    /* Port A pin detect interrupt */
    (void*) isr_portc_portd,	    /* Port C+D pin detect interrupt */
};

/* fcfield table */
__attribute__ ((section(".fcfield")))
const uint8_t flash_configuration_field[] = {
    0xff,    /* backdoor comparison key 3., offset: 0x0 */
    0xff,    /* backdoor comparison key 2., offset: 0x1 */
    0xff,    /* backdoor comparison key 1., offset: 0x2 */
    0xff,    /* backdoor comparison key 0., offset: 0x3 */
    0xff,    /* backdoor comparison key 7., offset: 0x4 */
    0xff,    /* backdoor comparison key 6., offset: 0x5 */
    0xff,    /* backdoor comparison key 5., offset: 0x6 */
    0xff,    /* backdoor comparison key 4., offset: 0x7 */
    0xff,    /* non-volatile p-flash protection 1 - low register, offset: 0x8 */
    0xff,    /* non-volatile p-flash protection 1 - high register, offset: 0x9 */
    0xff,    /* non-volatile p-flash protection 0 - low register, offset: 0xa */
    0xff,    /* non-volatile p-flash protection 0 - high register, offset: 0xb */
    0xfe,    /* non-volatile flash security register, offset: 0xc */
    0xff,    /* non-volatile flash option register, offset: 0xd */
    0xff,    /* non-volatile eeram protection register, offset: 0xe */
    0xff,    /* non-volatile d-flash protection register, offset: 0xf */
};
