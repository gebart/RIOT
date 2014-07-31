/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam3x8e
 * @{
 *
 * @file        uart.c
 * @brief       Implementation of the cpuid interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "cpu-conf.h"
#include "cpuid.h"

/**
 * @name key needs to put into the FCR register for every command
 *
 * See handbook page 312.
 */
#define FLASH_FKEY          (0x5a)

/**
 * @name flash commands that are written to the FCR register
 */
#define CMD_START_READ_UID  (0x0e)
#define CMD_STOP_READ_UID   (0x0f)

void cpuid_get(void *id)
{
    /* write the start read unique identifier command to the flash control register */
    EFC0->EEFC_FCR = (FLASH_FKEY << 24) | CMD_START_READ_UID;



}
