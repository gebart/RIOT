/*
 * Copyright (C) 2013, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     lpc2387
 * @{
 */

#include "cpu.h"
#include <stdint.h>

/* from arm_cpu.h ********************************/


uint32_t get_system_speed(void);
void cpu_clock_scale(uint32_t source, uint32_t target, uint32_t *prescale);

void arm_reset(void);
void stdio_flush(void);
/** **/

#define VICIntEnClear VICIntEnClr


#define PCRTC           BIT9
#define CL_CPU_DIV      4

#define WD_INTERVAL     10      ///< number of seconds before WD triggers

#define F_CCO                   288000000
#define CL_CPU_DIV              4                                   ///< CPU clock divider
// #define F_CPU                   (F_CCO / CL_CPU_DIV)                ///< CPU target speed in Hz
#define F_RC_OSCILLATOR         4000000                             ///< Frequency of internal RC oscillator
#define F_RTC_OSCILLATOR        32767                               ///< Frequency of RTC oscillator


#define GPIO_INT 17
#define IRQP_GPIO 4

#define _XTAL       (72000)

#define RX_BUF_SIZE  (10)

#ifndef UART0_BUFSIZE
#define UART0_BUFSIZE                   (128)
#endif

#define HWTIMER_MSEC  (HWTIMER_SPEED/1000)
#define HWTIMER_SEC   (HWTIMER_SPEED)



/**
 * @name Stdlib configuration
 * @{
 */
#define __FOPEN_MAX__       4
#define __FILENAME_MAX__    12
/** @} */

/**
 * @name Compiler specifics
 * @{
 */
#define CC_CONF_INLINE                  inline
#define CC_CONF_USED                    __attribute__((used))
#define CC_CONF_NONNULL(...)            __attribute__((nonnull(__VA_ARGS__)))
#define CC_CONF_WARN_UNUSED_RESULT      __attribute__((warn_unused_result))
/** @} */

// extern uintptr_t __stack_start;     ///< end of user stack memory space

void lpc2387_pclk_scale(uint32_t source, uint32_t target, uint32_t *pclksel, uint32_t *prescale);
bool install_irq(int IntNumber, void (*HandlerAddr)(void), int Priority);


static inline void pllfeed(void)
{
    PLLFEED = 0xAA;
    PLLFEED = 0x55;
}


void cpu_init(void)
{
    // Disconnect PLL
    PLLCON &= ~0x0002;
    pllfeed();

    // Disable PLL
    PLLCON &= ~0x0001;
    pllfeed();

    SCS |= 0x20;                        // Enable main OSC

    while (!(SCS & 0x40));              // Wait until main OSC is usable

    /* select main OSC, 16MHz, as the PLL clock source */
    CLKSRCSEL = 0x0001;

    // Setting Multiplier and Divider values
    PLLCFG = 0x0008;                    // M=9 N=1 Fcco = 288 MHz
    pllfeed();

    // Enabling the PLL */
    PLLCON = 0x0001;
    pllfeed();

    /* Set clock divider to 4 (value+1) */
    CCLKCFG = CL_CPU_DIV - 1;           // Fcpu = 72 MHz

#if USE_USB
    USBCLKCFG = USBCLKDivValue;     /* usbclk = 288 MHz/6 = 48 MHz */
#endif
}


/*---------------------------------------------------------------------------*/
/**
 * @brief   Enabling MAM and setting number of clocks used for Flash memory fetch
 * @internal
 */
static void
init_mam(void)
{
    MAMCR  = 0x0000;
    MAMTIM = 0x0003;
    MAMCR  = 0x0002;
}
/*---------------------------------------------------------------------------*/
void init_clks2(void)
{
    // Wait for the PLL to lock to set frequency
    while (!(PLLSTAT & BIT26));

    // Connect the PLL as the clock source
    PLLCON = 0x0003;
    pllfeed();

    /* Check connect bit status */
    while (!(PLLSTAT & BIT25));
}

void watchdog_init(void)
{
    WDCLKSEL = 0;                                   // clock source: RC oscillator
    WDMOD &= ~WDTOF;                                // clear time-out flag
    WDTC = (F_RC_OSCILLATOR / 4) * WD_INTERVAL;
}


void bl_init_clks(void)
{
    watchdog_init();
    PCONP = PCRTC;          // switch off everything except RTC
    init_clks1();
    init_clks2();
    init_mam();
}


void lpc2387_pclk_scale(uint32_t source, uint32_t target, uint32_t *pclksel, uint32_t *prescale)
{
    uint32_t pclkdiv;
    *prescale = source / target;

    if ((*prescale % 16) == 0) {
        *pclksel = 3;
        pclkdiv = 8;
    }
    else if ((*prescale % 8) == 0) {
        *pclksel = 0;
        pclkdiv = 4;
    }
    else if ((*prescale % 4) == 0) {
        *pclksel = 2;
        pclkdiv = 2;
    }
    else {
        *pclksel = 1;
        pclkdiv = 1;
    }

    *prescale /= pclkdiv;

    if (*prescale % 2) {
        (*prescale)++;
    }
}

void cpu_clock_scale(uint32_t source, uint32_t target, uint32_t *prescale)
{
    uint32_t pclksel;

    lpc2387_pclk_scale(source, target, &pclksel, prescale);

    PCLKSEL0 = (PCLKSEL0 & ~(BIT2 | BIT3)) | (pclksel << 2);        // timer 0
    PCLKSEL0 = (PCLKSEL0 & ~(BIT4 | BIT5)) | (pclksel << 4);        // timer 1
    PCLKSEL1 = (PCLKSEL1 & ~(BIT12 | BIT13)) | (pclksel << 12); // timer 2
}

/** @} */
