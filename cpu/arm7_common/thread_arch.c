/*
 * Copyright (C) 2008, 2009  Heiko Will <hwill@inf.fu-berlin.de>
 * Copyright (C) 2009  Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_arm7_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernel's reboot interface
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Heiko Will <heiko.will@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "sched.h"
#include "arch/thread_arch.h"

#define STACK_MARKER    (0x77777777)
#define REGISTER_CNT    (12)
#define NEW_TASK_CPSR   (0x1F)

char *thread_arch_stack_init(thread_task_func_t task_func, void *arg,
                             void *stack_start, int stack_size)
{
    unsigned int *stk;
    int i;
    stk = (unsigned int *)((uintptr_t)stack_start + stack_size);
    stk--;

    *stk = STACK_MARKER;

    /* set the return address (LR) */
    stk--;
    *stk = (unsigned int) sched_task_exit;

    /* set the stack pointer (SP) */
    stk--;
    *stk = (unsigned int)((unsigned int)stack_start + stack_size) - 4;

    /* build base stack */
    for (i = REGISTER_CNT; i > 0 ; i--) {
        stk--;
        *stk = i;
    }

    /* set argument to task_func */
    stk--;
    *stk = ((unsigned int) arg);

    /* set the entry point */
    stk--;
    *stk = ((unsigned int) task_func);
    /* set the saved program status register */
    stk--;
    *stk = (unsigned int) NEW_TASK_CPSR;

    return (char *)stk;
}

void thread_arch_stack_print(void)
{
    register void *stack = 0;
    asm("mov %0, sp" : "=r"(stack));

    register unsigned int *s = (unsigned int *)stack;
    printf("task: %X SP: %X\n", (unsigned int) sched_active_thread, (unsigned int) stack);
    register int i = 0;
    s += 5;

    while (*s != STACK_MARKER) {
        printf("STACK (%d) addr=%X = %X \n", i, (unsigned int) s, (unsigned int) *s);
        s++;
        i++;
    }

    printf("STACK (%d)= %X \n", i, *s);
}


void thread_arch_start_threading(void) NORETURN;


void thread_arch_yield(void)
{
    asm("svc 0\n");
}
