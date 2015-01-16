/*
 * Copyright (C) 2014 Eistec AB
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
 * @brief       Implementation of the kernel's power management interface
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */

#include "cpu.h"
#include "arch/lpm_arch.h"

#define LLWU_ENABLE_LPTMR() (BITBAND_REG(LLWU->ME, KINETIS_LLWU_WAKEUP_MODULE_LPTMR) = 1)

/*
 * Counting semaphores for inhibiting unsuitable power modes.
 */
volatile int kinetis_lpm_inhibit_stop_sema = 0;
volatile int kinetis_lpm_inhibit_vlps_sema = 0;
volatile int kinetis_lpm_inhibit_lls_sema = 0;

static inline void wait(void) {
  /* Clear the SLEEPDEEP bit to make sure we go into WAIT (sleep) mode instead
   * of deep sleep.
   */
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;

  /* WFI instruction will start entry into WAIT mode */
  __WFI();
}

/**
 * @brief Switch the CPU into a STOP mode
 *
 * @param[in] stopmode     The chosen STOP mode
 *
 * @see Kinetis CPU reference manual, chapter System Mode Controller, Power Mode
 * Control register (SMC_PMCTRL), field STOPM
 */
static inline void stop(uint8_t stopmode) {
    uint8_t dummy;
    /* Set the SLEEPDEEP bit to enable deep sleep modes (STOP) */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    /* Write new mode to PMCTRL register */
    KINETIS_PMCTRL_SET_MODE(stopmode);

    /* Wait for the write to complete before stopping core */
    dummy = KINETIS_PMCTRL;
    (void)dummy; /* Avoid warnings about set but not used variable [-Wunused-but-set-variable] */

    /* WFI instruction will start entry into STOP mode */
    __WFI();
}

/** @todo Implement Kinetis power modes: VLPR, VLPW, VLLSx */

static void kinetis_low_power_mode(void)
{
    wait();
    return;
    /* Check if any UARTs are currently receiving data, if we go to STOP mode we
     * will lose the byte in progress. */
    /* It is necessary to check the SIM_SCGCx registers to avoid hardfaulting when
     * we try to read a disabled peripheral */
    /* FIXME: It is possible that this could be implemented as a part of the
     * UART module interrupt by enabling the RXEDGIF IRQ and setting
     * inhibit_stop when RXEDGIF occurs. */
    if(((SIM->SCGC4 & SIM_SCGC4_UART0_MASK) && (UART0->S2 & UART_S2_RAF_MASK)) ||
        ((SIM->SCGC4 & SIM_SCGC4_UART1_MASK) && (UART1->S2 & UART_S2_RAF_MASK)) ||
        ((SIM->SCGC4 & SIM_SCGC4_UART2_MASK) && (UART2->S2 & UART_S2_RAF_MASK)) ||
        ((SIM->SCGC4 & SIM_SCGC4_UART3_MASK) && (UART3->S2 & UART_S2_RAF_MASK)) ||
        ((SIM->SCGC1 & SIM_SCGC1_UART4_MASK) && (UART4->S2 & UART_S2_RAF_MASK)) ||
        ((SIM->SCGC1 & SIM_SCGC1_UART5_MASK) && (UART5->S2 & UART_S2_RAF_MASK))) {
        wait();
    }
    else if(kinetis_lpm_inhibit_stop_sema != 0) {
        /* STOP inhibited, go to WAIT mode */
        wait();
    }
    else if(kinetis_lpm_inhibit_vlps_sema != 0) {
        /* VLPS inhibited, go to normal STOP mode */
        stop(KINETIS_POWER_MODE_NORMAL);
    }
    else if(kinetis_lpm_inhibit_lls_sema != 0) {
        /* LLS inhibited, go to VLPS mode */
        stop(KINETIS_POWER_MODE_VLPS);
    }
    else {
        /* go to LLS mode */
        stop(KINETIS_POWER_MODE_LLS);
    }
}

void lpm_arch_init(void)
{
    /* Setup Low Leakage Wake-up Unit (LLWU) */
    LLWU_UNLOCK();           /* Enable LLWU clock gate */
    KINETIS_PMPROT_UNLOCK(); /* Enable all available power modes */

    NVIC_EnableIRQ(KINETIS_LLWU_IRQ); /* Enable LLWU interrupt */

}

enum lpm_mode lpm_arch_set(enum lpm_mode target)
{
    switch (target)
    {
        case LPM_ON:
            /* MCU is active, do not go to low power */
            break;
        case LPM_IDLE:
        case LPM_SLEEP:
        case LPM_POWERDOWN:
        case LPM_OFF:
            /* Go to the lowest power mode the currently active peripherals will
             * allow for. */
            kinetis_low_power_mode();
            break;
        case LPM_UNKNOWN:
        default:
        break;
    }
    return 0;
}

enum lpm_mode lpm_arch_get(void)
{
    /* TODO */
    return 0;
}

void lpm_arch_awake(void)
{
    /* TODO */
}

void lpm_arch_begin_awake(void)
{
    /* TODO */
}

void lpm_arch_end_awake(void)
{
    /* TODO */
}
