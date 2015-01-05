/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_kinetis_common
 * @{
 *
 * @file
 * @brief           CPU specific hwtimer configuration options
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 * @author          Johann Fischer <j.fischer@phytec.de>
 */

#ifndef __HWTIMER_CPU_H
#define __HWTIMER_CPU_H

#include "cpu-conf.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name Hardware timer configuration
 * @{
 */
#define HWTIMER_MAXTIMERS    1                /**< the CPU implementation supports 1 HW timer */
#define HWTIMER_MAXTICKS     (0xFFFFFFFF)     /**< simulating 32-bit timer behavior */
#define HWTIMER_SPEED        LPTIMER_SPEED    /**< speed depends on clock source and prescale */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __HWTIMER_CPU_H */
/** @} */
