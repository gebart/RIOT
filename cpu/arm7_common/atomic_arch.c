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
 * @brief       Implementation of ARM7 atomic functions
 *
 * @author      ?
 * @}
 */

unsigned int atomic_arch_set_return(unsigned int *to_set, unsigned int value)
{
    asm("swp    r2, r1, [r0]");
    asm("mov    r0, r2");
}
