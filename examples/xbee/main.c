/*
 * Copyright (C) 2014 Freie Universität Berlin
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
 * @brief       Hello World application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "periph/uart.h"
#include "hwtimer.h"
#include "xbee.h"


void rx(void *arg, char c)
{
    (void)arg;
    if (c == '\r') {
        c = '\n';
    }

    printf("%c", c);
}


int main(void)
{
    char in;

    uart_init(UART_1, 9600, rx, NULL, NULL);

    while (1) {
        in = getchar();
        if (in == '+') {
            puts("Entering Command Mode...");
            uart_write_blocking(UART_1, '+');
            uart_write_blocking(UART_1, '+');
            uart_write_blocking(UART_1, '+');
            hwtimer_wait(2000 * 1000);
        }
        else if (in == '\n') {
            uart_write_blocking(UART_1, '\r');
        }
        else {
            uart_write_blocking(UART_1, in);
        }
    }

    return 0;
}
