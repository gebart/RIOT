/*
 * Copyright (C) 2008, 2009, 2010  Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2013 INRIA
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Default application that shows a lot of functionality of RIOT
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "posix_io.h"
#include "shell.h"
#include "shell_commands.h"
#include "board_uart0.h"

#define SND_BUFFER_SIZE     (100)
#define RCV_BUFFER_SIZE     (64)


/* TODO */
#include "nrf51prop.h"
#include "nomac.h"

nrf51prop_t radio;
static char nomac_stack[NOMAC_DEFAULT_STACKSIZE];


static int shell_readc(void)
{
    char c = 0;
    (void) posix_read(uart0_handler_pid, &c, 1);
    return c;
}

static void shell_putchar(int c)
{
    (void) putchar(c);
}

int main(void)
{
    shell_t shell;
    (void) posix_open(uart0_handler_pid, 0);

    /* TODO */
    nrf51prop_init(&radio);
    nomac_init(nomac_stack, sizeof(nomac_stack), PRIORITY_MAIN - 4, "nomac", (netdev_t *)&radio);

    (void) puts("Welcome to RIOT!");

    shell_init(&shell, NULL, UART0_BUFSIZE, shell_readc, shell_putchar);
    shell_run(&shell);
    return 0;
}
