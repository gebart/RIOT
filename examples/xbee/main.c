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

#define DATA_TYPE_DIS   (2U)
#define DATA_TYPE_HUM   (1U)

#define NODE_ADDR       (50U)
#define NODE_CHAN       (26U)
#define NODE_PAN        (12U)

#define ACT_ADDR        (70U)


extern void _send_bar(xbee_t *dev, uint16_t addr, const char *data);
extern void _send_blubb(xbee_t *dev, uint16_t addr, uint8_t *data, size_t len);


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

        if (size == 4 && (data[1] == DATA_TYPE_HUM || DATA_TYPE_DIS)) {
            _send_blubb(&xbee, ACT_ADDR, data, 4);
        }

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

static void hum(int argc, char **argv)
{
    uint16_t val;
    uint8_t buf[4];

    if (argc != 2) {
        puts("that is just wrong");
        return;
    }
    val = (uint16_t)atoi(argv[1]);
    buf[0] = NODE_ADDR;
    buf[1] = DATA_TYPE_HUM;
    buf[2] = val >> 8;
    buf[3] = val & 0xff;
    _send_blubb(&xbee, ACT_ADDR, buf, 4);
}

static void col(int argc, char **argv)
{
    uint16_t val;
    uint8_t buf[4];

    if (argc != 3) {
        puts("that is just wrong");
        return;
    }
    val = (uint16_t)atoi(argv[2]);
    buf[0] = (uint8_t)atoi(argv[1]);
    buf[1] = DATA_TYPE_DIS;
    buf[2] = val >> 8;
    buf[3] = val & 0xff;
    _send_blubb(&xbee, ACT_ADDR, buf, 4);
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
    { "hum", "send hum data", hum },
    { "col", "send col data", col },
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

    uint16_t blubb = NODE_ADDR;
    xbee.driver->set((ng_netdev_t *)&xbee, NETCONF_OPT_ADDRESS, &blubb, 2);
    blubb = NODE_CHAN;
    xbee.driver->set((ng_netdev_t *)&xbee, NETCONF_OPT_CHANNEL, &blubb, 2);
    blubb = NODE_PAN;
    xbee.driver->set((ng_netdev_t *)&xbee, NETCONF_OPT_NID, &blubb, 2);

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
