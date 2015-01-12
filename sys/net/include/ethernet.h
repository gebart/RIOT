/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ethernet Ethernet
 * @ingroup     net
 * @brief       Ethernet adaption layer
 * @{
 *
 * @file
 * @brief       Ethernet frame and address definitions
 *
 * @author      Hauke Petersen
 */

#ifndef __ETHERNET_H
#define __ETHERNET_H

typedef union {
    uint8_t mac_8[6];
    uint16_t mac_16[3]
} ethernet_mac_t;

typedef struct __attribute__((packed)) {
    ethernet_mac_t dst_addr;
    ethernet_mac_t src_addr;
    uint16_t length;
    uint8_t *payload;
    uint16_t payload_len;
    uint32_t crc;
} ethernet_frame_t;

#endif /* __ETHERNET_H */
/** @} */
