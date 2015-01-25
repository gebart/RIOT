/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  core_util
 * @{
 *
 * @file        atomic.h
 * @brief       Atomic getter and setter functions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 */

#ifndef _ATOMIC_H
#define _ATOMIC_H

#include "arch/atomic_arch.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief Sets a new and returns the old value of a variable atomically
 *
 * @param[in] val   The variable to be set
 * @param[in] set   The value to be written
 *
 * @return The old value of *val*
 */
unsigned int atomic_set_return(unsigned int *val, unsigned int set);

/**
 * @brief Increment a counter variable by one atomically and return the old value.
 *
 * @param[in]  val   Pointer to a counter variable.
 *
 * @return The value of *val* before the increment.
 */
int atomic_inc(int *val);

/**
 * @brief Decrement a counter variable by one atomically and return the old value.
 *
 * @param[in]  val   Pointer to a counter variable.
 *
 * @return The value of *val* before the decrement.
 */
int atomic_dec(int *val);

#ifdef __cplusplus
}
#endif

#endif /* _ATOMIC_H */
/** @} */
