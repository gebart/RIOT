/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net
 * @{
 *
 * @file
 * @brief       Generic link layer header
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef LLHDR_H_
#define LLHDR_H_

#include <string.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Generic link layer header
 *
 * The actual addresses are put into the memory following this datastructure.
 */
typedef struct __attribute__((packed)) {
    uint8_t addr_len;           /**< link layer address length in byte */
    uint8_t rssi;               /**< rssi of received packet (optional) */
    uint8_t lqi;                /**< lqi of received packet (optional) */
    uint8_t options;            /**< any optional options */
} llhdr_t;

/**
 * @brief   Initialize the given generic link layer header
 *
 * @param[in] hdr           header to initialize
 * @param[in]  addr_len     link layer address length
 */
inline void llhdr_init(llhdr_t *hdr, uint8_t addr_len)
{
    hdr->addr_len = addr_len;
    hdr->rssi = 0;
    hdr->lqi = 0;
}

/**
 * @brief   Get the size of the given generic link layer header
 *
 * @param[in] hdr           header to get the size of
 *
 * @return                  the size of the given header
 */
inline size_t llhdr_sizeof(llhdr_t *hdr)
{
    return sizeof(llhdr_t) + hdr->addr_len + hdr->addr_len;
}

/**
 * @brief   Get the source address from the given header
 *
 * @param[in] hdr           header to read from
 * @param[in] addr          buffer to write to
 * @param[in] addr_len      maximum number of bytes to read
 */
inline void llhdr_get_src_addr(llhdr_t *hdr, uint8_t *addr, uint8_t addr_len)
{
    memcpy(addr, (hdr + sizeof(llhdr_t)), hdr->addr_len);
}

/**
 * @brief   Set the source address in the given header
 *
 * @param[in] hdr           header to write to
 * @param[in] addr          new source address
 * @param[in] addr_len      *addr* length
 */
inline void llhdr_set_src_addr(llhdr_t *hdr, uint8_t *addr, uint8_t addr_len)
{
    memcpy((hdr + sizeof(llhdr_t)), addr, addr_len);
}


/**
 * @brief   Get the destination address from the given header
 *
 * @param[in] hdr           header to read from
 * @param[in] addr          buffer to write to
 * @param[in] addr_len      maximum number of bytes to read
 */
inline void llhdr_get_dst_addr(llhdr_t *hdr, uint8_t *addr, uint8_t addr_len)
{
    memcpy(addr, (hdr + sizeof(llhdr_t) + hdr->addr_len), hdr->addr_len);
}

/**
 * @brief   Set the destination address in the given header
 *
 * @param[in] hdr           header to write to
 * @param[in] addr          new destination address
 * @param[in] addr_len      *addr* length
 */
inline void llhdr_set_dst_addr(llhdr_t *hdr, uint8_t *addr, uint8_t addr_len)
{
    memcpy((hdr + sizeof(llhdr_t) + hdr->addr_len), addr, addr_len);
}

#ifdef __cplusplus
}
#endif

#endif /* LL_GEN_FRAME_H_ */
/** @} */
