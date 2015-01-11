/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_k20_definitions
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Finn Wilke <finn.wilke@fu-berlin.de>
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
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

#if CPU_FAMILY == MK20D5
#include "cmsis/MK20D5.h"
#elif CPU_FAMILY == MK20D7
#include "cmsis/MK20D7.h"
#elif CPU_FAMILY == MK20D10
#include "cmsis/MK20D10.h"
#elif CPU_FAMILY == MK20DZ10
#include "cmsis/MK20Dz10.h"
#elif CPU_FAMILY == MK20F12
#include "cmsis/MK20F12.h"
#endif

#include "periph_conf.h"

/**
 * @name clock configuration
 */
#if CLOCK_DESIRED_CORECLOCK > CPU_MAX_CORE_CLOCK_SPEED
#error "CPU clock too high! Check CLOCK_DESIRED_CORECLOCK and CPU_MAX_CORE_CLOCK_SPEED"
#endif

#if   CLOCK_DESIRED_CORECLOCK <= 25000000
#define CLOCK_MCG_DRST_CONFIG 0

#ifdef K20_USE_UNSAFE_FLL_FACTOR_ON_INTERNAL_OSCILLATOR
#define CLOCK_CORECLOCK (20971520U)
#else
#define CLOCK_CORECLOCK (23986176U)
#endif

#elif CLOCK_DESIRED_CORECLOCK <= 50000000
#define CLOCK_MCG_DRST_CONFIG 1

#ifdef K20_USE_UNSAFE_FLL_FACTOR_ON_INTERNAL_OSCILLATOR
#define CLOCK_CORECLOCK (47972352U)
#else
#define CLOCK_CORECLOCK (41943040U)
#endif

#elif CLOCK_DESIRED_CORECLOCK <= 75000000
#define CLOCK_MCG_DRST_CONFIG 2

#ifdef K20_USE_UNSAFE_FLL_FACTOR_ON_INTERNAL_OSCILLATOR
#define CLOCK_CORECLOCK (62914560U)
#else
#define CLOCK_CORECLOCK (71991296U)
#endif

#elif CLOCK_DESIRED_CORECLOCK <= 100000000
#define CLOCK_MCG_DRST_CONFIG 3

#ifdef K20_USE_UNSAFE_FLL_FACTOR_ON_INTERNAL_OSCILLATOR
#define CLOCK_CORECLOCK (83886080U)
#else
#define CLOCK_CORECLOCK (95977472U)
#endif

#endif

/**
 * @name Clockgate functions
 * @{
 */
static __INLINE void PORTA_CLKEN(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
}
static __INLINE void PORTB_CLKEN(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
}
static __INLINE void PORTC_CLKEN(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
}
static __INLINE void PORTD_CLKEN(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
}
static __INLINE void PORTE_CLKEN(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
}

static __INLINE void PORTA_CLKDIS(void) {
    SIM->SCGC5 &= ~SIM_SCGC5_PORTA_MASK;
}
static __INLINE void PORTB_CLKDIS(void) {
    SIM->SCGC5 &= ~SIM_SCGC5_PORTB_MASK;
}
static __INLINE void PORTC_CLKDIS(void) {
    SIM->SCGC5 &= ~SIM_SCGC5_PORTC_MASK;
}
static __INLINE void PORTD_CLKDIS(void) {
    SIM->SCGC5 &= ~SIM_SCGC5_PORTD_MASK;
}
static __INLINE void PORTE_CLKDIS(void) {
    SIM->SCGC5 &= ~SIM_SCGC5_PORTE_MASK;
}
/** @} */

/**
 * @defgroup k20_kernel_config Kernel configuration
 * @ingroup k20_definitions
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
 * kinetis_common definitions
 */

/**
 * @name misc
 * @{
 */
#define CPUID_ID_PTR ((void *)(&(SIM->UIDH)))
#define CPUID_ID_LEN 16
/** @} */

/**
 * @name LPTMR
 * @{
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
 * @{
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

#ifdef __cplusplus
}
#endif

#endif /* __CPU_CONF_H */

/** @} */
