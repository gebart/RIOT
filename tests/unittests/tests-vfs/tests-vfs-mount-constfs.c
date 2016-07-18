/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief vfs_mount related unit tests
 */
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "embUnit/embUnit.h"

#include "vfs.h"
#include "fs/constfs.h"

#include "tests-vfs.h"

static const uint8_t bin_data[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
    0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};
static const uint8_t str_data[] = "This is a test file";
                                /* 01234567890123456789 */
                                /* 0         1          */
static const constfs_file_t _files[] = {
    {
        .path = "/test.txt",
        .data = str_data,
        .size = 20,
    },
    {
        .path = "/data.bin",
        .data = bin_data,
        .size = 32,
    },
};

static const constfs_t fs = {
    .files = _files,
    .nfiles = sizeof(_files) / sizeof(_files[0]),
};

static void test_vfs_mount_umount(void)
{
    int md = vfs_mount(&constfs_file_system, "/test", (void *)&fs);
    TEST_ASSERT(md >= 0);
    int res = vfs_umount(md);
    TEST_ASSERT(res >= 0);
}

static void test_vfs_umount__invalid_md(void)
{
    int res;
    res = vfs_umount(-1);
    TEST_ASSERT(res < 0);
    res = vfs_umount(VFS_MAX_MOUNTS);
    TEST_ASSERT(res < 0);
}

static void test_vfs_constfs_open(void)
{
    int res;
    int md = vfs_mount(&constfs_file_system, "/test", (void *)&fs);
    TEST_ASSERT(md >= 0);

    int fd;
    fd = vfs_open("/test/notfound", O_RDONLY, 0);
    TEST_ASSERT(fd == -ENOENT);
    if (fd >= 0) {
        vfs_close(fd);
    }
    fd = vfs_open("/test/test.txt", O_WRONLY, 0);
    TEST_ASSERT(fd == -EROFS);
    if (fd >= 0) {
        vfs_close(fd);
    }
    fd = vfs_open("/test/test.txt", O_RDWR, 0);
    TEST_ASSERT(fd == -EROFS);
    if (fd >= 0) {
        vfs_close(fd);
    }
    fd = vfs_open("/test/test.txt", O_RDONLY, 0);
    TEST_ASSERT(fd >= 0);
    if (fd >= 0) {
        res = vfs_close(fd);
        TEST_ASSERT_EQUAL_INT(0, res);
    }

    res = vfs_umount(md);
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void test_vfs_constfs_read(void)
{
    int res;
    int md = vfs_mount(&constfs_file_system, "/test//", (void *)&fs);
    TEST_ASSERT(md >= 0);

    int fd = vfs_open("/test/test.txt", O_RDONLY, 0);
    TEST_ASSERT(fd >= 0);

    char strbuf[64];
    memset(strbuf, '\0', sizeof(strbuf));
    ssize_t nbytes;
    nbytes = vfs_read(fd, strbuf, sizeof(strbuf));
    TEST_ASSERT_EQUAL_INT(sizeof(str_data), nbytes);
    TEST_ASSERT_EQUAL_STRING((const char *)&str_data[0], (const char *)&strbuf[0]);

    res = vfs_fcntl(fd, F_GETFL, 0);
    TEST_ASSERT_EQUAL_INT(O_RDONLY, res);

    res = vfs_close(fd);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_umount(md);
    TEST_ASSERT_EQUAL_INT(0, res);
}

#if MODULE_NEWLIB
static void test_vfs_constfs__posix(void)
{
    int res;
    int md = vfs_mount(&constfs_file_system, "/test", (void *)&fs);
    TEST_ASSERT(md >= 0);

    int fd = open("/test/test.txt", O_RDONLY, 0);
    TEST_ASSERT(fd >= 0);

    char strbuf[64];
    memset(strbuf, '\0', sizeof(strbuf));
    ssize_t nbytes;
    nbytes = read(fd, strbuf, sizeof(strbuf));
    TEST_ASSERT_EQUAL_INT(sizeof(str_data), nbytes);
    TEST_ASSERT_EQUAL_STRING((const char *)&str_data[0], (const char *)&strbuf[0]);

#if HAVE_FCNTL
    /* fcntl support is optional in newlib */
    res = fcntl(fd, F_GETFL, 0);
    TEST_ASSERT_EQUAL_INT(O_RDONLY, res);
#endif

    res = close(fd);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_umount(md);
    TEST_ASSERT_EQUAL_INT(0, res);
}
#endif

Test *tests_vfs_mount_constfs_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_vfs_mount_umount),
        new_TestFixture(test_vfs_umount__invalid_md),
        new_TestFixture(test_vfs_constfs_open),
        new_TestFixture(test_vfs_constfs_read),
#if MODULE_NEWLIB
        new_TestFixture(test_vfs_constfs__posix),
#endif
    };

    EMB_UNIT_TESTCALLER(vfs_mount_tests, NULL, NULL, fixtures);

    return (Test *)&vfs_mount_tests;
}

/** @} */
