/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_k20
 * @{
 *
 * @file
 * @brief       Interrupt configuration and definitions
 *
 * @author      Finn Wilke <finn.wilke@fu-berlin.de>
 *
 * @}
 */


#ifndef __K20_INTERRUPTS_H
#define __K20_INTERRUPTS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name General configuration
 */
#define __NVIC_PRIO_BITS          4       /*!< K20 uses 4 Bits for the Interrupt Priority Levels */


#ifdef __cplusplus
}
#endif

#endif /* __K20_INTERRUPTS_H */
