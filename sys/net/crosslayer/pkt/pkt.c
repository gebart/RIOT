/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file    pkt.c
 */

#include "pkt.h"

int pktsnip_num(pktsnip_t *pkt)
{
    int c = 0;

    while (pkt) {
        c++;
        pktsnip_advance(&pkt);
    }

    return c;
}

pktsize_t pkt_len(pktsnip_t *pkt)
{
    pktsize_t c = 0;

    while (pkt) {
        c += pkt->size;
        pktsnip_advance(&pkt);
    }

    return c;
}

void pktsnip_add(pktsnip_t **pkt, pktsnip_t *snip)
{
    if (pkt == NULL || snip == NULL) {
        return;
    }

    if (*pkt == NULL) {
        *pkt = snip;
    }
    else {
        snip->next = (*pkt)->next;
        (*pkt)->next = snip;
    }
}

void pktsnip_remove(pktsnip_t **pkt, pktsnip_t *snip)
{
    if (pkt == NULL || *pkt == NULL || snip == NULL) {
        return;
    }

    if ((*pkt) == snip) {
        (*pkt) = (*pkt)->next;
        snip->next = NULL;
    }
    else {
        pktsnip_t *ptr = (*pkt)->next, *prev = *pkt;

        while (ptr != NULL) {
            if (ptr == snip) {
                prev->next = ptr->next;
                ptr->next = NULL;
                break;
            }

            pktsnip_advance(&ptr);
            pktsnip_advance(&prev);
        }
    }

}

/** @} */
