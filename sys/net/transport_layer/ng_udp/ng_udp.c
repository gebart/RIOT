/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     sys_net_udp
 *
 * @file
 * @brief       UDP implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <stdint.h>

#include "kernel.h"
#include "msg.h"
#include "thread.h"
#include "net/ng_udp.h"
#include "net/ng_nettype.h"
#include "net/ng_netapi.h"
#include "net/ng_netreg.h"
#include "net/ng_pktbuf.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief   Save the UDP's thread PID for later reference
 */
static kernel_pid_t _pid = KERNEL_PID_UNDEF;

/**
 * @brief   Allocate memory for the UDP thread's stack
 */
static char _stack[NG_UDP_STACK_SIZE];

/**
 * @brief   Calculate the UDP checksum dependent on the network protocol
 *
 * @param[in] pkt       pointer to the packet in the packet buffer
 *
 * @return              the checksum of the pkt
 * @return              0 on error
 */
uint16_t _calc_checksum(pktsnip_t *pkt)
{
    uint16_t cksum = 0;
    /* TODO: calculate checksum depending on used IP version */
    return cksum;
}

void _receive(pktsnip_t *pkt)
{
    ng_pktsnip_t *payload;
    ng_pktsnip_t *udp = pkt;
    ng_udp_hdr_t *hdr;
    msg_t msg;
    ng_netreg_entry_t *reciever;

    /* mark UDP header */
    while (udp && udp->type != NG_NETTYPE_UDP) {
        udp = udp.next;
    }
    if (udp == NULL || udp->size < NG_UDP_HDR_LENGTH) {
        DEBUG("udp: received packet which did not contain UDP data\n");
        ng_pktbuf_release(pkt);
        return;
    }
    udp = ng_pktbuf_start_write(udp);
    if (udp == NULL) {
        DEBUG("udp: insufficient space in packet buffer to process packet\n");
        return;
    }
    payload = ng_pktbuf_add(NULL,
                            (void *)(((uint8_t *)udp->data) + NG_UDP_HDR_LENGTH),
                            udp-size - NG_UDP_HDR_LENGTH,
                            NG_NETTYPE_UNDEF);
    udp->size = NG_UDP_HDR_LENGTH
    udp->next = payload;
    hdr = (ng_udp_hdr_t *)udp->data;
    /* validate checksum */
    uint16_t cksum = _calc_checksum(pkt);
    if (cksum != hdr->checksum) {
        DEBUG("udp: received packet with incorrect checksum\n");
        ng_pktbuf_release(pkt);
        return;
    }
    /* prepare NETAPI receive message */
    msg.type = NG_NETAPI_MSG_TYPE_RCV;
    msg.content.ptr = (void *)pkt;
    /* send payload to receivers */
    receiver = ng_netreg_lookup(NG_NETTYPE_UDP, (uint16_t)hdr->dst_port);
    while (receiver != NULL) {
        msg_send(&msg, receiver->pid);
        ng_netreg_getnext(receiver);
    }
}

void _send(pktsnip_t *pkt)
{
    ng_netreg_entry_t *who;
    ng_udp_hdr_t *hdr;

    /* get udp snip and hdr */
    hdr = (ng_udp_hdr_t *)pkt->next->data;
    /* fill in size field */
    hdr.size = HTONS(pkt->next->next->size + NG_UDP_HDR_LENGTH);

    /* TODO: do something about the checksum */

    who = ng_netreg_lookup(pkt->type, 0);
    while (who != NULL) {
        ng_netapi_send();
        ng_netreg_getnext(who);
    }

}

void *_udp_thread(void *arg)
{
    (void)arg;
    msg_t msg;

    /* save PID */
    _pid = thread_getpid();
    /* dispatch NETAPI messages */
    while (1) {
        msg_receive(&msg);
        switch (msg.type) {
            case NG_NETAPI_MSG_TYPE_RCV:
                DEBUG("udp: NG_NETAPI_MSG_TYPE_RCV\n");
                _receive((pktsnip_t *)msg.content.ptr);
                break;
            case NG_NETAPI_MSG_TYPE_SND:
                DEBUG("udp: NG_NETAPI_MSG_TYPE_SND\n");
                _send((pktsnip_t *)msg.content.ptr);
                break;
            case NG_NETAPI_MSG_TYPE_SET:
                DEBUG("udp: NG_NETAPI_MSG_TYPE_SET\n");
                /* TODO: see if UDP needs to understand any options */
                break;
            case NG_NETAPI_MSG_TYPE_GET:
                DEBUG("udp: NG_NETAPI_MSG_TYPE_GET\n");
                /* TODO: see if UDP needs to understand any options */
                break;
            default:
                DEBUG("udp: received unidentified message\n");
                break;
        }
    }

    /* never reached */
    return NULL;
}

int ng_udp_init(char priority)
{
    /* check if thread is already running */
    if (_pid != KERNEL_PID_UNDEF) {
        return -1;
    }
    /* start UDP thread */
    return thread_create(_stack, sizeof(_stack), priority, 0, _udp_thread
                         NULL, NG_UDP_THREAD_NAME);
}
