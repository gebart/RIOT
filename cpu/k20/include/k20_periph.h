/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_k20_definitions
 * @{
 *
 * @file
 * @brief       K20 definitions for inclusion in periph_conf.h
 *
 * @author      Finn Wilke <finn.wilke@fu-berlin.de>
 *
 * @}
 */


#ifndef __K20_PERIPH_H
#define __K20_PERIPH_H

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

/**
 * @name UART0
 */
#define K20_UART0_PORT_A 0
#define K20_UART0_PORT_B 1
#define K20_UART0_PORT_D 2


#ifdef __cplusplus
}
#endif

#endif /* __K20_PERIPH_H */
