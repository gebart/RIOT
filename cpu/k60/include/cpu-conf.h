/*
 * Copyright (C) 2014 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_k60 K60
 * @ingroup         cpu
 * @brief           CPU specific implementations for the K60
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Joakim Gebart <joakim.gebart@eistec.se>
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H

#if defined(CPU_MODEL_K60DN512VLL10)
/* Rev. 2.x silicon */
#include "MK60D10-CMSIS.h"
#elif defined(CPU_MODEL_K60DN512ZVLL10)
/* Rev. 1.x silicon */
#include "MK60DZ10-CMSIS.h"
#else
#error Unknown CPU model. Update Makefile.include in the board directory.
#endif

/**
 * @name Kernel configuration
 * @{
 */
#define KERNEL_CONF_STACKSIZE_PRINTF    (2500)

#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT   (2500)
#endif

#define KERNEL_CONF_STACKSIZE_IDLE      (512)
/** @} */

#endif /* __CPU_CONF_H */
/** @} */
