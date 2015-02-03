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
#include "kernel.h"
#include "shell_commands.h"
#include "board_uart0.h"
#include "kernel_types.h"

#define SND_BUFFER_SIZE     (100)
#define RCV_BUFFER_SIZE     (64)


/* TODO */
#include "nrf51prop.h"
#include "nomac.h"
#include "ll_gen_frame.h"
#include "pktbuf.h"
#include "pktdump.h"

extern kernel_pid_t who_to_hack;

static char nomac_stack[NOMAC_DEFAULT_STACKSIZE];
static char echo_stack[KERNEL_CONF_STACKSIZE_DEFAULT];
static kernel_pid_t radio_pid, echo_pid;
static nrf51prop_t radio;


static int shell_readc(void)
{
    char c = 0;
    (void) posix_read(uart0_handler_pid, &c, 1);
    return c;
}

static void txtsnd(int argc, char **argv)
{
    uint16_t addr;
    pktsnip_t *llhead, *payload;
    ll_gen_frame_t *frame;
    msg_t msg;

    if (argc < 3) {
        printf("usage: %s ADRESS DATA\n", argv[0]);
        return;
    }

    /* parse address */
    addr = (uint16_t)atoi(argv[1]);

    /* allocate packet */
    llhead = pktbuf_alloc(sizeof(ll_gen_frame_t) + 4);
    payload = pktbuf_alloc(strlen(argv[2]));
    /* set dst address */
    llhead->type = PKT_PROTO_LL_GEN;
    frame = (ll_gen_frame_t *)llhead->data;
    memset(frame, 0, sizeof(ll_gen_frame_t));
    frame->addr_len = 2;
    ll_gen_set_dst_addr(frame, (uint8_t *)&addr, 2);
    /* set and link payload */
    memcpy(payload->data, argv[2], payload->size);
    payload->type = PKT_PROTO_PAYLOAD;
    llhead->next = payload;

    /* send out data via netapi */
    msg.type = NETAPI_MSG_TYPE_SND;
    msg.content.ptr = (void *)llhead;
    printf("txtsnd: holding packet\n");
    //pktbuf_hold(llhead);
    puts("txtsnd: sending msg to echo_thread");
    msg_send(&msg, echo_pid);
    puts("txtsnd: sending msg to radio_thread");
    msg_send(&msg, radio_pid);
    puts("txtsnd: all ok");
}

static void addr(int argc, char **argv)
{
    uint16_t addr;
    int res;

    if (argc < 2) {
        printf("usage: %s ADDRESS\n", argv[0]);
        return;
    }

    addr = (uint16_t)atoi(argv[1]);
    printf("setting to address %i\n", (int)addr);

    res = netapi_set_option(radio_pid, NETCONF_OPT_ADDRESS, &addr, 2);
    if (res >= 0) {
        printf("set actual address to %i\n", res);
    }
    else {
        printf("setting address failed [%i]\n", res);
    }

}

static void trigger(int argc, char **argv)
{
    int state, res;

    if (argc < 2) {
        printf("usage: %s ACTION\n", argv[0]);
        puts("      0: switch to [off]");
        puts("      1: on");
        puts("      2: sleep");
        puts("      3: idle");
        puts("      4: rx");
        puts("      5: tx");
        return;
    }

    state = atoi(argv[1]);
    res = netapi_set_option(radio_pid, NETCONF_OPT_STATE, &state, 4);
    if (res >= 0) {
        printf("setting state was successful %i\n", res);
    }
    else {
        printf("setting state failed [%i]\n", res);
    }
}

static void shell_putchar(int c)
{
    (void) putchar(c);
}

static const shell_command_t shell_commands[] = {
    { "txtsnd", "send link layer packet", txtsnd },
    { "addr", "set link layer address", addr },
    { "trigger", "trigger link layer action", trigger },
    { NULL, NULL, NULL }
};

int main(void)
{
    shell_t shell;
    (void) posix_open(uart0_handler_pid, 0);

    printf("sizeof packet_t: %i\n", sizeof(nrf51prop_packet_t));
    printf("sizeof prop_t: %i\n", sizeof(nrf51prop_t));

    printf("addr of prop_t: %p\n", &radio);
    printf("addr of driver: %p\n", &(radio.driver));
    printf("addr of event : %p\n", &(radio.event_cb));
    printf("addr of into  : %p\n", radio.driver);
    radio.driver = (netdev_driver_t *)(0xfafafafa);
    printf("addr of into2 : %p\n", radio.driver);

    /* TODO */
    puts("Initializing radio");
    nrf51prop_init(&radio);
    puts("Initializing nomac");
    radio_pid = nomac_init(nomac_stack, sizeof(nomac_stack), PRIORITY_MAIN - 4, "nomac", (netdev_t *)&radio);
    puts("Initializing pktdump");
    echo_pid = pktdump_init(echo_stack, sizeof(echo_stack), PRIORITY_MAIN - 2, "echo");
    who_to_hack = echo_pid;

    (void) puts("Welcome to RIOT!");

    shell_init(&shell, shell_commands, UART0_BUFSIZE, shell_readc, shell_putchar);
    shell_run(&shell);
    return 0;
}
