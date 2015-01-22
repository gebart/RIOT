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

#include "embUnit/embUnit.h"
#include "pkt.h"

#include "tests-pkt.h"

#define _INIT_ELEM(len, data, next) \
    { (next), (data), (len), PKT_PROTO_UNKNOWN }
#define _INIT_ELEM_STATIC_DATA(data, next) _INIT_ELEM(sizeof(data), data, next)

#define TEST_PKTSIZE    (TEST_UINT16)

static void test_pktsnip_advance__NULL(void)
{
    pktsnip_t **pkt = NULL;

    pktsnip_advance(pkt);

    TEST_ASSERT_NULL(pkt);
}

static void test_pktsnip_advance__ptr_NULL(void)
{
    pktsnip_t *pkt = NULL;

    pktsnip_advance(&pkt);

    TEST_ASSERT_NULL(pkt);
}

static void test_pktsnip_advance__1_elem(void)
{
    pktsnip_t snip = _INIT_ELEM(0, NULL, NULL);
    pktsnip_t *pkt = &snip;

    pktsnip_advance(&pkt);

    TEST_ASSERT_NULL(pkt);
}

static void test_pktsnip_advance__2_elem(void)
{
    pktsnip_t snip1 = _INIT_ELEM(0, NULL, NULL);
    pktsnip_t snip2 = _INIT_ELEM(0, NULL, &snip1);
    pktsnip_t *pkt = &snip2;

    pktsnip_advance(&pkt);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT(&snip1 == pkt);

    pktsnip_advance(&pkt);

    TEST_ASSERT_NULL(pkt);
}

static void test_pktsnip_num__NULL(void)
{
    TEST_ASSERT_EQUAL_INT(0, pktsnip_num(NULL));
}

static void test_pktsnip_num__1_elem(void)
{
    pktsnip_t snip = _INIT_ELEM(0, NULL, NULL);

    TEST_ASSERT_EQUAL_INT(1, pktsnip_num(&snip));
}

static void test_pktsnip_num__2_elem(void)
{
    pktsnip_t snip1 = _INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    pktsnip_t snip2 = _INIT_ELEM(0, NULL, &snip1);

    TEST_ASSERT_EQUAL_INT(2, pktsnip_num(&snip2));
    TEST_ASSERT_EQUAL_INT(1, pktsnip_num(&snip1));
}

static void test_pktsnip_num__3_elem(void)
{
    pktsnip_t snip1 = _INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    pktsnip_t snip2 = _INIT_ELEM_STATIC_DATA(TEST_STRING12, &snip1);
    pktsnip_t snip3 = _INIT_ELEM(0, NULL, &snip2);

    TEST_ASSERT_EQUAL_INT(3, pktsnip_num(&snip3));
    TEST_ASSERT_EQUAL_INT(2, pktsnip_num(&snip2));
    TEST_ASSERT_EQUAL_INT(1, pktsnip_num(&snip1));
}

static void test_pktsnip_add__pkt_ptr_NULL__snip_NULL(void)
{
    pktsnip_add(NULL, NULL);
}

static void test_pktsnip_add__pkt_NULL__snip_NULL(void)
{
    pktsnip_t *pkt = NULL;

    pktsnip_add(&pkt, NULL);

    TEST_ASSERT_NULL(pkt);
}

static void test_pktsnip_add__pkt_ptr_NULL(void)
{
    pktsnip_t snip = _INIT_ELEM(0, NULL, NULL);

    pktsnip_add(NULL, &snip);

    TEST_ASSERT_NULL(snip.next);
}

static void test_pktsnip_add__pkt_empty(void)
{
    pktsnip_t *pkt = NULL;
    pktsnip_t snip = _INIT_ELEM(0, NULL, NULL);

    pktsnip_add(&pkt, &snip);

    TEST_ASSERT(&snip == pkt);
    TEST_ASSERT_NULL(pkt->next);
}

static void test_pktsnip_add__2_elem(void)
{
    pktsnip_t snip1 = _INIT_ELEM(0, NULL, NULL);
    pktsnip_t snip2 = _INIT_ELEM(0, NULL, NULL);
    pktsnip_t *pkt = NULL;

    pktsnip_add(&pkt, &snip1);

    TEST_ASSERT(&snip1 == pkt);
    TEST_ASSERT_NULL(pkt->next);

    pktsnip_add(&pkt, &snip2);

    TEST_ASSERT(&snip1 == pkt);
    TEST_ASSERT(&snip2 == pkt->next);
    TEST_ASSERT_NULL(pkt->next->next);
}

static void test_pktsnip_add__3_elem(void)
{
    pktsnip_t snip1 = _INIT_ELEM(0, NULL, NULL);
    pktsnip_t snip2 = _INIT_ELEM(0, NULL, NULL);
    pktsnip_t snip3 = _INIT_ELEM(0, NULL, NULL);
    pktsnip_t *pkt = NULL;

    pktsnip_add(&pkt, &snip1);
    pktsnip_add(&pkt, &snip2);
    pktsnip_add(&(pkt->next), &snip3);

    TEST_ASSERT(&snip1 == pkt);
    TEST_ASSERT(&snip2 == pkt->next);
    TEST_ASSERT(&snip3 == pkt->next->next);
    TEST_ASSERT_NULL(pkt->next->next->next);
}

static void test_pktsnip_remove__pkt_ptr_NULL__snip_NULL(void)
{
    pktsnip_remove(NULL, NULL);
}

static void test_pktsnip_remove__pkt_NULL__snip_NULL(void)
{
    pktsnip_t *pkt = NULL;

    pktsnip_remove(&pkt, NULL);

    TEST_ASSERT_NULL(pkt);
}

static void test_pktsnip_remove__pkt_ptr_NULL(void)
{
    pktsnip_t snip = _INIT_ELEM(0, NULL, NULL);

    pktsnip_remove(NULL, &snip);

    TEST_ASSERT_NULL(snip.next);
}

static void test_pktsnip_remove__pkt_empty(void)
{
    pktsnip_t *pkt = NULL;
    pktsnip_t snip = _INIT_ELEM(0, NULL, NULL);

    pktsnip_remove(&pkt, &snip);

    TEST_ASSERT_NULL(pkt);
    TEST_ASSERT_NULL(snip.next);
}

static void test_pktsnip_remove__snip_NULL(void)
{
    pktsnip_t snip1 = _INIT_ELEM(0, NULL, NULL);
    pktsnip_t snip2 = _INIT_ELEM(0, NULL, &snip1);
    pktsnip_t *pkt = &snip2;

    pktsnip_remove(&pkt, NULL);

    TEST_ASSERT(pkt == &snip2);
    TEST_ASSERT_NOT_NULL(pkt->next);
    TEST_ASSERT(pkt->next == &snip1);
    TEST_ASSERT_NULL(pkt->next->next);
}

static void test_pktsnip_remove__1st_snip_first(void)
{
    pktsnip_t snip1 = _INIT_ELEM(0, NULL, NULL);
    pktsnip_t snip2 = _INIT_ELEM(0, NULL, &snip1);
    pktsnip_t *pkt = &snip2;

    pktsnip_remove(&pkt, &snip2);

    TEST_ASSERT(pkt == &snip1);
    TEST_ASSERT_NULL(pkt->next);

    pktsnip_remove(&pkt, &snip1);

    TEST_ASSERT_NULL(pkt);
}

static void test_pktsnip_remove__nth_snip_first(void)
{
    pktsnip_t snip1 = _INIT_ELEM(0, NULL, NULL);
    pktsnip_t snip2 = _INIT_ELEM(0, NULL, &snip1);
    pktsnip_t *pkt = &snip2;

    pktsnip_remove(&pkt, &snip1);

    TEST_ASSERT(pkt == &snip2);
    TEST_ASSERT_NULL(pkt->next);

    pktsnip_remove(&pkt, &snip2);

    TEST_ASSERT_NULL(pkt);
}

static void test_pkt_len__NULL(void)
{
    TEST_ASSERT_EQUAL_INT(0, pkt_len(NULL));
}

static void test_pkt_len__1_elem__size_MAX(void)
{
    pktsnip_t snip = _INIT_ELEM(PKTSIZE_MAX, NULL, NULL);

    TEST_ASSERT_EQUAL_INT(PKTSIZE_MAX, pkt_len(&snip));
}

static void test_pkt_len__1_elem__size_0(void)
{
    pktsnip_t snip = _INIT_ELEM(0, NULL, NULL);

    TEST_ASSERT_EQUAL_INT(0, pkt_len(&snip));
}

static void test_pkt_len__1_elem__size_data(void)
{
    pktsnip_t snip = _INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);

    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt_len(&snip));
}

static void test_pkt_len__2_elem(void)
{
    pktsnip_t snip1 = _INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    pktsnip_t snip2 = _INIT_ELEM_STATIC_DATA(TEST_STRING12, &snip1);

    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8) + sizeof(TEST_STRING12),
                          pkt_len(&snip2));
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt_len(&snip1));
}

static void test_pkt_len__2_elem__overflow(void)
{
    pktsnip_t snip1 = _INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    pktsnip_t snip2 = _INIT_ELEM(PKTSIZE_MAX, NULL, &snip1);

    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8) - 1, pkt_len(&snip2));
    /* size should overflow */
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt_len(&snip1));
}

static void test_pkt_len__3_elem(void)
{
    pktsnip_t snip1 = _INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    pktsnip_t snip2 = _INIT_ELEM_STATIC_DATA(TEST_STRING12, &snip1);
    pktsnip_t snip3 = _INIT_ELEM(sizeof("a"), "a", &snip2);

    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8) + sizeof(TEST_STRING12) + sizeof("a"),
                          pkt_len(&snip3));
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8) + sizeof(TEST_STRING12), pkt_len(&snip2));
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt_len(&snip1));
}

Test *tests_pkt_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_pktsnip_advance__NULL),
        new_TestFixture(test_pktsnip_advance__ptr_NULL),
        new_TestFixture(test_pktsnip_advance__1_elem),
        new_TestFixture(test_pktsnip_advance__2_elem),
        new_TestFixture(test_pktsnip_num__NULL),
        new_TestFixture(test_pktsnip_num__1_elem),
        new_TestFixture(test_pktsnip_num__2_elem),
        new_TestFixture(test_pktsnip_num__3_elem),
        new_TestFixture(test_pktsnip_add__pkt_ptr_NULL__snip_NULL),
        new_TestFixture(test_pktsnip_add__pkt_NULL__snip_NULL),
        new_TestFixture(test_pktsnip_add__pkt_ptr_NULL),
        new_TestFixture(test_pktsnip_add__pkt_empty),
        new_TestFixture(test_pktsnip_add__2_elem),
        new_TestFixture(test_pktsnip_add__3_elem),
        new_TestFixture(test_pktsnip_remove__pkt_ptr_NULL__snip_NULL),
        new_TestFixture(test_pktsnip_remove__pkt_NULL__snip_NULL),
        new_TestFixture(test_pktsnip_remove__pkt_ptr_NULL),
        new_TestFixture(test_pktsnip_remove__pkt_empty),
        new_TestFixture(test_pktsnip_remove__snip_NULL),
        new_TestFixture(test_pktsnip_remove__1st_snip_first),
        new_TestFixture(test_pktsnip_remove__nth_snip_first),
        new_TestFixture(test_pkt_len__NULL),
        new_TestFixture(test_pkt_len__1_elem__size_MAX),
        new_TestFixture(test_pkt_len__1_elem__size_0),
        new_TestFixture(test_pkt_len__1_elem__size_data),
        new_TestFixture(test_pkt_len__2_elem),
        new_TestFixture(test_pkt_len__2_elem__overflow),
        new_TestFixture(test_pkt_len__3_elem),
    };

    EMB_UNIT_TESTCALLER(pkt_tests, NULL, NULL, fixtures);

    return (Test *)&pkt_tests;
}

void tests_pkt(void)
{
    TESTS_RUN(tests_pkt_tests());
}
/** @} */
