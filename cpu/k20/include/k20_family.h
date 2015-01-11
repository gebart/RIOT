/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_k20_definitions
 * @{
 *
 * @file
 * @brief           K20 memory and clock configuration
 *
 * This file is primarily for the cpp preprocessor that generates the linker
 * script, thus it must not contain any C code. It is also used to determine
 * the CMSIS header to be included and to find out the maximum core clock speed.
 *
 * @author          Finn Wilke <finn.wilke@fu-berlin.de>
 */


/* Calculate the values, see documentation below */
#if defined(CPU_MODEL_MK20DN32VLF5) || defined(CPU_MODEL_MK20DX32VLF5) || defined(CPU_MODEL_MK20DN32VLF5) || defined(CPU_MODEL_MK20DX32VFT5) || defined(CPU_MODEL_MK20DN64VLF5) || defined(CPU_MODEL_MK20DX64VLF5) || defined(CPU_MODEL_MK20DN64VFT5) || defined(CPU_MODEL_MK20DX64VFT5) || defined(CPU_MODEL_MK20DN128VLF5) || defined(CPU_MODEL_MK20DX128VLF5) || defined(CPU_MODEL_MK20DN128VFT5) || defined(CPU_MODEL_MK20DX128VFT5)

#define __K20_FAMILY_CPU_FAMILY MK20D5
#define __K20_FAMILY_CPU_MAX_CORE_CLOCK_SPEED 50000000

#define __K20_FAMILY_FLASH_CONFIGURATION_FIELD_OFFSET 0x400
#define __K20_FAMILY_FLASH_CONFIGURATION_FIELD_SIZE 0x10

#define __K20_ISR_VECTOR_COUNT 61

#else
#error "Unsupported Freescale K20 CPU."
#endif


#if   defined(CPU_MODEL_MK20DN32VLF5) || defined(CPU_MODEL_MK20DX32VLF5) || defined(CPU_MODEL_MK20DN32VLF5) || defined(CPU_MODEL_MK20DX32VFT5)
#define __K20_FAMILY_CPU_ROM_SIZE 0x8000
#define __K20_FAMILY_CPU_SRAM_SIZE 0x2000

#elif defined(CPU_MODEL_MK20DN64VLF5) || defined(CPU_MODEL_MK20DX64VLF5) || defined(CPU_MODEL_MK20DN64VFT5) || defined(CPU_MODEL_MK20DX64VFT5)
#define __K20_FAMILY_CPU_ROM_SIZE 0x10000
#define __K20_FAMILY_CPU_SRAM_SIZE 0x4000

#elif defined(CPU_MODEL_MK20DN128VLF5) || defined(CPU_MODEL_MK20DX128VLF5) || defined(CPU_MODEL_MK20DN128VFT5) || defined(CPU_MODEL_MK20DX128VFT5)
#define __K20_FAMILY_CPU_ROM_SIZE 0x20000
#define __K20_FAMILY_CPU_SRAM_SIZE 0x4000
#endif

#if   defined(CPU_MODEL_MK20DN32VLF5) || defined(CPU_MODEL_MK20DN32VLF5) || defined(CPU_MODEL_MK20DN64VLF5) || defined(CPU_MODEL_MK20DN64VFT5) || defined(CPU_MODEL_MK20DN128VLF5) || defined(CPU_MODEL_MK20DN128VFT5)
#define __K20_FAMILY_CPU_FLEXNVM_SIZE 0
#define __K20_FAMILY_CPU_FLEXRAM_SIZE 0

#elif defined(CPU_MODEL_MK20DX32VLF5) || defined(CPU_MODEL_MK20DX32VFT5) || defined(CPU_MODEL_MK20DX64VLF5) || defined(CPU_MODEL_MK20DX64VFT5) || defined(CPU_MODEL_MK20DX128VLF5) || defined(CPU_MODEL_MK20DX128VFT5)
#define __K20_FAMILY_CPU_FLEXNVM_SIZE 0x8000
#define __K20_FAMILY_CPU_FLEXRAM_SIZE 0x800
#endif


/* Documentation start */

/** CPU Family */
#define CPU_FAMILY __K20_FAMILY_CPU_FAMILY

/** Maximum core clock allowed on this architecture */
#define CPU_MAX_CORE_CLOCK_SPEED __K20_FAMILY_CPU_MAX_CORE_CLOCK_SPEED

/** Start address of the CPU flash @{ */
#define CPU_ROM_START 0x0

/** Size of the CPU flash */
#define CPU_ROM_SIZE __K20_FAMILY_CPU_ROM_SIZE

/** Start of the CPU flash configuration section */
#define CPU_FLASH_CONFIGURATION_FIELD_OFFSET __K20_FAMILY_FLASH_CONFIGURATION_FIELD_OFFSET

/** Size of the CPU flash configuration section */
#define CPU_FLASH_CONFIGURATION_FIELD_SIZE __K20_FAMILY_FLASH_CONFIGURATION_FIELD_SIZE

/** Start address of the SRAM */
#define CPU_SRAM_START 0x20000000 - (__K20_FAMILY_CPU_SRAM_SIZE / 2)

/** Size of the CPU SRAM */
#define CPU_SRAM_SIZE __K20_FAMILY_CPU_ROM_SIZE

/** Start address of the FlexNVM */
#define CPU_FLEXNVM_START 0x10000000

/** Size of the FlexNVM */
#define CPU_FLEXNVM_SIZE __K20_FAMILY_CPU_FLEXNVM_SIZE

/** Start address of the FlexRAM */
#define CPU_FLEXRAM_START 0x14000000

/** Size of the FlexRAM */
#define CPU_FLEXRAM_SIZE __K20_FAMILY_CPU_FLEXRAM_SIZE

#ifdef CPU_BOOTLOADER_OFFSET
#define CPU_APPLICATION_ROM_START (CPU_ROM_START + CPU_BOOTLOADER_OFFSET)
#define CPU_APPLICATION_ROM_SIZE CPU_ROM_SIZE - CPU_BOOTLOADER_OFFSET
#else
/** RIOT entry point and location of interrupt vectors */
#define CPU_APPLICATION_ROM_START CPU_ROM_START

/** Size of the flash without the bootloader size */
#define CPU_APPLICATION_ROM_SIZE CPU_ROM_SIZE
#endif

/* We need to include the flash configuration field if, and only if, the start
 * offset is in front of it */
#if (CPU_APPLICATION_ROM_START < CPU_FLASH_CONFIGURATION_FIELD_OFFSET + CPU_FLASH_CONFIGURATION_FIELD_SIZE)
#define CPU_INCLUDE_FLASH_CONFIG
#endif

/** @} */
