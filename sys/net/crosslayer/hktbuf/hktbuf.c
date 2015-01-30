
#include <stdint.h>
#include <stdbool.h>

#include "mutex.h"
#include "atomic.h"
#include "pktbuf.h"
#include "pkt.h"

typedef struct foobees_t freebees_t;

typedef struct foobees_t {
    freebees_t *next;
    pktsize_t size;
};

static uint8_t _mem[PKTBUF_SIZE];
static freebees_t *_freebees;
static mutex_t _lock;


void pktbuf_init(void)
{
    _freebees = (freebees_t *)_mem;
    _freebees->next = NULL;
    _freebees->size = PKTBUF_SIZE - sizeof(freebees_t);
    /* make sure the end of the free space is aligned in memory */
    if (PKTBUF_SIZE & 0x03) {
        _freebees->size -= PKTBUF_SIZE & 0x03;
    }
}

pktsnip_t *pktbuf_alloc(pktsize_t size)
{
    pktsnip_t *res = NULL;
    freebees_t *chunk = _freebees;
    pktsize_t to_alloc = (size + sizeof(pktsnip_t));
    /* make sure to_alloc is a multiple of 4 (alignment) */
    if (to_alloc & 0x03) {
        to_alloc += (4 - (to_alloc & 0x03));
    }

    /* make sure we are the only one looking for memory */
    mutex_lock(&_lock);
    /* cycle through all free chunks and see if one is large enough */
    while (chunk) {
        /* does it fit? */
        if (chunk->size >= to_alloc) {
            /* yes... */
            chunk->size -= to_alloc;
            res = (pktsnip_t *)(((uint8_t *)chunk) + chunk->size + 1);
            chunk = NULL;
        }
        else {
            chunk = chunk->next;
        }
    }
    /* now others can look again */
    mutex_unlock(&_lock);

    /* if we found memory, preset pktsnip values */
    if (res) {
        res->next = NULL;
        res->type = PKT_PROTO_UNKNOWN;
        res->users = 1;
        res->size = size;
        res->data = (uint8_t *)(((uint8_t *)res) + sizeof(pktsnip_t));
    }

    return res;
}

pktsnip_t *pktbuf_add_header(pktsnip_t *pkt, void *data, pktsize_t size,
                             pkt_proto_t type)
{
    pktsnip_t *res;

    res = pktbuf_alloc(size);
    if (res) {
        res->type = type;
        res->next = pkt;
        memcpy(res->data, data, size);
    }
    return res;
}

void pktbuf_release(pktsnip_t *pkt)
{
    freebees_t *chunk = _freebees;
    freebees_t *last = NULL;

    mutex_lock(&_lock);
    atomic_set_return(&pkt->users, (pkt->users - 1));
    if (pkt->users == 0) {
        while (chunk) {
            /* will be false at one time, as the last chunk starts at the lowest pktbuf address... */
            if ((void *)chunk > (void *)pkt) {
                last = chunk;
                chunk = chunk->next;
            }
            else {
                /* see if pkt is right after free space */
                if (((uint8_t *)chunk + chunk->size + 1) == (uint8_t *)pkt) {
                    /* expand chunk and drop pkt */
                    chunk->size += pkt->size + sizeof(pktsnip_t);
                    pkt = NULL;
                    /* keep alignment */
                    if (chunk->size & 0x03) {
                        chunk->size += (4 - (chunk->size & 0x03));
                    }
                }
                else {
                    pktsize_t tmp = pkt->size;
                    freebees_t *insert = (freebees_t *)pkt;
                    insert->size = tmp;
                    if (tmp & 0x03) {
                        insert->size += (4 - (tmp & 0x03));
                    }
                    insert->next = chunk;
                    if (last) {
                        last->next = insert;
                    }
                    chunk = insert;
                }
                /* see if two chunks can be joined */
                /* TODO: go through list and join adjacing chunks */
                chunk = NULL;
            }
        }
    }
    mutex_unlock(&_lock);
}

pktsnip_t *pktbuf_start_write(pktsnip_t *pkt)
{
    pktsnip_t *res = NULL;

    if (pkt->users == 1) {
        return pkt;
    }

    res = pktbuf_alloc(pkt->size);
    if (res) {
        res->next = pkt->next;
        res->type = pkt->type;
        memcpy(res->data, pkt->data, pkt->size);
        atomic_set_return(&pkt->users, (pkt->users - 1));
    }

    return res;
}


bool pktbuf_contains(const void *ptr)
{
    /* no idea how to implement in this case... -> whats the use case behind
       this function, I think we don't need it... */
    return false;
}
