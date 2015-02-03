/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_pktdump
 * @{
 *
 * @file
 * @brief       Generic module to dump packages to STDIO received via netapi
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "msg.h"
#include "pktbuf.h"
#include "pktdump.h"
#include "netapi.h"


void *_eventloop(void *arg);
void _dump(pktsnip_t *pkt);

kernel_pid_t pktdump_init(char *stack, int stacksize, char priority, char *name)
{
    return thread_create(stack, stacksize, priority, 0, _eventloop, NULL, name);
}

void _dump(pktsnip_t *pkt)
{
    int snips = 0;
    int size = 0;

    while (pkt != NULL) {
        printf("~~ SNIP %2i - type: %02x, size: %3i byte\n", snips, pkt->type, pkt->size);
        ++snips;
        size += pkt->size;
        pkt = pkt->next;
    }
    printf("~~ PKT    - %2i snips, total size: %3i byte\n", snips, size);
    pktbuf_release(pkt);
}

void *_eventloop(void *arg)
{
    (void)arg;
    msg_t msg;

    while (1) {
        msg_receive(&msg);

        switch (msg.type) {
            case NETAPI_MSG_TYPE_RCV:
                puts("PKTDUMP: data received:\n");
                _dump((pktsnip_t *)msg.content.ptr);
                break;
            case NETAPI_MSG_TYPE_SND:
                puts("PKTDUMP: data to send:\n");
                _dump((pktsnip_t *)msg.content.ptr);
                break;
            default:
                puts("PKTDUMP: received something unexpected\n");
                break;
        }
    }

    /* never reached */
    return NULL;
}
