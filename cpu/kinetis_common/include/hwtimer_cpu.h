/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_mkw2x
 * @{
 *
 * @file
 * @brief           CPU specific hwtimer configuration options
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#ifndef __HWTIMER_CPU_H
#define __HWTIMER_CPU_H

/**
 * @name Hardware timer configuration
 * @{
 */
#define HWTIMER_MAXTIMERS   1               /**< the CPU implementation supports 1 HW timers */
#define HWTIMER_SPEED       1000000         /**< the HW timer runs with 1MHz */
#define HWTIMER_MAXTICKS    (0xFFFFFFFF)    /**< 32-bit timer */
/** @} */

#endif /* __HWTIMER_CPU_H */
/** @} */
