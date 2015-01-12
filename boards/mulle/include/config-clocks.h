/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_mulle
 * @{
 *
 * @file
 * @name        K60 clock configuration defines for the Eistec Mulle
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 */

#ifndef CONFIG_CLOCKS_H_
#define CONFIG_CLOCKS_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Base clocks */
#define CPU_XTAL_CLK_HZ                 8000000u /**< Value of the external crystal or oscillator clock frequency in Hz */
#define CPU_XTAL32k_CLK_HZ              32768u   /**< Value of the external 32k crystal or oscillator clock frequency in Hz */
#define CPU_INT_SLOW_CLK_HZ             32768u   /**< Value of the slow internal oscillator clock frequency in Hz  */
#define CPU_INT_FAST_CLK_HZ             4000000u /**< Value of the fast internal oscillator clock frequency in Hz  */
#define DEFAULT_SYSTEM_CLOCK            96000000u /**< Default System clock value */

/**
 * System clock divider setting, the actual hardware register value, see reference manual for details.
 */
#define CONFIG_CLOCK_K60_SYS_DIV 0x00

/**
 * Bus clock divider setting, the actual hardware register value, see reference manual for details
 */
#define CONFIG_CLOCK_K60_BUS_DIV 0x01

/**
 * Flexbus clock divider setting, the actual hardware register value, see reference manual for details
 */
#define CONFIG_CLOCK_K60_FB_DIV 0x01

/**
 * Flash clock divider setting, the actual hardware register value, see reference manual for details
 */
#define CONFIG_CLOCK_K60_FLASH_DIV 0x03

/**
 * FLL parameter DRST DRS in MCG register C4, see reference manual for details
 */
#define CONFIG_CLOCK_K60_FLL_MCG_C4_DRST_DRS 0b11

/**
 * FLL parameter DMX32 in MCG register C4, see reference manual for details
 */
#define CONFIG_CLOCK_K60_FLL_MCG_C4_DMX32 1

/**
 * CPU core frequency resulting from the chosen divisors and multipliers.
 */
#define F_CPU DEFAULT_SYSTEM_CLOCK

/**
 * System frequency resulting from the chosen divisors and multipliers.
 */
#define F_SYS (F_CPU / (CONFIG_CLOCK_K60_SYS_DIV + 1))

/**
 * Bus frequency resulting from the chosen divisors and multipliers.
 */
#define F_BUS (F_CPU / (CONFIG_CLOCK_K60_BUS_DIV + 1))

/**
 * FlexBus frequency resulting from the chosen divisors and multipliers.
 */
#define F_FLEXBUS (F_CPU / (CONFIG_CLOCK_K60_FB_DIV + 1))

/**
 * Flash frequency resulting from the chosen divisors and multipliers.
 */
#define F_FLASH (F_CPU / (CONFIG_CLOCK_K60_FLASH_DIV + 1))

/** @} */

#ifdef __cplusplus
}
#endif

#endif
