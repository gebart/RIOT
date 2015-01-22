/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkt Packet
 * @brief       Network packet abstraction types
 * @ingroup     net
 * @{
 *
 * @file    pkt.h
 * @brief   General definitions for network packets
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef __PKT_H_
#define __PKT_H_

#include <inttypes.h>

#include "clist.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Definition of protocol families to determine the type of the packet
 *          or which protocols a network device (see @ref netdev) or protocol
 *          layer (see @ref netapi) can handle
 *
 * @note    XXX: The concrete definition of the values is necessary to work
 *          with super-flexible devices as e.g. @ref native_net. It was also
 *          decided not to use ethertype since protocols not supplied by it
 *          might be supported
 */
typedef enum {
    PKT_PROTO_UNKNOWN       = 0x0000,   /**< Type was not specified */

    /**
     * @brief   Radio frame protocol
     *
     * @details Sends frames as defined by radio_packet_t.
     */
    PKT_PROTO_RADIO         = 0x0001,

    PKT_PROTO_ETHERNET      = 0x0002,   /**< Ethernet */
    PKT_PROTO_802154_BEACON = 0x0003,   /**< IEEE 802.15.4 beacon frame */
    PKT_PROTO_802154_DATA   = 0x0004,   /**< IEEE 802.15.4 data frame */
    PKT_PROTO_802154_ACK    = 0x0005,   /**< IEEE 802.15.4 acknowledgment frame */
    PKT_PROTO_802154_MACCMD = 0x0006,   /**< IEEE 802.15.4 MAC command frame */
    PKT_PROTO_BTLE          = 0x0007,   /**< Bluetooth Low-Energy */

    /**
     * @brief   CC110x frame format protocol
     *
     * @details Sends frames as defined by cc110x_packet_t.
     */
    PKT_PROTO_CC110X        = 0x0008,
    PKT_PROTO_6LOWPAN       = 0x0009,   /**< 6LoWPAN. */
    PKT_PROTO_IPV4          = 0x000a,   /**< IPv4. */
    PKT_PROTO_IPV6          = 0x000b,   /**< IPv6. */
    PKT_PROTO_UDP           = 0x000c,   /**< UDP. */
    PKT_PROTO_TCP           = 0x000d,   /**< TCP. */
    PKT_PROTO_CCNL          = 0x000e,   /**< CCN lite. */
} pkt_proto_t;

/**
 * @brief   Type to define payload size of a packet
 */
typedef uint16_t pktsize_t;

/**
 * @brief   Macro for pktsize_t printing formatter
 */
#define PRIpktsize  PRIu16

/**
 * @brief   Maximum value for packet size
 */
#define PKTSIZE_MAX (UINT16_MAX)

/**
 * @brief   Type to represent parts (snips) of a network packet
 * @detail  The idea behind thie packet parts is that they either can represent
 *          protocol-specific headers or payload. A packet can be comprised of
 *          *n* `pktsnip_t` elements, where the first element represents the
 *          header of the lowest available network layer and the *(n - 1)*st
 *          element represents the payload of the highest available layer.
 *
 *          Example:
 *
 *                                                                  buffer
 *              +---------------------------+                      +------+
 *              | size = 14                 | data +-------------->|      |
 *              | type = PKT_PROTO_ETHERNET |------+               +------+
 *              +---------------------------+         +----------->|      |
 *                    | next                          |            |      |
 *                    v                               |            |      |
 *              +---------------------------+         |            +------+
 *              | size = 40                 | data    |  +-------->|      |
 *              | type = PKT_PROTO_IPV6     |---------+  |         +------+
 *              +---------------------------+            |  +----->|      |
 *                    | next                             |  |      +------+
 *                    v                                  |  |  +-->|      |
 *              +---------------------------+            |  |  |   |      |
 *              | size = 8                  | data       |  |  |   .      .
 *              | type = PKT_PROTO_UDP      |------------+  |  |   .      .
 *              +---------------------------+               |  |
 *                    | next                                |  |
 *                    v                                     |  |
 *              +---------------------------+               |  |
 *              | size = 5                  | data          |  |
 *              | type = PKT_PROTO_COAP     |---------------+  |
 *              +---------------------------+                  |
 *                    | next                                   |
 *                    v                                        |
 *              +---------------------------+                  |
 *              | size = 54                 | data             |
 *              | type = PKT_PROTO_UNKNOWN  |------------------+
 *              +---------------------------+
 *
 *
 * @note    This type implements its own list implementation because of the way
 *          it is stored in the packet buffer.
 * @note    This type has no initializer on purpose. Please use @ref pktbuf
 *          as factory.
 */
typedef struct __attribute__((packed)) pktsnip_t {    /* packed to be aligned
                                                       * correctly in static
                                                       * packet buffer */
    struct pktsnip_t *next;         /**< next snip in the packet. */
    void *data;                     /**< pointer to the data of the snip */
    pktsize_t size;                 /**< the length of the snip in byte. */
    pkt_proto_t type;               /**< protocol of the packet part. */
} pktsnip_t;


/**
 * @brief Advances safely to the next packet part
 *
 * @param[in,out] snip  The packet part to work upon.
 *
 * @return  The next packet part after @p snip
 * @return  NULL if @p snip was the last packet part in the packet
 */
static inline pktsnip_t *pktsnip_advance(pktsnip_t **snip)
{
    if (snip != NULL && *snip != NULL) {
        *snip = (*snip)->next;
        return *snip;
    }

    return NULL;
}

/**
 * @brief Calculates number of packet parts in a packet.
 *
 * @param[in] pkt  list of packet parts comprising a packet.
 *
 * @return  number of packets parts in packet.
 */
int pktsnip_num(pktsnip_t *pkt);

/**
 * @brief Adds a new packet part to a packet.
 *
 * @param[in] pkt       The packet to add the packet part @p snip to
 * @param[in] snip      The packet part to add to @p pkt.
 */
void pktsnip_add(pktsnip_t **pkt, pktsnip_t *snip);

/**
 * @brief Removes a packet part from a packet.
 *
 * @param[in] pkt       The packet to remove the packet part @p snip from
 * @param[in] snip      The packet part to remove from @p pkt.
 */
void pktsnip_remove(pktsnip_t **pkt, pktsnip_t *snip);

/**
 * @brief Calculates length of a packet in byte.
 *
 * @param[in] pkt  list of packet parts.
 *
 * @return  length of the list of headers.
 */
pktsize_t pkt_len(pktsnip_t *pkt);

#ifdef __cplusplus
}
#endif

#endif /* __PKT_H_ */
/** @} */
