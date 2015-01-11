/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_k20 K20
 * @ingroup         cpu_k20
 * @brief           CPU specific implementations for the K20
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Finn Wilke <finn.wilke@fu-berlin.de>
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 *
 * @note    This implementation currently only supports the k20_50 subfamily without FPU
 *          and a max frequency of 50 Mhz. The newer K20 devices with higher frequencies
 *          can probably be added easily.
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Select the k20 CPU family header
 */
#include "k20_family.h"

#ifdef CPU_FAMILY_MK20D5
#include "cmsis/MK20D5.h"
#endif

#include "periph_conf.h"
#include "k20_interrupts.h"
#include "core_cm4.h"

/**
 * @name Kernel configuration
 *
 * TODO: measure and adjust
 * @{
 */
#define KERNEL_CONF_STACKSIZE_PRINTF    (1024)

#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT   (2048)
#endif

#define KERNEL_CONF_STACKSIZE_IDLE      (256)
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
 * @name CC110X buffer size definitions for the k20
 * @{
 */
#ifdef MODULE_CC110X
#define TRANSCEIVER_BUFFER_SIZE         (10)
#define RX_BUF_SIZE                     (10)
#endif
/** @} */

/**
 * @name Cortex M4-common configuration
 * @{
 */
#define __CM4_REV                 0x0001  /*!< Core revision r0p1                                */
#define __Vendor_SysTickConfig    0       /*!< Set to 1 if different SysTick Config is used      */
#define __FPU_PRESENT             0       /*!< FPU present                                       */
/** @{ */

/**
 * @name kinetis definitions
 * @{
 */


/**
 * @name kinetis_common definitions
 * @{
 */

/**
 * @name misc
 */
#define CPUID_ID_PTR ((void *)(&(SIM->UIDH)))
#define CPUID_ID_LEN 16

/**
 * @name LPTMR
 */

/* work around header differences */
#define SIM_SCGC5_LPTMR_MASK SIM_SCGC5_LPTIMER_MASK

/** clocks */
#define LPTIMER_CLKSRC_MCGIRCLK          0    /**< internal reference clock (4MHz) */
#define LPTIMER_CLKSRC_LPO               1    /**< PMC 1kHz output */
#define LPTIMER_CLKSRC_ERCLK32K          2    /**< RTC clock 32768Hz */
#define LPTIMER_CLKSRC_OSCERCLK          3    /**< system oscillator output, clock from RF-Part */

#ifndef LPTIMER_CLKSRC
#define LPTIMER_CLKSRC                   LPTIMER_CLKSRC_LPO    /**< default clock source */
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
/** @} */

/**
 * @name PIT timer (currently not working)
 */
static __INLINE void TIMER_0_CLKEN(void)
{
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
}


#define TIMER_0_IRQ_CHAN PIT1_IRQn
#define TIMER_1_IRQ_CHAN PIT3_IRQn
#define TIMER_0_DEV PIT

/* I don't know?!?! */
#define TIMER_0_CLOCK 1
/** @} */

/**
 * @name UART
 * @{
 */
#define UART_0_DEV UART0
#define UART_0_ISR isr_uart0_status
#define UART_0_IRQ_CHAN UART0_RX_TX_IRQn
#define UART_0_CLK CLOCK_CORECLOCK
static __INLINE void UART_0_CLKEN(void) {
    SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
}

/** We ifdef these things as there are different UART0 pins. If you define PORT you define everything */
#ifndef UART_0_PORT

#if defined(K20_UART0_OUTPUT_PTA_1_2)
#define UART_0_PORT PORTA
#define UART_0_SCG5_PORT_MASK SIM_SCGC5_PORTA_MASK
#define UART_0_TX_PIN 1
#define UART_0_RX_PIN 2
#define UART_0_AF 2

#elif defined(K20_UART0_OUTPUT_PTB_16_17)
#define UART_0_PORT PORTB
#define UART_0_SCG5_PORT_MASK SIM_SCGC5_PORTB_MASK
#define UART_0_TX_PIN 16
#define UART_0_RX_PIN 17
#define UART_0_AF 3

#elif defined(K20_UART0_OUTPUT_PTD_6_7)
#define UART_0_PORT PORTD
#define UART_0_SCG5_PORT_MASK SIM_SCGC5_PORTD_MASK
#define UART_0_TX_PIN 6
#define UART_0_RX_PIN 7
#define UART_0_AF 3

#else
#error "You need to define K20_UART0_OUTPUT_PTA_1_2, K20_UART0_OUTPUT_PTB_16_17 or K20_UART0_OUTPUT_PTD_6_7 depending on which output you want to use"
#endif

static __INLINE void UART_0_PORT_CLKEN(void) {
    SIM->SCGC5 |= UART_0_SCG5_PORT_MASK;
}

#endif // UART_0_PORT

#define UART_1_DEV UART1
#define UART_1_ISR isr_uart1_status
#define UART_1_IRQ_CHAN UART1_RX_TX_IRQn
#define UART_1_CLK CLOCK_CORECLOCK
static __INLINE void UART_1_CLKEN(void) {
    SIM->SCGC4 |= SIM_SCGC4_UART1_MASK;
}

#ifndef UART_1_PORT
#define UART_1_PORT PORTC
#define UART_1_TX_PIN 3
#define UART_1_RX_PIN 4
#define UART_1_AF 3

static __INLINE void UART_1_PORT_CLKEN(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
}
#endif

#define UART_2_DEV UART1
#define UART_2_IRQ_CHAN UART1_RX_TX_IRQn
#define UART_2_CLK F_MCGOUTCLK
static __INLINE void UART_2_CLKEN(void)
{
    SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
}

#ifndef UART_2_PORT
#define UART_2_ISR isr_uart1_status
#define UART_2_PORT PORTD
#define UART_2_TX_PIN 2
#define UART_2_RX_PIN 3
#define UART_2_AF 3

static __INLINE void UART_2_PORT_CLKEN(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
}
#endif
/** @} */



/** @} */


/**
 * @name Clock gate definition used by the kinetis SIM driver
 *
 * The values are calculated as follows:
 * clockgate / 0x20 = id of SCG register
 * clockgate % 0x20 = shift for value
 */
typedef enum {
    /* SCGC4 */
    K20_CGATE_VREF   = 0x80 + 20,
    K20_CGATE_CMP    = 0x80 + 19,
    K20_CGATE_USBOTG = 0x80 + 18,
    K20_CGATE_UART2  = 0x80 + 12,
    K20_CGATE_UART1  = 0x80 + 11,
    K20_CGATE_UART0  = 0x80 + 10,
    K20_CGATE_I2C0   = 0x80 + 6,
    K20_CGATE_CMT    = 0x80 + 2,
    K20_CGATE_EWM    = 0x80 + 1,

    /* SCGC5 */
    K20_CGATE_PORTE  = 0xA0 + 13,
    K20_CGATE_PORTD  = 0xA0 + 12,
    K20_CGATE_PORTC  = 0xA0 + 11,
    K20_CGATE_PORTB  = 0xA0 + 10,
    K20_CGATE_PORTA  = 0xA0 + 9,
    K20_CGATE_TSI    = 0xA0 + 5,
    K20_CGATE_LPTMR  = 0xA0 + 0,

    /* SCGC6 */
    K20_CGATE_RTC    = 0xC0 + 29,
    K20_CGATE_ADC0   = 0xC0 + 27,
    K20_CGATE_FTM1   = 0xC0 + 25,
    K20_CGATE_FTM0   = 0xC0 + 24,
    K20_CGATE_PIT    = 0xC0 + 23,
    K20_CGATE_PDB    = 0xC0 + 22,
    K20_CGATE_USBDCD = 0xC0 + 21,
    K20_CGATE_CRC    = 0xC0 + 18,
    K20_CGATE_I2S    = 0xC0 + 15,
    K20_CGATE_SPI0   = 0xC0 + 12,
    K20_CGATE_DMAMUX = 0xC0 + 1,
    K20_CGATE_FTFL   = 0xC0 + 0,

    /* SCGC7 */
    K20_CGATE_DMA    = 0xE0 + 01
} kinetis_clock_gate_t;
/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __CPU_CONF_H */
/** @} */
