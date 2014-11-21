/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_arm7_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernel's reboot interface
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include "cpu.h"
#include "irq.h"

int reboot_arch(int mode)
{
    disableIRQ();
    WDTC = 0x0FFFF;
    WDMOD = 0x03;
    WDFEED= 0xAA;
    WDFEED= 0x55;
    while(1);
}
