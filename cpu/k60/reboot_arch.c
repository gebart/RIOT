/*
 * Copyright (C) 2014 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_k60
 * @{
 *
 * @file
 * @brief       Implementation of the kernels reboot interface
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se
 *
 * @}
 */

#include <stdio.h>

#include "arch/reboot_arch.h"
#include "MK60D10-CMSIS.h"


int reboot_arch(int mode)
{
    printf("Going into reboot, mode %i\n", mode);

    NVIC_SystemReset();

    return 0;
}
