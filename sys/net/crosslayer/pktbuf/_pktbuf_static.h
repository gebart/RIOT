/*
 * Copyright (C) 2015 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pktbuf
 * @{
 *
 * @file
 * @brief   Internal definitions for the static packet buffer
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef XPKTBUF_STATIC_H_
#define XPKTBUF_STATIC_H_

#include <stdbool.h>

#include "pkt.h"

/**
 * @brief   Internal alloc on static packet buffer
 *
 * @internal
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html">
 *          malloc()
 *      </a>
 */
void *_pktbuf_static_alloc(size_t size);

/**
 * @brief   Internal realloc on static packet buffer
 *
 * @internal
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/realloc.html">
 *          realloc()
 *      </a>
 */
void *_pktbuf_static_realloc(void *ptr, size_t size);

/**
 * @brief   Internal free on static packet buffer
 *
 * @internal
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html">
 *          free()
 *      </a>
 */
void _pktbuf_static_free(void *ptr);

/**
 * @brief   Checks if a pointer is part of the static packet buffer.
 *
 * @param[in] ptr   A pointer.
 *
 * @return true, if @p ptr is part of the static packet buffer.
 * @return false, if @p ptr is not part of the static packet buffer.
 */
bool _pktbuf_static_contains(const void *ptr);

/* for testing */
#ifdef TEST_SUITES
/**
 * @brief   Counts the number of allocated bytes
 *
 * @return  Number of allocated bytes
 */
size_t _pktbuf_static_bytes_allocated(void);

/**
 * @brief   Checks if packet buffer is empty
 *
 * @return  1, if packet buffer is empty
 * @return  0, if packet buffer is not empty
 */
bool _pktbuf_static_is_empty(void);

/**
 * @brief   Sets the whole packet buffer to 0
 */
void _pktbuf_static_reset(void);
#endif  /* TEST_SUITES */

#endif /* XPKTBUF_STATIC_H_ */
/** @} */
