/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_k60
 * @{
 *
 * @file
 * @brief           CPU specific hwtimer configuration options
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef __HWTIMER_CPU_H
#define __HWTIMER_CPU_H

#include "cpu.h"
/* For F_BUS, F_SYS etc */
#include "periph_conf.h"

/**
 * @name Hardware timer configuration
 * @{
 */
#define HWTIMER_MAXTIMERS   3               /**< the CPU implementation supports 4 HW timers, but we are dedicating one to serve as a time reference. */
#define HWTIMER_SPEED       F_BUS           /**< the HW timer (PIT module) is clocked by the bus clock */
#define HWTIMER_MAXTICKS    (0xFFFFFFFF)    /**< 32-bit timer */
/** @} */

#endif /* __HWTIMER_CPU_H */
/** @} */
