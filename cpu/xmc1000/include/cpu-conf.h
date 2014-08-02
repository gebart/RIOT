/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_xmc1000 XMC1000 family
 * @brief           XMC1000 specific code
 * @ingroup         cpu
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H

#ifdef CPU_MODEL_XMC1100
#include "xmc1100.h"
#endif


/**
 * @name Kernel configuration
 *
 * The absolute minimum stack size is 140 byte (68 byte for the tcb + 72 byte
 * for a complete context save).
 *
 * TODO: measure and adjust for the Cortex-M0
 * @{
 */
#define KERNEL_CONF_STACKSIZE_PRINTF    (768)

#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT   (768)
#endif

#define KERNEL_CONF_STACKSIZE_IDLE      (192)
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

#endif /* __CPU_CONF_H */
/** @} */
