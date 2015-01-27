/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    nomac Link layer with no medium access
 * @ingroup     net
 * @brief       Link layer protocol to speak with any transceiver driver
 *              directly without any medium access.
 * @{
 *
 * @file
 * @brief       Link layer protocol to speak with any transceiver driver
 *              directly without any medium access.
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __NOMAC_H_
#define __NOMAC_H_

#include <stdlib.h>

#include "netdev.h"
#include "netapi.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NOMAC_REGISTRY_SIZE
/**
 * @brief   The size of NOMAC's registry of receiving threads.
 */
#define NOMAC_REGISTRY_SIZE (1)
#endif

/**
 * @brief   Recommended stack size for a NOMAC thread
 * TODO: determine real minimal size based on thread_print_all() output
 */
#define NOMAC_DEFAULT_STACKSIZE     (KERNEL_CONF_STACKSIZE_DEFAULT)

/**
 * @brief   Initialize new NOMAC layer.
 *
 * @param[in] stack     Stack for the control thread
 * @param[in] stacksize Size of *stack*
 * @param[in] priority  Priority for the control thread
 * @param[in] name      Name for the control thread
 * @param[in] dev       An *initialized* network device to use with this MAC
 *                      layer
 *
 * @see @ref thread_create
 *
 * @return  PID of NOMAC control thread on success
 * @return  -EINVAL if priority is invalid
 * @return  -EOVERFLOW if no slot for the new thread is available
 */
kernel_pid_t nomac_init(char *stack, int stacksize, char priority,
                        const char *name, netdev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* __NOMAC_H_ */
/** @} */
