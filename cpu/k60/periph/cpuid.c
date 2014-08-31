/*
 * Copyright (C) 2014 FU Berlin
 * Copyright (C) 2014 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_k60
 * @{
 *
 * @file        cpuid.c
 * @brief       CPUID low level implementation
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Joakim Gebart <joakim.gebart@eistec.se
 */

#include <string.h>
#include "cpu-conf.h"

#include "periph/cpuid.h"

void cpuid_get(void *id)
{
    /* Copy all of SIM_UID{H,MH,ML,L} */
    memcpy(id, (void *)(&(SIM->UIDH)), CPU_ID_LEN);
}

/** @} */
