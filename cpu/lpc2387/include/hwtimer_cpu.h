/*
 * Copyright (C) 2009 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_lpc2387
 * @{
 *
 * @file
 * @brief           CPU specific hwtimer configuration options
 *
 * @author          Heiko Will <hwill@inf.fu-berlin.de>
 * @author          Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef __HWTIMER_CPU_H
#define __HWTIMER_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Hardware timer configuration
 * @{
 */
#define HWTIMER_MAXTIMERS       (4U)            /* Number of hwtimers */
#define HWTIMER_SPEED           (1000000)       /* running with 1MHz */
#define HWTIMER_MAXTICKS        (0xFFFFFFFF)    /* 32-bit timer */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __HWTIMER_CPU_H */
/** @} */
