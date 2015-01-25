/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_cortexm0_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernels atomic interface
 *
 * @author      Stefan Pfeiffer <stefan.pfeiffer@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */

#include "arch/atomic_arch.h"
#include "irq.h"


unsigned int atomic_arch_set_return(unsigned int *to_set, unsigned int value)
{
    disableIRQ();
    unsigned int old = *to_set;
    *to_set = value;
    enableIRQ();
    return old;
}


int atomic_inc(int *val)
{
    int old_val;
    unsigned int irqmask = disableIRQ();
    old_val = (*val)++;
    restoreIRQ(irqmask);
    return old_val;
}

int atomic_dec(int *val)
{
    int old_val;
    unsigned int irqmask = disableIRQ();
    old_val = (*val)--;
    restoreIRQ(irqmask);
    return old_val;
}
