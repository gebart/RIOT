/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_pktdump Dump Packtes to STDOUT
 * @ingroup     net
 * @brief       Dump packets to STDOUT for debugging
 *
 * @{
 *
 * @file
 * @brief       Interface for a generic packet dumping module via netapi
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PKTDUMP_H_
#define PKTDUMP_H_

#include "kernel.h"

kernel_pid_t pktdump_init(char *stack, int stacksize, char priority, char *name);

#endif /* PKTDUMP_H_ */
/** @} */
