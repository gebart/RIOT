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
#include "net/ng_netdev.h"
#include "net/ng_ifhdr.h"
#include "shell.h"
#include "thread.h"
#include "kernel.h"


#define SHELL_BUFSIZE   (64U)


extern void _send_bar(xbee_t *dev, uint16_t addr, const char *data);


static xbee_t xbee;

static char data_mem[100];
static char hdr_mem[30];

static char stack[KERNEL_CONF_STACKSIZE_MAIN];

static ng_pktsnip_t hdr;
static ng_pktsnip_t data;


static void *mac(void *arg)
{
    msg_t msg;

    printf("mac is up \n");
    xbee.mac_pid = thread_getpid();

    while (1) {
        msg_receive(&msg);
        if (msg.type == NG_NETDEV_MSG_TYPE_EVENT) {
            xbee.driver->isr_event((ng_netdev_t *)&xbee, (uint16_t)msg.content.value);
        }
    }

    return NULL;
}

void ondata(ng_netdev_event_t type, void *arg)
{
    if (type == NETDEV_EVENT_RX_COMPLETE) {
        puts("APP: got some data!!!!");
        ng_pktsnip_t *d = (ng_pktsnip_t *)arg;
        // ng_ifhdr_t *hdr = (ng_ifhdr_t *)d->data;
        uint8_t *data = (uint8_t *)d->next->data;
        int size = (int)d->next->size;
        printf("DATA: (%i byte)\n", size);
        for (int i = 0; i < size; i++) {
            printf("%c", data[i]);
        }
        puts("");
    }
    else {
        puts("APP: unknown event triggered");
    }
}


static void addr(int argc, char **argv)
{
    uint16_t addr;
    size_t len = 2;
    int res;

    if (argc == 1) {
        xbee.driver->get((ng_netdev_t *)&xbee, NETCONF_OPT_ADDRESS, &addr, &len);
        printf("Address: %u\n", addr);
        return;
    }

    addr = (uint16_t)atoi(argv[1]);
    res = xbee.driver->set((ng_netdev_t *)&xbee, NETCONF_OPT_ADDRESS, &addr, 2);
    if (res != 0) {
        puts("Error setting address");
        return;
    }
    printf("Set address to %u\n", addr);
}

static void chan(int argc, char **argv)
{
    uint16_t chan;
    size_t len = 2;
    int res;

    if (argc == 1) {
        xbee.driver->get((ng_netdev_t *)&xbee, NETCONF_OPT_CHANNEL, &chan, &len);
        printf("Channel: %u\n", chan);
        return;
    }

    chan = (uint16_t)atoi(argv[1]);
    res = xbee.driver->set((ng_netdev_t *)&xbee, NETCONF_OPT_CHANNEL, &chan, 2);
    if (res != 0) {
        puts("Error setting channel");
        return;
    }
    printf("Set channel to %u\n", chan);
}

static void pan(int argc, char **argv)
{
    uint16_t pan;
    size_t len = 2;
    int res;

    if (argc == 1) {
        xbee.driver->get((ng_netdev_t *)&xbee, NETCONF_OPT_NID, &pan, &len);
        printf("PAN ID: %u\n", pan);
        return;
    }

    pan = (uint16_t)atoi(argv[1]);
    res = xbee.driver->set((ng_netdev_t *)&xbee, NETCONF_OPT_NID, &pan, 2);
    if (res != 0) {
        puts("Error setting PAN ID");
        return;
    }
    printf("Set PAN ID to %u\n", pan);
}

static void send(int argc, char **argv)
{
    uint16_t addr;

    if (argc != 3) {
        puts("fuck you");
        return;
    }
    addr = (uint16_t)atoi(argv[1]);

    _send_bar(&xbee, addr, argv[2]);
}


static int shell_readc(void)
{
    return (int)getchar();
}

static void shell_putchar(int c)
{
    putchar(c);
}

static const shell_command_t shell_commands[] = {
    { "addr", "set address", addr },
    { "chan", "set channel", chan },
    { "pan", "set pan id", pan },
    { "send", "send data", send },
    { NULL, NULL, NULL }
};

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

    xbee.driver->add_event_callback((ng_netdev_t *)&xbee, ondata);

    thread_create(stack, sizeof(stack), PRIORITY_MAIN - 1, 0, mac, NULL, "MAC");


    shell_t shell;
    shell_init(&shell, shell_commands, SHELL_BUFSIZE, shell_readc,
               shell_putchar);
    shell_run(&shell);
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
