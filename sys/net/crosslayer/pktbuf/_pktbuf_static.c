/*
 * Copyright (C) 2015 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "pktbuf.h"
#include "_pktbuf_static.h"

/* only for static packet buffer */
#if PKTBUF_SIZE > 0

/* make _bufsz_t dependent on PKTBUF_SIZE */
#if PKTBUF_SIZE < 256
typedef uint8_t _bufsz_t;
#elif PKTBUF_SIZE < 65536
typedef uint16_t _bufsz_t;
#else
typedef size_t _bufsz_t;
#endif

#ifdef DEVELHELP
unsigned int pktbuf_max_bytes = 0;
#endif

/**
 * @brief   Data type to represent empty chunks in packet buffer.
 */
typedef struct __attribute__((packed)) _used_t {
    struct _used_t *next;
    _bufsz_t size;
} _used_t;

static uint8_t _buf[PKTBUF_SIZE];
/**
 * @brief   Get first element in buffer
 */
static inline _used_t *_head(void)
{
    return (_used_t *)_buf;
}

/**
 * @brief Get data part (memory behind `_used_t` descriptive header) of a packet
 */
static inline void *_data(_used_t *node)
{
    return (void *)(((_used_t *)node) + 1);
}

/**
 * @brief   Size with metadata of allocation
 */
static inline size_t _total_sz(_bufsz_t sz)
{
    return sizeof(_used_t) + sz;
}

/**
 * @brief   _used_t typed alias for _total_sz
 */
static inline size_t __total_sz(_used_t *node)
{
    return _total_sz(node->size);
}

/**
 * @brief   Index of an allocation's first byte in buffer
 */
static inline unsigned int _start_idx(_used_t *node)
{
    return (int)(((uint8_t *)node) - _buf);
}

/**
 * @brief   Index of an allocation's last byte in buffer
 */
static inline unsigned int _end_idx(_used_t *node)
{
    return _start_idx(node) + __total_sz(node) - 1;
}

static _used_t *_find(_used_t **prev_ptr, _used_t **node_ptr, const void *ptr)
{
    _used_t *node = _head(), *prev = NULL;

    if (ptr != NULL) {
        while (node != NULL) {
            if (_data(node) == ptr) {
                *prev_ptr = prev;
                *node_ptr = node;
                return node;
            }

            prev = node;
            node = (_used_t *)node->next;
        }
    }

    *prev_ptr = NULL;
    *node_ptr = NULL;

    return NULL;
}

void *_pktbuf_static_alloc(size_t size)
{
    _used_t *node = _head(), *old_next, *new_next;

    if ((size == 0) || (size > PKTBUF_SIZE)) {
        return NULL;
    }

    if (node->size == 0) {          /* if head is currently not initialized */
        if (node->next == NULL || (_start_idx(node->next) >= _total_sz(size))) {
            /* if enough space is there */
            node->size = size;  /* just take it */

            return _data(node);
        }
        else if (node->next != NULL) {
            /* else go directly to next allocation if it exists */
            node = node->next;
        }
    }

    while ((node->next != NULL)
           /* and if space between current and next allocation is not big enough */
           && ((_start_idx(node->next) - _end_idx(node)) < _total_sz(size))) {
        node = node->next;
    }

    /* jump ahead size of current packet */
    new_next = (_used_t *)(((uint8_t *)node) + __total_sz(node));

    if ((((uint8_t *)new_next) + size) > (((uint8_t *)_head()) + PKTBUF_SIZE)) {
        /* new packet does not fit into _pktbuf */
        return NULL;
    }

    old_next = node->next;
    node->next = new_next;
    node->next->next = old_next;

    node = new_next;
    node->size = size;

#ifdef DEVELHELP

    if ((_end_idx(node) + 1) > pktbuf_max_bytes) {
        pktbuf_max_bytes = _end_idx(node) + 1;
    }

#endif

    return _data(node);
}

static void _free_helper(_used_t *prev, _used_t *node)
{
    if (prev == NULL) {
        node->size = 0;
    }
    else {
        prev->next = node->next;
    }
}

void _pktbuf_static_free(void *ptr)
{
    _used_t *prev, *node;

    if (_find(&prev, &node, ptr) != NULL) {
        _free_helper(prev, node);
    }
}

void *_pktbuf_static_realloc(void *ptr, size_t size)
{
    _used_t *new, *prev, *orig = NULL;

    if ((size == 0) || (size > PKTBUF_SIZE)) {
        return NULL;
    }

    _find(&prev, &orig, ptr);

    if ((orig != NULL) &&
        ((orig->size >= size)       /* size in orig is sufficient */
         || ((orig->next == NULL)   /* or orig is last packet and buffer space is sufficient */
             && ((_start_idx(orig) + _total_sz(size)) < PKTBUF_SIZE))
         || ((orig->next != NULL)   /* or space between orig and orig->next is sufficient */
             && ((_start_idx(orig->next) - _start_idx(orig)) >= _total_sz(size))))) {
        orig->size = size;

        return ptr;
    }

    new = _pktbuf_static_alloc(size);

    if (new != NULL) {
        memcpy(_data(new), ptr, size);

        /* cppcheck-suppress nullPointer this must be a bug in cppcheck,
         *                               makes no sense otherwise */
        if (orig != NULL) {
            _free_helper(prev, orig);
        }

        return _data(new);
    }

    return NULL;
}

bool _pktbuf_static_contains(const void *ptr)
{
    return ((_buf < ((uint8_t *)ptr)) && (((uint8_t *)ptr) <= &(_buf[PKTBUF_SIZE - 1])));
}

/* for testing */
#ifdef TEST_SUITES
bool _pktbuf_static_is_empty(void)
{
    return ((_head()->next == NULL) && (_head()->size == 0));
}

void _pktbuf_static_reset(void)
{
    _head()->next = NULL;
    _head()->size = 0;
#ifdef DEVELHELP
    pktbuf_max_bytes = 0;
#endif
}
#endif  /* TEST_SUITES */
#endif  /* PKTBUF_SIZE > 0 */

/** @} */
