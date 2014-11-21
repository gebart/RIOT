/* Copyright (C) 2005, 2006, 2007, 2008 by Thomas Hillebrandt and Heiko Will
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

#ifndef __ARM_COMMON_H
#define __ARM_COMMON_H

/**
 * @ingroup         arm_common
 * @{
 */


/**
 * @name    IRQ Priority Mapping
 */
//@{
#define HIGHEST_PRIORITY    0x01
#define IRQP_RTIMER         1   // FIQ_PRIORITY // TODO: investigate problems with rtimer and FIQ
#define IRQP_TIMER1         1
#define IRQP_WATCHDOG       1
#define IRQP_CLOCK          3
#define IRQP_GPIO           4
#define IRQP_RTC            8
#define LOWEST_PRIORITY     0x0F
// @}


// #define VECT_ADDR_INDEX 0x100
// #define VECT_CNTL_INDEX 0x200

#include <stdbool.h>
#include "cpu.h"


/** @} */
#endif /*ARMVIC_H_*/
