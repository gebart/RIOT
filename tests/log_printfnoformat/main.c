/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief       Test logging with no format gives the expected output
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 */

#include <stdint.h>

#include "log.h"

#define TEST_FORMAT "Logging value %d and string %s"

int main(void)
{
    uint8_t value = 42;
    const char *string = "test";

    LOG_ERROR(TEST_FORMAT, value, string);
    LOG_WARNING(TEST_FORMAT, value, string);
    LOG_INFO(TEST_FORMAT, value, string);
    LOG_DEBUG(TEST_FORMAT, value, string);

    return 0;
}
