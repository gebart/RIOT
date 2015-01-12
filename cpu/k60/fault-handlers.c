/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <stdint.h>
#include <stdio.h>

/**
 * @ingroup         cpu_k60
 * @{
 *
 * @file
 * @brief           Implementation of debugging fault handlers for ARM Cortex M3/M4 CPUs.
 *
 * @author          Joakim Gebart <joakim.gebart@eistec.se>
 */

/**
 * Hard fault handler (C part)
 *
 * This is basically a copy and paste of the hardfault handler originally
 * written by Joseph Yiu and published on various forums around the web.
 *
 * Author: Joseph Yiu
 *
 * \see Cortex-M4 Devices Generic User Guide, ARM, 2010
 * \see The Definitive Guide to ARM® Cortex®-M3 and Cortex®-M4 Processors, Third Edition, ISBN-978-0124080829
 *
 * \param hardfault_args A pointer to the stack area where the hardfault parameters are located.
 *
 * \note This function should never return.
 */
void hard_fault_handler_c(uint32_t * hardfault_args)
{
  uint32_t stacked_r0;
  uint32_t stacked_r1;
  uint32_t stacked_r2;
  uint32_t stacked_r3;
  uint32_t stacked_r12;
  uint32_t stacked_lr;
  uint32_t stacked_pc;
  uint32_t stacked_psr;
  uint32_t rBFAR;
  uint32_t rCFSR;
  uint32_t rHFSR;
  uint32_t rDFSR;
  uint32_t rAFSR;
  uint32_t rMMAR;

  stacked_r0 = hardfault_args[0];
  stacked_r1 = hardfault_args[1];
  stacked_r2 = hardfault_args[2];
  stacked_r3 = hardfault_args[3];

  stacked_r12 = hardfault_args[4];
  stacked_lr = hardfault_args[5];
  stacked_pc = hardfault_args[6];
  stacked_psr = hardfault_args[7];

  rBFAR = (*((volatile uint32_t *)(0xE000ED38)));
  rCFSR = (*((volatile uint32_t *)(0xE000ED28)));
  rHFSR = (*((volatile uint32_t *)(0xE000ED2C)));
  rDFSR = (*((volatile uint32_t *)(0xE000ED30)));
  rAFSR = (*((volatile uint32_t *)(0xE000ED3C)));
  rMMAR = (*((volatile uint32_t *)(0xE000ED34)));

  printf("[Hard fault handler]\n");
  printf("R0 = %lx\n", (unsigned long)stacked_r0);
  printf("R1 = %lx\n", (unsigned long)stacked_r1);
  printf("R2 = %lx\n", (unsigned long)stacked_r2);
  printf("R3 = %lx\n", (unsigned long)stacked_r3);
  printf("R12 = %lx\n", (unsigned long)stacked_r12);
  printf("LR = %lx\n", (unsigned long)stacked_lr);
  printf("PC = %lx\n", (unsigned long)stacked_pc);
  printf("PSR = %lx\n", (unsigned long)stacked_psr);
  printf("BFAR = %lx\n", (unsigned long)rBFAR);
  printf("CFSR = %lx\n", (unsigned long)rCFSR);
  printf("HFSR = %lx\n", (unsigned long)rHFSR);
  printf("DFSR = %lx\n", (unsigned long)rDFSR);
  printf("AFSR = %lx\n", (unsigned long)rAFSR);
  printf("MMAR = %lx\n", (unsigned long)rMMAR);

  /* Trigger a debugger to break here */
  __asm volatile ("bkpt #0\n");
  while(1);
}
/** @} */
