/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis_common
 * @{
 *
 * @file
 * @brief       Implementation of the Kinetis Multipurpose Clock Generator
 *              (MCG) driver.
 *              Please add mcg.h in cpu conf.h
 *              and MCG configuration to periph_conf.h
 *
 *              MCG neighbor modes matrix:
 *              |--------------------------------------------------------
 *              |     | FEI | FEE | FBI | FBE | BLPI | BLPE | PBE | PEE |
 *              |--------------------------------------------------------
 *              |PEE  |  0  |  0  |  0  |  0  |  0   |  0   |  1  |  0  |
 *              |--------------------------------------------------------
 *              |PBE  |  0  |  0  |  0  |  1  |  0   |  1   |  0  |  1  |
 *              |--------------------------------------------------------
 *              |BLPE |  0  |  0  |  0  |  1  |  0   |  0   |  1  |  0  |
 *              |--------------------------------------------------------
 *              |BLPI |  0  |  0  |  1  |  0  |  0   |  0   |  0  |  0  |
 *              |--------------------------------------------------------
 *              |FBE  |  1  |  1  |  1  |  0  |  0   |  1   |  1  |  0  |
 *              |--------------------------------------------------------
 *              |FBI  |  1  |  1  |  0  |  1  |  1   |  0   |  0  |  0  |
 *              |--------------------------------------------------------
 *              |FEE  |  1  |  0  |  1  |  1  |  0   |  0   |  0  |  0  |
 *              |--------------------------------------------------------
 *              |FEI  |  1  |  1  |  1  |  1  |  0   |  0   |  0  |  0  |
 *              |--------------------------------------------------------
 *
 *              Each neighbor mode can be selected directly.
 *              Further, the paths between the following modes are defined:
 *              - from FEI to PEE
 *              - from BLPE to PEE
 *              - from PEE to BLPE
 *              - from FEE to BLPE
 *              - from FEE to BLPI
 *              - from BLPE to FEE
 *              - from BLPI to FEE
 *
 *              MCG configuration Examples (for periph_conf.h):
 *
 *              Example for FEI Mode (MCGOUTCLK = 20MHz ... 25MHz):
 *
 *              #define KINETIS_MCG_USE_ERC               0
 *              #define KINETIS_MCG_DCO_RANGE             (24000000U)
 *
 *
 *              Example for FEE Mode, 32768Hz Crystal,
 *              (MCGOUTCLK = 24MHz):
 *
 *              #define KINETIS_MCG_USE_ERC               1
 *              #define KINETIS_MCG_USE_PLL               0
 *              #define KINETIS_MCG_DCO_RANGE             (24000000U)
 *              #define KINETIS_MCG_ERC_OSCILLATOR        1
 *              #define KINETIS_MCG_ERC_FRDIV             0
 *              #define KINETIS_MCG_ERC_RANGE             0
 *              #define KINETIS_MCG_ERC_FREQ              32768
 *
 *
 *              Example for FEE Mode, external 4MHz reference,
 *              (MCGOUTCLK = 4MHz / 128 * 732 = 22.875MHz):
 *
 *              #define KINETIS_MCG_USE_ERC               1
 *              #define KINETIS_MCG_USE_PLL               0
 *              #define KINETIS_MCG_DCO_RANGE             (24000000U)
 *              #define KINETIS_MCG_ERC_OSCILLATOR        0
 *              #define KINETIS_MCG_ERC_FRDIV             2
 *              #define KINETIS_MCG_ERC_RANGE             1
 *              #define KINETIS_MCG_ERC_FREQ              4000000
 *
 *              Example for PEE Mode, external 4MHz reference,
 *              (MCGOUTCLK = 4MHz / 2 * 24 = 48MHz):
 *
 *              #define KINETIS_MCG_USE_ERC               1
 *              #define KINETIS_MCG_USE_PLL               1
 *              #define KINETIS_MCG_DCO_RANGE             (24000000U)
 *              #define KINETIS_MCG_ERC_OSCILLATOR        0
 *              #define KINETIS_MCG_ERC_FRDIV             2
 *              #define KINETIS_MCG_ERC_RANGE             1
 *              #define KINETIS_MCG_ERC_FREQ              4000000
 *              #define KINETIS_MCG_PLL_PRDIV             1
 *              #define KINETIS_MCG_PLL_VDIV0             0
 *              #define KINETIS_MCG_PLL_FREQ              48000000
 *
 *
 *              The desired mode can be selected in cpu.c:cpu_clock_init()
 *              with kinetis_mcg_set_mode(KINETIS_MCG_PEE);
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @}
 */

#ifndef __MCG_CPU_H
#define __MCG_CPU_H

#include "cpu-conf.h"
#include "periph_conf.h"

#if KINETIS_CPU_USE_MCG

#ifdef __cplusplus
extern "C"
{
#endif

#define KINETIS_MCG_FEI         7 /**< FLL Engaged Internal Mode */
#define KINETIS_MCG_FEE         6 /**< FLL Engaged External Mode */
#define KINETIS_MCG_FBI         5 /**< FLL Bypassed Internal Mode */
#define KINETIS_MCG_FBE         4 /**< FLL Bypassed External Mode */
#define KINETIS_MCG_BLPI        3 /**< FLL Bypassed Low Power Internal Mode */
#define KINETIS_MCG_BLPE        2 /**< FLL Bypassed Low Power External Mode */
#define KINETIS_MCG_PBE         1 /**< PLL Bypassed External Mode */
#define KINETIS_MCG_PEE         0 /**< PLL Engaged External Mode */

uint32_t cpufreq;

/**
 * @brief Initialize the MCG 
 *
 */
int kinetis_mcg_set_mode(uint8_t mode);

#ifdef __cplusplus
}
#endif

#endif /* __MCG_CPU_H */
/** @} */

#endif /* KINETIS_CPU_USE_MCG */
