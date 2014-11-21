/*
 * Copyright (C) 2013, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_lpc2387 NXP LPC2387
 * @ingroup     cpu
 * @brief       NXP LPC2387 specific code
 * @{
 *
 * @file
 * @brief       CPU configuration for the LPC2387
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lpc23xx.h"
#include "lpc2387.h"

/**
 * @name Kernel configuration
 * @{
 */
#define KERNEL_CONF_STACKSIZE_PRINTF    (2048)
#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT   (512)
#endif
#define KERNEL_CONF_STACKSIZE_IDLE      (160)
/** @} */

/**
 * @name UART0 buffer size definition for compatibility reasons
 *
 * TODO: remove once the remodeling of the uart0 driver is done
 * @{
 */
#ifndef UART0_BUFSIZE
#define UART0_BUFSIZE                   (64)
#endif
/** @} */

/**
 * @name Define the transceivers buffer size
 *
 * TODO: remove once implementations is moved to netdev interface
 * @{
 */
#define TRANSCEIVER_BUFFER_SIZE (10)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __CPUCONF_H */
/** @} */
