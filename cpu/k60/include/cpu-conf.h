/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_k60 K60
 * @ingroup         cpu
 * @brief           CPU specific implementations for the K60
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Joakim Gebart <joakim.gebart@eistec.se>
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(CPU_MODEL_K60DN512VLL10) || defined(CPU_MODEL_K60DN256VLL10)

/* Rev. 2.x silicon */
#define K60_CPU_REV 2
#include "MK60D10.h"

/** The expected CPUID value, can be used to implement a check that we are
 * running on the right hardware */
#define K60_EXPECTED_CPUID 0x410fc241u

#elif defined(CPU_MODEL_K60DN512ZVLL10) || defined(CPU_MODEL_K60DN256ZVLL10)

/* Rev. 1.x silicon */
#define K60_CPU_REV 1
#include "MK60DZ10.h"

/** The expected CPUID value, can be used to implement a check that we are
 * running on the right hardware */
#define K60_EXPECTED_CPUID 0x410fc240u

/* Some compatibility defines to minimize the ifdefs needed for the register
 * name changes */

#define SIM_SCGC6_SPI0_MASK SIM_SCGC6_DSPI0_MASK
#define SIM_SCGC6_SPI0_SHIFT SIM_SCGC6_DSPI0_SHIFT

#define MCG_C2_RANGE0_MASK MCG_C2_RANGE_MASK
#define MCG_C5_PRDIV0_MASK MCG_C5_PRDIV_MASK
#define MCG_C6_VDIV0_MASK MCG_C6_VDIV_MASK

#define UART_BASES                               { UART0, UART1, UART2, UART3, UART4, UART5 }

#else
#error Unknown CPU model. Update Makefile.include in the board directory.
#endif

/* Compatibility defines for compatibility with differing module names in MKW2x */
#define SIM_SCGC5_LPTMR_MASK SIM_SCGC5_LPTIMER_MASK
#define SIM_SCGC5_LPTMR_SHIFT SIM_SCGC5_LPTIMER_SHIFT

/* Helpers for the below macro, used to expand the sig variable to a numeric
 * string, even if it is defined as a macro constant. */
#define K60_H_EXPAND_AND_STRINGIFY(s) K60_H_STRINGIFY(s)
#define K60_H_STRINGIFY(s) #s

/**
 * Make the CPU signal to the debugger and break execution by issuing a bkpt
 * instruction.
 */
#define DEBUGGER_BREAK(sig) asm volatile ("bkpt #" K60_H_EXPAND_AND_STRINGIFY(sig) "\n")

/*
 * These are constants that can be used to identify the signal to
 * DEBUGGER_BREAK(). These can not be converted to an enum because
 * DEBUGGER_BREAK() is a macro and not a function
 */
#define BREAK_FAULT_HANDLER 0
#define BREAK_INVALID_PARAM 1
#define BREAK_EXIT 2
#define BREAK_MEMORY_CORRUPTION 3
#define BREAK_WRONG_K60_CPU_REV 99
/* Do not use 0xAB, it is reserved for ARM semihosting environment. */
#define BREAK_SEMIHOSTING 0xAB

/**
 * @name GPIO pin mux function numbers
 */
/** @{ */
#define PIN_MUX_FUNCTION_ANALOG 0
#define PIN_MUX_FUNCTION_GPIO 1
/** @} */
/**
 * @name GPIO interrupt flank settings
 */
/** @{ */
#define PIN_INTERRUPT_RISING 0b1001
#define PIN_INTERRUPT_FALLING 0b1010
#define PIN_INTERRUPT_EDGE 0b1011
/** @} */
/**
 * @name Kernel stack size configuration
 *
 * TODO: Tune this
 * @{
 */
#define KERNEL_CONF_STACKSIZE_PRINTF    (1024)

#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT   (1024)
#endif

#define KERNEL_CONF_STACKSIZE_IDLE      (256)
/** @} */

/**
 * @name Length and address for reading CPU_ID (named UID in Freescale documents)
 * @{
 */
#define CPUID_ID_LEN                    (16)
#define CPUID_ID_PTR                    ((void *)(&(SIM->UIDH)))
/** @} */


/**
 * @name UART0 buffer size definition for compatibility reasons
 *
 * TODO: remove once the remodeling of the uart0 driver is done
 * @{
 */
#ifndef UART0_BUFSIZE
#define UART0_BUFSIZE                   (128)
#endif
/** @} */

/**
 * @name UART driver settings
 */
/** @{ */
/** UART typedef from CPU header. */
#define KINETIS_UART                    UART_Type
/** @} */

/**
 * @name Clock settings for the LPTMR0 timer
 * @{
 */
#define LPTIMER_DEV                      (LPTMR0) /**< LPTIMER hardware module */
#define LPTIMER_CLKEN()                  (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_LPTIMER_SHIFT) = 1)    /**< Enable LPTMR0 clock gate */
#define LPTIMER_CLKDIS()                 (BITBAND_REG(SIM->SCGC5, SIM_SCGC5_LPTIMER_SHIFT) = 0)    /**< Disable LPTMR0 clock gate */
#define LPTIMER_CLKSRC_MCGIRCLK          0    /**< internal reference clock (4MHz) */
#define LPTIMER_CLKSRC_LPO               1    /**< PMC 1kHz output */
#define LPTIMER_CLKSRC_ERCLK32K          2    /**< RTC clock 32768Hz */
#define LPTIMER_CLKSRC_OSCERCLK          3    /**< system oscillator output, clock from RF-Part */

#ifndef LPTIMER_CLKSRC
#define LPTIMER_CLKSRC                   LPTIMER_CLKSRC_ERCLK32K    /**< default clock source */
#endif

#if (LPTIMER_CLKSRC == LPTIMER_CLKSRC_MCGIRCLK)
#define LPTIMER_CLK_PRESCALE    1
#define LPTIMER_SPEED           1000000
#elif (LPTIMER_CLKSRC == LPTIMER_CLKSRC_OSCERCLK)
#define LPTIMER_CLK_PRESCALE    1
#define LPTIMER_SPEED           1000000
#elif (LPTIMER_CLKSRC == LPTIMER_CLKSRC_ERCLK32K)
#define LPTIMER_CLK_PRESCALE    0
#define LPTIMER_SPEED           32768
#else
#define LPTIMER_CLK_PRESCALE    0
#define LPTIMER_SPEED           1000
#endif

/** IRQ priority for hwtimer interrupts */
#define LPTIMER_IRQ_PRIO          1
/** IRQ channel for hwtimer interrupts */
#define LPTIMER_IRQ_CHAN          LPTMR0_IRQn

/* Compatibility definitions */
#if K60_CPU_REV == 1
#define LPTMR0_IRQn LPTimer_IRQn

/* Rev 2.x made the OSC32KSEL field into a bitfield (is a single bit in 1.x) */
#define SIM_SOPT1_OSC32KSEL(a) (SIM_SOPT1_OSC32KSEL_MASK)

/*
 * The CNR register latching in LPTMR0 was added in silicon rev 2.x. With
 * rev 1.x we do not need to do anything in order to read the current timer counter
 * value
 */
#define LPTIMER_CNR_NEEDS_LATCHING 0

#elif K60_CPU_REV == 2

#define LPTIMER_CNR_NEEDS_LATCHING 1

#endif
/** @} */

/**
 * @name Power mode hardware details
 */
/** @{ */
#if K60_CPU_REV == 1
#define KINETIS_PMCTRL MC->PMCTRL
#define KINETIS_PMCTRL_SET_MODE(x) (KINETIS_PMCTRL = MC_PMCTRL_LPLLSM(x) | MC_PMCTRL_LPWUI_MASK)
/* Clear LLS protection, clear VLPS, VLPW, VLPR protection */
/* Note: This register can only be written once after each reset, so we must
 * enable all power modes that we wish to use. */
#define KINETIS_UNLOCK_PMPROT() (MC->PMPROT |= MC_PMPROT_ALLS_MASK | MC_PMPROT_AVLP_MASK)
#elif K60_CPU_REV == 2
#define KINETIS_PMCTRL SMC->PMCTRL
#define KINETIS_PMCTRL_SET_MODE(x) (KINETIS_PMCTRL = SMC_PMCTRL_STOPM(x) | SMC_PMCTRL_LPWUI_MASK)
#define KINETIS_PMPROT_UNLOCK() (SMC->PMPROT |= SMC_PMPROT_ALLS_MASK | SMC_PMPROT_AVLP_MASK)
#else
#error Unknown K60 CPU revision!
#endif

#define KINETIS_POWER_MODE_NORMAL (0b000)
#define KINETIS_POWER_MODE_VLPS   (0b010)
#define KINETIS_POWER_MODE_LLS    (0b011)

/**
 * @brief Wake up source number for the LPTMR0
 *
 * In order to let the hwtimer wake the CPU from low power modes, we need to
 * enable this wake up source.
 */
#define KINETIS_LLWU_WAKEUP_MODULE_LPTMR 0

/**
 * @brief IRQn name to enable LLWU IRQ in NVIC
 */
#define KINETIS_LLWU_IRQ LLW_IRQn

/**
 * @brief Enable clock gate on LLWU module.
 */
#define LLWU_UNLOCK() (BITBAND_REG(SIM->SCGC4, SIM_SCGC4_LLWU_SHIFT) = 1)

/**
 * Internal modules whose interrupts are mapped to LLWU wake up sources.
 *
 * Wake up source module number: Kinetis Module
 * 0: LPTMR
 * 1: CMP0
 * 2: CMP1
 * 3: CMP2
 * 4: TSI
 * 5: RTC Alarm
 * 6: Reserved
 * 7: RTC Seconds
 *
 * Other modules CAN NOT be used to wake the CPU from LLS or VLLSx power modes.
 */
typedef enum llwu_wakeup_module {
    LLWU_WAKEUP_MODULE_LPTMR = 0,
    LLWU_WAKEUP_MODULE_CMP0 = 1,
    LLWU_WAKEUP_MODULE_CMP1 = 2,
    LLWU_WAKEUP_MODULE_CMP2 = 3,
    LLWU_WAKEUP_MODULE_TSI = 4,
    LLWU_WAKEUP_MODULE_RTC_ALARM = 5,
    LLWU_WAKEUP_MODULE_RESERVED = 6,
    LLWU_WAKEUP_MODULE_RTC_SECONDS = 7,
    LLWU_WAKEUP_MODULE_END,
} llwu_wakeup_module_t;

/** enum that maps physical pins to wakeup pin numbers in LLWU module
 *
 * Other modules CAN NOT be used to wake the CPU from LLS or VLLSx power modes. */
typedef enum llwu_wakeup_pin {
    LLWU_WAKEUP_PIN_PTE1 = 0,
    LLWU_WAKEUP_PIN_PTE2 = 1,
    LLWU_WAKEUP_PIN_PTE4 = 2,
    LLWU_WAKEUP_PIN_PTA4 = 3,
    LLWU_WAKEUP_PIN_PTA13 = 4,
    LLWU_WAKEUP_PIN_PTB0 = 5,
    LLWU_WAKEUP_PIN_PTC1 = 6,
    LLWU_WAKEUP_PIN_PTC3 = 7,
    LLWU_WAKEUP_PIN_PTC4 = 8,
    LLWU_WAKEUP_PIN_PTC5 = 9,
    LLWU_WAKEUP_PIN_PTC6 = 10,
    LLWU_WAKEUP_PIN_PTC11 = 11,
    LLWU_WAKEUP_PIN_PTD0 = 12,
    LLWU_WAKEUP_PIN_PTD2 = 13,
    LLWU_WAKEUP_PIN_PTD4 = 14,
    LLWU_WAKEUP_PIN_PTD6 = 15,
    LLWU_WAKEUP_PIN_END
} llwu_wakeup_pin_t;

typedef enum llwu_wakeup_edge {
    LLWU_WAKEUP_EDGE_DISABLE = 0b00,
    LLWU_WAKEUP_EDGE_RISING = 0b01,
    LLWU_WAKEUP_EDGE_FALLING = 0b10,
    LLWU_WAKEUP_EDGE_ANY = 0b11,
} llwu_wakeup_edge_t;

/** @} */

#define TRANSCEIVER_BUFFER_SIZE (3)

#ifdef __cplusplus
}
#endif

#endif /* __CPU_CONF_H */
/** @} */
