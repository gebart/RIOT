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
 * @file        startup.c
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "cpu.h"
#include "board.h"

/**
 * @brief functions for initializing the board, std-lib and kernel
 */
extern void board_init(void);
extern void kernel_init(void);
extern void __libc_init_array(void);

/* The reset handler does not need any function prologue/epilogue since it is
 * only called after a reset */
void reset_handler(void) __attribute__((naked));

/* Start of .data section in RAM */
extern uint32_t _data_start[];
/* End of .data section in RAM */
extern uint32_t _data_end[];
/* Start of .data section in flash */
extern uint32_t _data_load[];

/*
 * Copy all initialized variables in .data from flash to RAM.
 * .data must be 4-byte aligned!
 */
static inline void
copy_initialized(void)
{
  uint32_t *ram = _data_start;
  uint32_t *rom = _data_load;
  while(ram < _data_end) {
    *(ram++) = *(rom++);
  }
}
/* Start of .bss section in RAM */
extern uint32_t __bss_start[];
/* End of .bss section in RAM */
extern uint32_t __bss_end[];

/*
 * Clear out .bss section.
 * .bss must be 4-byte aligned!
 */
static inline void
clear_bss(void)
{
  uint32_t *p = __bss_start;
  while(p < __bss_end) {
    *p = 0x0ul;
    ++p;
  }
}
/* Start of .ramcode section in RAM */
extern uint32_t _ramcode_start[];
/* End of .ramcode section in RAM */
extern uint32_t _ramcode_end[];
/* Start of .ramcode section in flash */
extern uint32_t _ramcode_load[];
/*
 * Copy the ramcode section to RAM.
 */
static inline void
copy_ramcode(void)
{
  uint32_t *ram = _ramcode_start;
  uint32_t *rom = _ramcode_load;
  while(ram < _ramcode_end) {
    *(ram++) = *(rom++);
  }
}

/* This variable is used to reference interrupt-vector-k60.c so that it will be
 * pulled in during linking. */
extern void *isr_vector[];

/* Start of .vector_table section in RAM */
extern void *_vector_ram_start[];
/* End of .vector_table section in RAM */
extern void *_vector_ram_end[];
/* Start of .vector_table section in flash */
extern void *_vector_rom[];
/*
 * Copy the interrupt vector table to RAM.
 */
static inline void
copy_isr_vector(void)
{
  void **ram = _vector_ram_start;
  void **rom = isr_vector;
  while(ram < _vector_ram_end) {
    *(ram++) = *(rom++);
  }
}


/* Initialize all data used by the C runtime. */
static inline void
init_data(void)
{
  copy_initialized();

  clear_bss();

  copy_ramcode();

  copy_isr_vector();
}

/* our local copy of newlib init */
void call_init_array(void);

/* Stack pointer will be set to _stack_start by the hardware at reset/power on */
void
reset_handler(void)
{
#if DISABLE_WDOG
  /* Disable watchdog to allow single stepping through the startup code. */
  /** \todo Only disable watchdog on debug builds. */
  /*
   * The following unlock sequence must be completed within 256 bus cycles or
   * the watchdog will reset the system. The watchdog is enabled by default at
   * power on.
   *
   * The sequence is:
   * 1. Write 0xC520 to the unlock register
   * 2. Write 0xD928 to the unlock register
   *
   * Watchdog is now unlocked to allow us to change its settings
   *
   * 3. Clear the WDOGEN bit of the WDOG_STCTRLH register to completely disable
   *    the watchdog.
   *
   * It is now possible to single step through the code without the watchdog
   * resetting the system.
   */
  WDOG->UNLOCK = 0xC520;
  WDOG->UNLOCK = 0xD928;
  WDOG->STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;

  /*
   * The line below this comment is the earliest possible location for a
   * breakpoint when debugging the startup code.
   */
#endif /* DISABLE_WDOG */

  /* Copy .data and clear .bss */
  init_data();

  /* initialize the CPU clocks and the board */
  board_init();

  /* initialize std-c library (this should be done after board_init) */
  __libc_init_array();

  /* startup the kernel */
  kernel_init();
#if DISABLE_WDOG
  puts("WARNING: Debug build, Hardware Watchdog disabled, do not use in production installations!\n");
#endif

  kernel_init();

  /* main should never return, but just in case... */
  while(1);
}
