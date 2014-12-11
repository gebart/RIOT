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
 * @file    pktbuf.c
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "clist.h"
#include "mutex.h"
#include "pkt.h"
#include "pktbuf.h"
#include "_pktbuf_static.h"

#if PKTBUF_SIZE > 0
#define ALLOC(size)         _pktbuf_static_alloc(size)
#define REALLOC(ptr, size)  _pktbuf_static_realloc(ptr, size)
#define FREE(ptr)           _pktbuf_static_free(ptr)
#else
#define ALLOC(size)         (((size) == 0) ? NULL : malloc(size))
#define REALLOC(ptr, size)  (((size) == 0) ? NULL : realloc(ptr, size))
#define FREE(ptr)           free(ptr)
#endif

/* chunk table to allow for FREE(ptr + x)-like behaviour */
typedef struct __attribute__((packed)) _chunk_list_t {
    struct _chunk_list_t *next;
    struct _chunk_list_t *prev;
    uint8_t *ptr;
} _chunk_list_t;

typedef struct __attribute__((packed)) _chunk_table_t {
    struct _chunk_table_t *next;
    uint8_t *range_start;
    size_t range_len;
    _chunk_list_t *chunks;
    uint8_t used;
} _chunk_table_t;

#if defined(TEST_SUITES) && PKTBUF_SIZE == 0 /* for reseting of dynamic pktbuf
                                              * in testing */
#include <stdio.h>

#ifndef TEST_MAX_PKT
#define TEST_MAX_PKT    (10)
#endif

static pktsnip_t *_allocated_pkts[TEST_MAX_PKT];

static void _test_add_pktsnip(pktsnip_t *pkt)
{
    for (int i = 0; i < TEST_MAX_PKT; i++) {
        if (_allocated_pkts[i] == NULL) {
            _allocated_pkts[i] = pkt;
            return;
        }
    }

    puts("Number of allowed packets in test mode exceeded, please increase "
         "TEST_MAX_PKT at compile time");
}

static void _test_rem_pktsnip(pktsnip_t *pkt)
{
    for (int i = 0; i < TEST_MAX_PKT; i++) {
        if (_allocated_pkts[i] == pkt) {
            _allocated_pkts[i] = NULL;
            return;
        }
    }
}
#endif

static mutex_t _pktbuf_mutex = MUTEX_INIT;
static _chunk_table_t *_chunk_table = NULL;

/* this organizes chunks, since free(ptr + x) is not possible on most platforms */
static _chunk_table_t *_create_table_entry(void *pkt, pktsize_t size);
static _chunk_table_t *_find_chunk(const uint8_t *chunk, _chunk_table_t **prev,
                                   _chunk_list_t **node_res);
static void _free_chunk(void *chunk);
static inline bool _in_range(_chunk_table_t *entry, uint8_t *ptr);
static bool _add_chunk(uint8_t *ptr);

/* internal pktbuf functions */
static pktsnip_t *_pktbuf_alloc_unsafe(pktsize_t size);
static pktsnip_t *_pktbuf_add_header_unsafe(pktsnip_t *pkt, void *data,
        pktsize_t size, pkt_proto_t type);
static pktsnip_t *_pktbuf_duplicate(const pktsnip_t *pkt);

pktsnip_t *pktbuf_alloc(pktsize_t size)
{
    pktsnip_t *res;

    mutex_lock(&_pktbuf_mutex);
    res = _pktbuf_alloc_unsafe(size);
    mutex_unlock(&_pktbuf_mutex);

    return res;
}

int pktbuf_realloc_data(pktsnip_t *pkt, pktsize_t size)
{
    uint8_t *new;
    _chunk_table_t *entry;
    _chunk_list_t *node = NULL;

    if (pkt == NULL || !pktbuf_contains(pkt->data)) {
        return ENOENT;
    }

    if (pkt->users > 1 || pkt->next != NULL) {
        return EINVAL;
    }

    mutex_lock(&_pktbuf_mutex);

    entry = _find_chunk(pkt->data, NULL, &node);

    /* entry can't be NULL since prelimanary pktbuf_contains() check ensures that */
    if ((pkt->data == entry->range_start) && (entry->chunks == NULL)) {
        new = REALLOC(entry->range_start, size);

        if (new == NULL) {
            mutex_unlock(&_pktbuf_mutex);

            return ENOMEM;
        }

        entry->range_start = new;
        entry->range_len = size;
    }
    else {
        _chunk_table_t *chunk_entry;

        new = ALLOC(size);

        if (new == NULL) {
            mutex_unlock(&_pktbuf_mutex);

            return ENOMEM;
        }

        chunk_entry = _create_table_entry(new, size);

        if (chunk_entry == NULL) {
            FREE(new);
            mutex_unlock(&_pktbuf_mutex);

            return ENOMEM;
        }

        memcpy(new, pkt->data, (size < pkt->size) ? size : pkt->size);
        _free_chunk(pkt->data);
    }

    mutex_unlock(&_pktbuf_mutex);

    pkt->data = new;
    pkt->size = size;

    return 0;
}

pktsnip_t *pktbuf_add_header(pktsnip_t *pkt, void *data, pktsize_t size,
                             pkt_proto_t type)
{
    pktsnip_t *snip;

    mutex_lock(&_pktbuf_mutex);

    snip = _pktbuf_add_header_unsafe(pkt, data, size, type);

    mutex_unlock(&_pktbuf_mutex);

    return snip;
}

void pktbuf_release(pktsnip_t *pkt)
{
    atomic_set_return(&(pkt->users), pkt->users - 1);

    if (pkt->users == 0 && pktbuf_contains(pkt->data)) {
        mutex_lock(&_pktbuf_mutex);

        _free_chunk(pkt->data);
        FREE(pkt);

#if defined(TEST_SUITES) && PKTBUF_SIZE == 0
        _test_rem_pktsnip(pkt);
#endif

        mutex_unlock(&_pktbuf_mutex);
    }
}

pktsnip_t *pktbuf_start_write(pktsnip_t *pkt)
{
    if (pkt->users > 1) {
        pktsnip_t *res = NULL;

        mutex_lock(&_pktbuf_mutex);

        res = _pktbuf_duplicate(pkt);

        atomic_set_return(&pkt->users, pkt->users - 1);

        mutex_unlock(&_pktbuf_mutex);

        return res;
    }

    return pkt;
}

bool pktbuf_contains(const void *ptr)
{
#if PKTBUF_SIZE > 0
    return (bool)_pktbuf_static_contains(ptr);
#else
    return (_find_chunk(ptr, NULL, NULL) != NULL);
#endif
}

/***********************************
 * internal pktbuf functions       *
 ***********************************/

static pktsnip_t *_pktbuf_alloc_unsafe(pktsize_t size)
{
    pktsnip_t *pkt;
    _chunk_table_t *chunk_entry;

    pkt = (pktsnip_t *)ALLOC(sizeof(pktsnip_t));

    if (pkt == NULL) {
        return NULL;
    }

    pkt->data = ALLOC(size);

    if (pkt->data == NULL) {
        FREE(pkt);

        return NULL;
    }

    chunk_entry = _create_table_entry(pkt->data, size);

    if (chunk_entry == NULL) {
        FREE(pkt->data);
        FREE(pkt);

        return NULL;
    }

#if defined(TEST_SUITES) && PKTBUF_SIZE == 0
    _test_add_pktsnip(pkt);
#endif

    pkt->next = NULL;
    pkt->size = size;
    pkt->type = PKT_PROTO_UNKNOWN;
    pkt->users = 1;

    return pkt;
}

static pktsnip_t *_pktbuf_add_header_unsafe(pktsnip_t *pkt, void *data,
        pktsize_t size, pkt_proto_t type)
{
    pktsnip_t *snip;

    if (size == 0) {
        return NULL;
    }

    snip = (pktsnip_t *)ALLOC(sizeof(pktsnip_t));

    if (snip == NULL) {
        return NULL;
    }

    if (pkt == NULL || pkt->data != data) {
        _chunk_table_t *chunk_entry = NULL;

        snip->data = ALLOC(size);

        if (snip->data == NULL) {
            FREE(snip);

            return NULL;
        }

        chunk_entry = _create_table_entry(snip->data, size);

        if (chunk_entry == NULL) {
            FREE(snip->data);
            FREE(snip);

            return NULL;
        }

        if (data != NULL) {
            memcpy(snip->data, data, size);
        }
    }
    else {
        snip->data = data;
        pkt->size -= size;
        pkt->data = (void *)(((uint8_t *)pkt->data) + size);

        if (!_add_chunk(pkt->data)) {
            FREE(snip);

            return NULL;
        }
    }

#if defined(TEST_SUITES) && PKTBUF_SIZE == 0
    _test_add_pktsnip(snip);
#endif

    snip->next = NULL;
    snip->size = size;
    snip->type = type;
    snip->users = 1;

    pktsnip_add(&snip, pkt);

    return snip;
}

static pktsnip_t *_pktbuf_duplicate(const pktsnip_t *pkt)
{
    pktsnip_t *res = NULL;

    res = _pktbuf_alloc_unsafe(pkt->size);

    if (res == NULL) {
        return NULL;
    }

    memcpy(res->data, pkt->data, pkt->size);
    res->type = pkt->type;

    while (pkt->next) {
        pktsnip_t *header = NULL;

        pkt = pkt->next;
        header = _pktbuf_add_header_unsafe(res, pkt->data, pkt->size, pkt->type);

        if (header == NULL) {
            do {
                pktsnip_t *next = res->next;

                _free_chunk(res->data);
                FREE(res);

                res = next;
            } while (res);

            return NULL;
        }
    }

    return res;
}

/**********************************
 * chunk management functions     *
 **********************************/
static _chunk_table_t *_find_chunk(const uint8_t *chunk, _chunk_table_t **prev,
                                   _chunk_list_t **node_res)
{
    _chunk_table_t *entry = _chunk_table;

    if (prev != NULL) {
        *prev = NULL;
    }

    while (entry != NULL) {
        _chunk_list_t *node = entry->chunks;

        if (entry->range_start == chunk) {
            if (node_res != NULL) {
                *node_res = NULL;
            }

            return entry;
        }

        if (node != NULL) {
            do {
                if (node->ptr == chunk) {
                    if (node_res != NULL) {
                        *node_res = node;
                    }

                    return entry;
                }

                clist_advance((clist_node_t **)&node);
            } while (node != entry->chunks);
        }

        if (prev != NULL) {
            *prev = entry;
        }

        entry = entry->next;
    }

    return NULL;
}

static void _free_chunk(void *chunk)
{
    _chunk_list_t *node = NULL;
    _chunk_table_t *prev = NULL, *entry = _find_chunk(chunk, &prev, &node);

    if (node != NULL) {
        clist_remove((clist_node_t **)&entry->chunks, (clist_node_t *)node);
        FREE(node);
    }
    else if (entry->range_start == chunk) {
        entry->used = 0;
    }

    if (entry->chunks == NULL && entry->used == 0) {
        if (prev == NULL) {
            if (entry->next == NULL) {
                _chunk_table = NULL;
            }
            else {
                _chunk_table = entry->next;
            }
        }
        else {
            prev->next = entry->next;
        }

        FREE(entry->range_start);
        FREE(entry);
    }
}

static inline bool _in_range(_chunk_table_t *entry, uint8_t *ptr)
{
    return (entry != NULL) &&
           ((ptr >= entry->range_start) ||
            (ptr < (entry->range_start + entry->range_len)));
}

static bool _add_chunk(uint8_t *ptr)
{
    _chunk_table_t *entry = _chunk_table;

    while (entry != NULL) {
        if (_in_range(entry, ptr)) {
            _chunk_list_t *node = ALLOC(sizeof(_chunk_list_t));

            if (node == NULL) {
                return 0;
            }

            node->ptr = ptr;
            clist_add((clist_node_t **)&entry->chunks, (clist_node_t *)node);

            return 1;
        }
    }

    return 0;
}

static _chunk_table_t *_create_table_entry(void *data, pktsize_t size)
{
    _chunk_table_t *chunk_entry = (_chunk_table_t *)ALLOC(sizeof(_chunk_table_t));

    if (chunk_entry == NULL) {
        return NULL;
    }

    if (_chunk_table == NULL) {
        chunk_entry->next = NULL;
        _chunk_table = chunk_entry;
    }
    else {
        chunk_entry->next = _chunk_table;
        _chunk_table = chunk_entry;
    }

    chunk_entry->range_start = data;
    chunk_entry->range_len = size;
    chunk_entry->chunks = NULL;
    chunk_entry->used = 1;

    return chunk_entry;
}

#ifdef TEST_SUITES
bool pktbuf_is_empty(void)
{
#if PKTBUF_SIZE > 0
    return _pktbuf_static_is_empty() && _chunk_table == NULL;
#else

    for (int i = 0; i < TEST_MAX_PKT; i++) {
        if (_allocated_pkts[i] != NULL) {
            return false;
        }
    }

    return (_chunk_table == NULL);
#endif
}

void pktbuf_reset(void)
{
#if PKTBUF_SIZE > 0
    _pktbuf_static_reset();
#else
    _chunk_table_t *entry = _chunk_table;

    for (int i = 0; i < TEST_MAX_PKT; i++) {
        if (_allocated_pkts[i] != NULL) {
            FREE(_allocated_pkts[i]->data);
            FREE(_allocated_pkts[i]);
            _allocated_pkts[i] = NULL;
        }
    }

    while (entry != NULL) {
        _chunk_table_t *next = entry->next;
        _chunk_list_t *node = entry->chunks;

        while (entry->chunks != NULL) {
            clist_remove((clist_node_t **)&entry->chunks, (clist_node_t *)node);
            FREE(node);
        }

        FREE(entry);
        entry = next;
    }

#endif
    _chunk_table = NULL;
}
#endif

/** @} */
