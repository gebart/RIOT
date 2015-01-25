/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */


/**
 * @ingroup     core_util
 * @{
 *
 * @file
 * @brief       Generic implementation of the kernel's atomic interface
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */

#include "atomic.h"
#include "irq.h"
#include "cpu.h"

/* Set ARCH_HAS_ATOMIC_INT within cpu.h to override these functions */
#if (ARCH_HAS_ATOMIC_INT != 0)

unsigned int atomic_set_return(unsigned int *val, unsigned int set)
{
    unsigned int mask = disableIRQ();
    unsigned int old_val = *val;
    *val = set;
    restoreIRQ(mask);
    return old_val;
}

int atomic_inc(int *val)
{
    unsigned int mask = disableIRQ();
    int old_val;
    old_val = (*val)++;
    restoreIRQ(mask);
    return old_val;
}

int atomic_dec(int *val)
{
    unsigned int mask = disableIRQ();
    int old_val;
    old_val = (*val)--;
    restoreIRQ(mask);
    return old_val;
}

#endif
