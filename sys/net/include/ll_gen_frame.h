/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ll_gen_frame Generic link layer header
 * @ingroup     net
 * @brief
 * @{
 *
 * @file
 * @brief
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef LL_GEN_FRAME_H_
#define LL_GEN_FRAME_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Generic link layer frame format
 */
typedef struct {
    uint8_t addr_len;
    uint8_t rssi;
    uint8_t lqi;
    uint8_t options;            /**< not used so far... */
} ll_gen_frame_t;

inline void ll_gen_get_src_addr(ll_gen_frame_t *frame, uint8_t *addr, uint8_t addr_len)
{
    memcpy(addr, (frame + sizeof(ll_gen_frame_t)), addr_len);
}

inline void ll_gen_set_src_addr(ll_gen_frame_t *frame, uint8_t *addr, uint8_t addr_len)
{
    memcpy((frame + sizeof(ll_gen_frame_t)), addr, addr_len);
}

inline void ll_gen_get_dst_addr(ll_gen_frame_t *frame, uint8_t *addr, uint8_t addr_len)
{
    memcpy(addr, (frame + sizeof(ll_gen_frame_t) + addr_len), addr_len);
}

inline void ll_gen_set_dst_addr(ll_gen_frame_t *frame, uint8_t *addr, uint8_t addr_len)
{
    memcpy((frame + sizeof(ll_gen_frame_t) + addr_len), addr, addr_len);
}

#ifdef __cplusplus
}
#endif

#endif /* LL_GEN_FRAME_H_ */
/** @} */
