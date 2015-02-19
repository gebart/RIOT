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
#include "periph/gpio.h"
#include "xbee.h"
#include "net/ng_pkt.h"

static xbee_t xbee;

static char data_mem[100];
static char hdr_mem[30];

static ng_pktsnip_t hdr;
static ng_pktsnip_t data;

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
    hdr.next = &data;
    hdr.data = &hdr_mem;
    hdr.size = sizeof(hdr_mem);
    hdr.type = NG_NETTYPE_UNDEF;

    data.next = NULL;
    data.data = &data_mem;
    data.size = sizeof(data_mem);
    data.type = NG_NETTYPE_UNDEF;

    xbee.rx_data = &hdr;

    xbee_init(&xbee, UART_1, 9600, GPIO_NUMOF, GPIO_NUMOF);


    // char in;

    // uart_init(UART_1, 9600, rx, NULL, NULL);

    // while (1) {
    //     in = getchar();
    //     if (in == '+') {
    //         puts("Entering Command Mode...");
    //         uart_write_blocking(UART_1, '+');
    //         uart_write_blocking(UART_1, '+');
    //         uart_write_blocking(UART_1, '+');
    //         hwtimer_wait(2000 * 1000);
    //     }
    //     else if (in == '\n') {
    //         uart_write_blocking(UART_1, '\r');
    //     }
    //     else {
    //         uart_write_blocking(UART_1, in);
    //     }
    // }

    return 0;
}
