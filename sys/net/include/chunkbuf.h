/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_chunkbuf Central Data Buffer
 * @ingroup     net
 * @brief       A global data buffer for the network stack
 * @{
 *
 * @file
 * @brief   Interface and data structures for the central data buffer
 *
 * @note    **WARNING!!** Do not store data structures that are not packed
 *          (defined with `__attribute__((packed))`) or enforce alignment in
 *          in any way in here. On some RISC architectures this *will* lead to
 *          alignment problems and can potentially result in segmentation/hard
 *          faults and other unexpected behaviour.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __CHUNKBUF_H
#define __CHUNKBUF_H

#include <stdint.h>


#ifndef CHUNKBUF_SIZE
/**
 * @brief   Maximum size of the chunk buffer.
 *
 * @details The rational here is to have at least space for 4 full-MTU IPv6
 *          packages (2 incoming, 2 outgoing; 2 * 2 * 1280 B = 5 KiB) +
 *          Meta-Data (roughly estimated to 1 KiB; might be smaller)
 */
#define CHUNKBUF_SIZE  (6144)
#endif  /* CHUNKBUF_SIZE */


typedef struct __attribute__((packed)) {
    chunk_t *next;
    char *data;
    uint16_t size;
    uint16_t type;
} chunk_t;

chunk_t *chunkbuf_alloc_chunk(size_t size);

int chunkbuf_alloc(chunk_t *chunk, size_t size);

int chunkbuf_realloc(chunk_t *chunk, size_t size);

int chunkbuf_free(chunk_t *chunk);




#endif /* __CHUNKBUF_H */
/** @} */
