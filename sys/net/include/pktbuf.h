/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pktbuf   Packet buffer
 * @ingroup     net
 * @brief       A global network packet buffer.
 *
 * @note    **WARNING!!** Do not store data structures that are not packed
 *          (defined with `__attribute__((packed))`) or enforce alignment in
 *          in any way in here if @ref PKTBUF_SIZE > 0. On some RISC architectures
 *          this *will* lead to alignment problems and can potentially result
 *          in segmentation/hard faults and other unexpected behaviour.
 *
 * @{
 *
 * @file    pktbuf.h
 * @brief   Interface definition for the global network buffer. Network devices
 *          and layers can allocate space for packets here.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef PKTBUF_H_
#define PKTBUF_H_

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "atomic.h"
#include "cpu-conf.h"
#include "pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def     PKTBUF_SIZE
 * @brief   Maximum size of the static packet buffer.
 *
 * @details The rational here is to have at least space for 4 full-MTU IPv6
 *          packages (2 incoming, 2 outgoing; 2 * 2 * 1280 B = 5 KiB) +
 *          Meta-Data (roughly estimated to 1 KiB; might be smaller). If
 *          @ref PKTBUF_SIZE is 0 the packet buffer will use dynamic memory
 *          management to allocate packets.
 */
#ifndef PKTBUF_SIZE
#define PKTBUF_SIZE  (6144)
#endif  /* PKTBUF_SIZE */


/**
 * @var     pktbuf_max_bytes
 * @brief   All-over maximum number of bytes allocated in packet buffer during
 *          runtime
 *
 * @detail  This variable is only available if @ref DEVELHELP is active and
 *          @ref PKTBUF_SIZE > 0.
 */
#if defined(DEVELHELP) && (PKTBUF_SIZE > 0)
extern unsigned int pktbuf_max_bytes;
#endif

/**
 * @brief   Allocates new packet part in the packet buffer.
 *
 * @details pktsnip_t::type of the result will be set to @ref PKT_PROTO_UNKNOWN
 *          and pktsnip_t::users to 1.
 *
 * @param[in] size  The length of the packet you want to allocate.
 *
 * @return  Pointer to the packet in the packet buffer, on success.
 * @return  NULL, if no space is left in the packet buffer or size was 0.
 */
pktsnip_t *pktbuf_alloc(pktsize_t size);

/**
 * @brief   Reallocates pktsnip_t::data of @p pkt in the packet buffer, without
 *          changing the content.
 *
 * @pre `pkt->users == 1 && pkt->next == NULL`
 *
 * @details If enough memory is available behind it or @p size is smaller than
 *          the original size the packet then pktsnip_t::data of @p pkt will
 *          not be moved. Otherwise, it will be moved. If no space is available
 *          nothing happens.
 *
 * @param[in] pkt           A packet part.
 * @param[in] size          The size for @p pkt.
 *
 * @return  0, on success
 * @return  EINVAL, if precondition is not met
 * @return  ENOENT, if pktsnip_t::data of @p pkt was not from the packet buffer.
 * @return  ENOMEM, if no space is left in the packet buffer or size was 0.
 */
int pktbuf_realloc_data(pktsnip_t *pkt, pktsize_t size);

/**
 * @brief   Allocates new packet part in the packet buffer and sets the data
 *          for it.
 *
 * @details pktsnip_t::type of the result will be set to @ref PKT_PROTO_UNKNOWN
 *          and pktsnip_t::users to 1.
 *
 * @param[in] data  Data you want to insert into the packet.
 *                  If @p data is NULL, the call is equivalent to
 *                  `pktbuf_alloc(size)`
 * @param[in] size  The length of the packet you want to allocate.
 *
 * @return  Pointer to the packet in the packet buffer, on success.
 * @return  NULL, if no space is left in the packet buffer or size was 0.
 */
static inline pktsnip_t *pktbuf_insert(const void *data, pktsize_t size)
{
    pktsnip_t *pkt;

    pkt = pktbuf_alloc(size);

    /* currently only I own a pointer to that so copying can be done without
     * locking */
    if (pkt != NULL && data != NULL) {
        memcpy(pkt->data, data, size);
    }

    return pkt;
}

/**
 * @brief   Prepends a header to a packet.
 *
 * @details It is ill-advised to add a header simply by using
 *
 *     pkt = pkt_alloc(size);
 *     header = pkt_alloc(header_size);
 *
 *     header->next = pkt;
 *     pkt->data = pkt->data + header_size;
 *
 * Since @p data can be in the range of the data allocated on
 * pktsnip_t::data of @p pkt, it would be impossible to free
 * pktsnip_t::data of @p pkt, after @p pkt was released and the
 * generated header not or vice versa. This function ensures that this
 * can't happen.
 *
 * @param[in] pkt   The packet you want to add the header to. If pktsnip_t::data
 *                  field of @p pkt is equal to data it will be set to
 *                  `pkt->data + size`. If @p pkt is NULL it will the
 *                  pktsnip_t::next field of the result will be also set to NULL.
 * @param[in] data  Data of the header. If @p data is NULL no data will be
 *                  inserted into the result.
 * @param[in] size  Size of the header.
 * @param[in] type  Protocol type of the header.
 *
 * @return  Pointer to the packet part that represents the new header.
 * @return  NULL, if no space is left in the packet buffer or size was 0.
 */
pktsnip_t *pktbuf_add_header(pktsnip_t *pkt, void *data, pktsize_t size,
                             pkt_proto_t type);

/**
 * @brief   Increases pktsnip_t::users of @pkt atomically.
 *
 * @param[in] A packet.
 */
static inline void pktbuf_hold(pktsnip_t *pkt)
{
    if (pkt != NULL) {
        atomic_set_return(&(pkt->users), pkt->users + 1);
    }
}

/**
 * @brief   Decreases pktsnip_t::users of @pkt atomically and removes it if it
 *          reaches 0.
 *
 * @param[in] pkt   A packet.
 */
void pktbuf_release(pktsnip_t *pkt);

/**
 * @brief   Must be called once before there is a write operation in a thread.
 *
 * @details This function duplicates a packet in the packet buffer if
 *          pktsnip_t::users of @p pkt > 1.
 *
 * @note    Do *not* call this function in a thread twice on the same packet.
 *
 * @param[in] pkt   The packet you want to write into.
 *
 * @return  The (new) pointer to the pkt.
 * @return  NULL, if pktsnip_t::users of @p pkt > 1 and if there is not anough
 *          space in the packet buffer.
 */
pktsnip_t *pktbuf_start_write(pktsnip_t *pkt);

/**
 * @brief   Checks if a given data pointer is stored in the packet buffer.
 *
 * @param[in] ptr   Pointer to be checked
 *
 * @note    This will not work for pktsnip_t types, when they are dynamically
 *          allocated, but for there pktsnip_t::data field it will.
 *
 * @return  true, if @p ptr is in packet buffer
 * @return  false, otherwise
 */
bool pktbuf_contains(const void *ptr);

/* for testing */
#ifdef TEST_SUITES
/**
 * @brief   Checks if packet buffer is empty
 *
 * @return  true, if packet buffer is empty
 * @return  false, if packet buffer is not empty
 */
bool pktbuf_is_empty(void);

/**
 * @brief   Resets the whole packet buffer
 */
void pktbuf_reset(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* PKTBUF_H_ */
/** @} */
