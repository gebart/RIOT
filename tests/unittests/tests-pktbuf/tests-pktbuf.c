/*
 * Copyright (C) 2014 Martine Lenders <mail@martine-lenders.eu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file    tests-pktbuf.c
 */
#include <errno.h>
#include <stdint.h>

#include "embUnit.h"

#include "pkt.h"
#include "pktbuf.h"

#include "tests-pktbuf.h"

typedef struct __attribute__((packed)) {
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    int8_t s8;
    int16_t s16;
    int32_t s32;
    int64_t s64;
} test_pktbuf_struct_t;

static void tear_down(void)
{
    pktbuf_reset();
}

static void test_pktbuf_reset(void)
{
    TEST_ASSERT(pktbuf_is_empty());
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(1));
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(2));
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(3));
    TEST_ASSERT(!pktbuf_is_empty());
    pktbuf_reset();
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_alloc__size_0(void)
{
    TEST_ASSERT_NULL(pktbuf_alloc(0));
    TEST_ASSERT(pktbuf_is_empty());
}

#if PKTBUF_SIZE > 0
static void test_pktbuf_alloc__memfull(void)
{
    TEST_ASSERT_NULL(pktbuf_alloc(PKTBUF_SIZE + 1));
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_alloc__memfull2(void)
{
    TEST_ASSERT_NULL(pktbuf_alloc(PKTBUF_SIZE - sizeof(pktsnip_t)));
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_alloc__memfull3(void)
{
    for (int i = 0; i < 9; i++) {
        TEST_ASSERT_NOT_NULL(pktbuf_alloc((PKTBUF_SIZE / 10) + 4));
        /* Why 4? Because: http://xkcd.com/221/, thats why ;-) */
    }

    TEST_ASSERT_NULL(pktbuf_alloc((PKTBUF_SIZE / 10) + 4));
}
#endif

static void test_pktbuf_alloc__success(void)
{
    pktsnip_t *pkt, *pkt_prev = NULL;

    for (int i = 0; i < 9; i++) {
        pkt = pktbuf_alloc((PKTBUF_SIZE / 10) + 4);

        TEST_ASSERT_NOT_NULL(pkt);
        TEST_ASSERT_NULL(pkt->next);
        TEST_ASSERT_NOT_NULL(pkt->data);
        TEST_ASSERT_EQUAL_INT((PKTBUF_SIZE / 10) + 4, pkt->size);
        TEST_ASSERT_EQUAL_INT(PKT_PROTO_UNKNOWN, pkt->type);
        TEST_ASSERT_EQUAL_INT(1, pkt->users);

        if (pkt_prev != NULL) {
            TEST_ASSERT(pkt_prev < pkt);
            TEST_ASSERT(pkt_prev->data < pkt->data);
        }

        pkt_prev = pkt;
    }
}

static void test_pktbuf_realloc_data__pkt_NULL(void)
{
    TEST_ASSERT_EQUAL_INT(ENOENT, pktbuf_realloc_data(NULL, 0));
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_realloc_data__pkt_wrong(void)
{
    pktsnip_t pkt = { NULL, TEST_STRING8, sizeof(TEST_STRING8), PKT_PROTO_UNKNOWN, 1};

    TEST_ASSERT_EQUAL_INT(ENOENT, pktbuf_realloc_data(&pkt, 0));
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_realloc_data__pkt_data_wrong(void)
{
    pktsnip_t *pkt = pktbuf_alloc(sizeof(TEST_STRING8));
    void *orig_data = pkt->data;
    pkt->data = TEST_STRING8;

    TEST_ASSERT_EQUAL_INT(ENOENT, pktbuf_realloc_data(pkt, 0));
    pkt->data = orig_data;
    pktbuf_release(pkt);
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_realloc_data__pkt_users_gt_1(void)
{
    pktsnip_t *pkt = pktbuf_alloc(sizeof(TEST_STRING8));
    pktbuf_hold(pkt);

    TEST_ASSERT_EQUAL_INT(EINVAL, pktbuf_realloc_data(pkt, sizeof(TEST_STRING8) - 1));
    pktbuf_release(pkt);
    pktbuf_release(pkt);
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_realloc_data__pkt_next_neq_NULL(void)
{
    pktsnip_t *pkt = pktbuf_alloc(sizeof(TEST_STRING8));
    pkt->next = pkt;

    TEST_ASSERT_EQUAL_INT(EINVAL, pktbuf_realloc_data(pkt, sizeof(TEST_STRING8) - 1));
    pktbuf_release(pkt);
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_realloc_data__size_0(void)
{
    pktsnip_t *pkt = pktbuf_alloc(sizeof(TEST_STRING8));

    TEST_ASSERT_EQUAL_INT(ENOMEM, pktbuf_realloc_data(pkt, 0));
    pktbuf_release(pkt);
    TEST_ASSERT(pktbuf_is_empty());
}

#if PKTBUF_SIZE > 0
static void test_pktbuf_realloc_data__memfull(void)
{
    pktsnip_t *pkt = pktbuf_alloc(sizeof(TEST_STRING8));

    TEST_ASSERT_EQUAL_INT(ENOMEM, pktbuf_realloc_data(pkt, PKTBUF_SIZE + 1));
    pktbuf_release(pkt);
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_realloc_data__memfull2(void)
{
    pktsnip_t *pkt = pktbuf_alloc(sizeof(TEST_STRING8));

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(sizeof(TEST_STRING16)));
    TEST_ASSERT_EQUAL_INT(ENOMEM, pktbuf_realloc_data(pkt, PKTBUF_SIZE - sizeof(TEST_STRING8)));
}

static void test_pktbuf_realloc_data__memfull3(void)
{
    pktsnip_t *pkt;

    TEST_ASSERT_NOT_NULL(pktbuf_alloc(sizeof(TEST_STRING16)));

    pkt = pktbuf_alloc(TEST_UINT8);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(sizeof(TEST_STRING8)));
    TEST_ASSERT_EQUAL_INT(ENOMEM, pktbuf_realloc_data(pkt, PKTBUF_SIZE - TEST_UINT8));
}

/* dynamic malloc is a little more unpredictable ;-) */
static void test_pktbuf_realloc_data__shrink(void)
{
    pktsnip_t *pkt;
    void *exp_data;

    pkt = pktbuf_alloc(sizeof(TEST_STRING16));
    exp_data = pkt->data;

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(4));

    TEST_ASSERT_EQUAL_INT(0, pktbuf_realloc_data(pkt, sizeof(TEST_STRING8)));
    TEST_ASSERT(exp_data == pkt->data);
    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt->size);
    TEST_ASSERT_EQUAL_INT(PKT_PROTO_UNKNOWN, pkt->type);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);
}

static void test_pktbuf_realloc_data__memenough(void)
{
    pktsnip_t *pkt;
    void *exp_data;

    pkt = pktbuf_alloc(sizeof(TEST_STRING16));
    exp_data = pkt->data;

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(0, pktbuf_realloc_data(pkt, sizeof(TEST_STRING8)));
    TEST_ASSERT(exp_data == pkt->data);
    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt->size);
    TEST_ASSERT_EQUAL_INT(PKT_PROTO_UNKNOWN, pkt->type);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);
}

static void test_pktbuf_realloc_data__nomemenough(void)
{
    pktsnip_t *pkt1, *pkt2;
    void *exp_data;

    pkt1 = pktbuf_alloc(128);
    exp_data = pkt1->data;

    TEST_ASSERT_NOT_NULL(pkt1);

    pkt2 = pktbuf_alloc(128);

    TEST_ASSERT_NOT_NULL(pkt2);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(25));

    pktbuf_release(pkt2);

    TEST_ASSERT_EQUAL_INT(0, pktbuf_realloc_data(pkt1, 130));
    TEST_ASSERT(exp_data != pkt1->data);
    TEST_ASSERT_NULL(pkt1->next);
    TEST_ASSERT_EQUAL_INT(130, pkt1->size);
    TEST_ASSERT_EQUAL_INT(PKT_PROTO_UNKNOWN, pkt1->type);
    TEST_ASSERT_EQUAL_INT(1, pkt1->users);
}
#endif

static void test_pktbuf_realloc_data__success(void)
{
    pktsnip_t *pkt;

    pkt = pktbuf_insert(TEST_STRING8, sizeof(TEST_STRING16));

    TEST_ASSERT_NOT_NULL(pkt);

    TEST_ASSERT_EQUAL_INT(0, pktbuf_realloc_data(pkt, sizeof(TEST_STRING8)));
    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING8, pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt->size);
    TEST_ASSERT_EQUAL_INT(PKT_PROTO_UNKNOWN, pkt->type);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);
}

static void test_pktbuf_realloc_data__success2(void)
{
    pktsnip_t *pkt;

    pkt = pktbuf_insert(TEST_STRING8, sizeof(TEST_STRING8));

    TEST_ASSERT_NOT_NULL(pkt);

    TEST_ASSERT_EQUAL_INT(0, pktbuf_realloc_data(pkt, sizeof(TEST_STRING16)));
    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING8, pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING16), pkt->size);
    TEST_ASSERT_EQUAL_INT(PKT_PROTO_UNKNOWN, pkt->type);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);
}

static void test_pktbuf_realloc_data__further_down_the_line(void)
{
    pktsnip_t *pkt, *header;
    void *exp_data;

    pkt = pktbuf_insert(TEST_STRING16, sizeof(TEST_STRING16));
    exp_data = pkt->data;

    TEST_ASSERT_NOT_NULL(pkt);

    header = pktbuf_add_header(pkt, pkt->data, 4, PKT_PROTO_UNKNOWN);

    TEST_ASSERT_NOT_NULL(header);
    TEST_ASSERT(header->next == pkt);
    TEST_ASSERT_EQUAL_INT(4, header->size);
    TEST_ASSERT(((uint8_t *)pkt->data) == (((uint8_t *)header->data) + 4));
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING16) - 4, pkt->size);

    TEST_ASSERT_EQUAL_INT(0, pktbuf_realloc_data(pkt, 20));
    TEST_ASSERT(exp_data != pkt->data);
    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING16 + 4, pkt->data);
    TEST_ASSERT_EQUAL_INT(20, pkt->size);
    TEST_ASSERT_EQUAL_INT(PKT_PROTO_UNKNOWN, pkt->type);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);
    pktbuf_release(pkt);
    pktbuf_release(header);
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_insert__size_0(void)
{
    TEST_ASSERT_NULL(pktbuf_insert("", 0));
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_insert__data_NULL(void)
{
    pktsnip_t *pkt;

    TEST_ASSERT_NOT_NULL((pkt = pktbuf_insert(NULL, 4)));
    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_NOT_NULL(pkt->data);
    TEST_ASSERT_EQUAL_INT(4, pkt->size);
    TEST_ASSERT_EQUAL_INT(PKT_PROTO_UNKNOWN, pkt->type);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);
    TEST_ASSERT(!pktbuf_is_empty());
}

static void test_pktbuf_insert__success(void)
{
    pktsnip_t *pkt;

    TEST_ASSERT_NOT_NULL((pkt = pktbuf_insert(TEST_STRING8, sizeof(TEST_STRING8))));
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt->size);
    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING8, pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt->size);
    TEST_ASSERT_EQUAL_INT(PKT_PROTO_UNKNOWN, pkt->type);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);
    TEST_ASSERT(!pktbuf_is_empty());
}

static void test_pktbuf_insert__packed_struct(void)
{
    test_pktbuf_struct_t data = { 0x4d, 0xef43, 0xacdef574, 0x43644305695afde5,
                                  34, -4469, 149699748, -46590430597
                                };
    test_pktbuf_struct_t *data_cpy;
    pktsnip_t *pkt = pktbuf_insert(&data, sizeof(test_pktbuf_struct_t));
    data_cpy = (test_pktbuf_struct_t *)pkt->data;

    TEST_ASSERT_EQUAL_INT(data.u8, data_cpy->u8);
    TEST_ASSERT_EQUAL_INT(data.u16, data_cpy->u16);
    TEST_ASSERT_EQUAL_INT(data.u32, data_cpy->u32);
    TEST_ASSERT_EQUAL_INT(data.u64, data_cpy->u64);
    TEST_ASSERT_EQUAL_INT(data.s8, data_cpy->s8);
    TEST_ASSERT_EQUAL_INT(data.s16, data_cpy->s16);
    TEST_ASSERT_EQUAL_INT(data.s32, data_cpy->s32);
    TEST_ASSERT_EQUAL_INT(data.s64, data_cpy->s64);
}

static void test_pktbuf_add_header__pkt_NULL__data_NULL__size_0(void)
{
    TEST_ASSERT_NULL(pktbuf_add_header(NULL, NULL, 0, PKT_PROTO_UNKNOWN));
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_add_header__pkt_NOT_NULL__data_NULL__size_0(void)
{
    pktsnip_t *pkt = pktbuf_alloc(TEST_UINT8);

    TEST_ASSERT_NULL(pktbuf_add_header(pkt, NULL, 0, PKT_PROTO_UNKNOWN));
    pktbuf_release(pkt);
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_add_header__pkt_NULL__data_NOT_NULL__size_0(void)
{
    TEST_ASSERT_NULL(pktbuf_add_header(NULL, TEST_STRING8, 0, PKT_PROTO_UNKNOWN));
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_add_header__pkt_NOT_NULL__data_NOT_NULL__size_0(void)
{
    pktsnip_t *pkt = pktbuf_alloc(TEST_UINT8);

    TEST_ASSERT_NULL(pktbuf_add_header(pkt, TEST_STRING8, 0, PKT_PROTO_UNKNOWN));
    pktbuf_release(pkt);
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_add_header__pkt_NULL__data_NULL__size_not_0(void)
{
    pktsnip_t *header;

    TEST_ASSERT_NOT_NULL((header = pktbuf_add_header(NULL, NULL, sizeof(TEST_STRING8),
                                   PKT_PROTO_UNKNOWN)));
    TEST_ASSERT_NULL(header->next);
    TEST_ASSERT_NOT_NULL(header->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), header->size);
    TEST_ASSERT_EQUAL_INT(PKT_PROTO_UNKNOWN, header->type);
    TEST_ASSERT_EQUAL_INT(1, header->users);
    pktbuf_release(header);
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_add_header__pkt_NOT_NULL__data_NULL__size_not_0(void)
{
    pktsnip_t *pkt = pktbuf_alloc(TEST_UINT8), *header;

    TEST_ASSERT_NOT_NULL((header = pktbuf_add_header(pkt, NULL, sizeof(TEST_STRING8),
                                   PKT_PROTO_UNKNOWN)));
    TEST_ASSERT(header->next == pkt);
    TEST_ASSERT_NOT_NULL(header->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), header->size);
    TEST_ASSERT_EQUAL_INT(PKT_PROTO_UNKNOWN, header->type);
    TEST_ASSERT_EQUAL_INT(1, header->users);
    pktbuf_release(pkt);
    pktbuf_release(header);
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_add_header__pkt_NOT_NULL__data_NOT_NULL__size_not_0(void)
{
    pktsnip_t *pkt = pktbuf_alloc(TEST_UINT8), *header;

    TEST_ASSERT_NOT_NULL((header = pktbuf_add_header(pkt, TEST_STRING8, sizeof(TEST_STRING8),
                                   PKT_PROTO_UNKNOWN)));
    TEST_ASSERT(header->next == pkt);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING8, header->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), header->size);
    TEST_ASSERT_EQUAL_INT(PKT_PROTO_UNKNOWN, header->type);
    TEST_ASSERT_EQUAL_INT(1, header->users);
    pktbuf_release(header);
    pktbuf_release(pkt);
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_add_header__in_place(void)
{
    pktsnip_t *pkt = pktbuf_insert(TEST_STRING16, sizeof(TEST_STRING16)), *header;

    TEST_ASSERT_NOT_NULL((header = pktbuf_add_header(pkt, pkt->data, 4, PKT_PROTO_UNKNOWN)));
    TEST_ASSERT(header->next == pkt);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING16, header->data); /* there is no 0 byte */
    TEST_ASSERT_EQUAL_INT(4, header->size);
    TEST_ASSERT_EQUAL_INT(PKT_PROTO_UNKNOWN, header->type);
    TEST_ASSERT_EQUAL_INT(1, header->users);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING16 + 4, pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING16) - 4, pkt->size);
    TEST_ASSERT_EQUAL_INT(PKT_PROTO_UNKNOWN, pkt->type);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);
    pktbuf_release(header);
    pktbuf_release(pkt);
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_hold__pkt_null(void)
{
    pktbuf_hold(NULL);
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_hold__pkt_external(void)
{
    pktsnip_t pkt = { NULL, TEST_STRING8, sizeof(TEST_STRING8), PKT_PROTO_UNKNOWN, 1 };

    pktbuf_hold(&pkt);
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_hold__success(void)
{
    pktsnip_t *pkt = pktbuf_insert(TEST_STRING16, sizeof(TEST_STRING16));

    for (uint8_t i = 0; i < TEST_UINT8; i++) {
        uint8_t prev_users = pkt->users;
        pktbuf_hold(pkt);
        TEST_ASSERT_EQUAL_INT(prev_users + 1, pkt->users);
    }
}

static void test_pktbuf_release__pkt_null(void)
{
    pktbuf_release(NULL);
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_release__pkt_external(void)
{
    pktsnip_t pkt = { NULL, TEST_STRING8, sizeof(TEST_STRING8), PKT_PROTO_UNKNOWN, 1 };

    pktbuf_release(&pkt);
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_release__success(void)
{
    pktsnip_t *pkt = pktbuf_insert(TEST_STRING16, sizeof(TEST_STRING16));

    for (uint8_t i = 0; i < TEST_UINT8; i++) {
        uint8_t prev_users = pkt->users;
        pktbuf_hold(pkt);
        TEST_ASSERT_EQUAL_INT(prev_users + 1, pkt->users);
    }

    TEST_ASSERT(!pktbuf_is_empty());

    for (uint8_t i = 0; i < TEST_UINT8; i++) {
        uint8_t prev_users = pkt->users;
        pktbuf_release(pkt);
        TEST_ASSERT_EQUAL_INT(prev_users - 1, pkt->users);
    }

    TEST_ASSERT(!pktbuf_is_empty());
    pktbuf_release(pkt);
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_start_write__NULL(void)
{
    pktbuf_start_write(NULL);
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_start_write__pkt_users_1(void)
{
    pktsnip_t *pkt = pktbuf_insert(TEST_STRING16, sizeof(TEST_STRING16)), *pkt_copy;
    TEST_ASSERT_NOT_NULL((pkt_copy = pktbuf_start_write(pkt)));
    TEST_ASSERT(pkt == pkt_copy);
    pktbuf_release(pkt);
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_start_write__pkt_users_2(void)
{
    pktsnip_t *pkt = pktbuf_insert(TEST_STRING16, sizeof(TEST_STRING16)), *pkt_copy;
    pktbuf_hold(pkt);
    TEST_ASSERT_NOT_NULL((pkt_copy = pktbuf_start_write(pkt)));
    TEST_ASSERT(pkt != pkt_copy);
    TEST_ASSERT(pkt->next == pkt_copy->next);
    TEST_ASSERT_EQUAL_STRING(pkt->data, pkt_copy->data);
    TEST_ASSERT_EQUAL_INT(pkt->size, pkt_copy->size);
    TEST_ASSERT_EQUAL_INT(pkt->type, pkt_copy->type);
    TEST_ASSERT_EQUAL_INT(pkt->users, pkt_copy->users);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);

    pktbuf_release(pkt_copy);
    pktbuf_release(pkt);
    TEST_ASSERT(pktbuf_is_empty());
}

static void test_pktbuf_contains__NULL(void)
{
    TEST_ASSERT(!pktbuf_contains(NULL));
}

static void test_pktbuf_contains__external(void)
{
    TEST_ASSERT(!pktbuf_contains(TEST_STRING8));
}

static void test_pktbuf_contains__success(void)
{
    pktsnip_t *pkt = pktbuf_insert(TEST_STRING8, sizeof(TEST_STRING8));

    TEST_ASSERT(pktbuf_contains(pkt->data));
}

Test *tests_pktbuf_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_pktbuf_reset),
        new_TestFixture(test_pktbuf_alloc__size_0),
#if PKTBUF_SIZE > 0
        new_TestFixture(test_pktbuf_alloc__memfull),
        new_TestFixture(test_pktbuf_alloc__memfull2),
        new_TestFixture(test_pktbuf_alloc__memfull3),
#endif
        new_TestFixture(test_pktbuf_alloc__success),
        new_TestFixture(test_pktbuf_realloc_data__pkt_NULL),
        new_TestFixture(test_pktbuf_realloc_data__pkt_wrong),
        new_TestFixture(test_pktbuf_realloc_data__pkt_data_wrong),
        new_TestFixture(test_pktbuf_realloc_data__pkt_users_gt_1),
        new_TestFixture(test_pktbuf_realloc_data__pkt_next_neq_NULL),
        new_TestFixture(test_pktbuf_realloc_data__size_0),
#if PKTBUF_SIZE > 0
        new_TestFixture(test_pktbuf_realloc_data__memfull),
        new_TestFixture(test_pktbuf_realloc_data__memfull2),
        new_TestFixture(test_pktbuf_realloc_data__memfull3),
        new_TestFixture(test_pktbuf_realloc_data__nomemenough),
        new_TestFixture(test_pktbuf_realloc_data__shrink),
        new_TestFixture(test_pktbuf_realloc_data__memenough),
#endif
        new_TestFixture(test_pktbuf_realloc_data__success),
        new_TestFixture(test_pktbuf_realloc_data__success2),
        new_TestFixture(test_pktbuf_realloc_data__further_down_the_line),
        new_TestFixture(test_pktbuf_insert__size_0),
        new_TestFixture(test_pktbuf_insert__data_NULL),
        new_TestFixture(test_pktbuf_insert__success),
        new_TestFixture(test_pktbuf_insert__packed_struct),
        new_TestFixture(test_pktbuf_add_header__pkt_NULL__data_NULL__size_0),
        new_TestFixture(test_pktbuf_add_header__pkt_NOT_NULL__data_NULL__size_0),
        new_TestFixture(test_pktbuf_add_header__pkt_NULL__data_NOT_NULL__size_0),
        new_TestFixture(test_pktbuf_add_header__pkt_NOT_NULL__data_NOT_NULL__size_0),
        new_TestFixture(test_pktbuf_add_header__pkt_NULL__data_NULL__size_not_0),
        new_TestFixture(test_pktbuf_add_header__pkt_NOT_NULL__data_NULL__size_not_0),
        new_TestFixture(test_pktbuf_add_header__pkt_NOT_NULL__data_NOT_NULL__size_not_0),
        new_TestFixture(test_pktbuf_add_header__in_place),
        new_TestFixture(test_pktbuf_hold__pkt_null),
        new_TestFixture(test_pktbuf_hold__pkt_external),
        new_TestFixture(test_pktbuf_hold__success),
        new_TestFixture(test_pktbuf_release__pkt_null),
        new_TestFixture(test_pktbuf_release__pkt_external),
        new_TestFixture(test_pktbuf_release__success),
        new_TestFixture(test_pktbuf_start_write__NULL),
        new_TestFixture(test_pktbuf_start_write__pkt_users_1),
        new_TestFixture(test_pktbuf_start_write__pkt_users_2),
        new_TestFixture(test_pktbuf_contains__NULL),
        new_TestFixture(test_pktbuf_contains__external),
        new_TestFixture(test_pktbuf_contains__success),
    };

    EMB_UNIT_TESTCALLER(pktbuf_tests, NULL, tear_down, fixtures);

    return (Test *)&pktbuf_tests;
}

void tests_pktbuf(void)
{
    TESTS_RUN(tests_pktbuf_tests());
}
/** @} */
