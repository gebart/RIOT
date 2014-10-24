/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the ISL29020 light sensor
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#ifndef TEST_PDC8544_SPI
#error "TEST_PDC8544_SPI not defined"
#endif
#ifndef TEST_PDC8544_CS
#error "TEST_PDC8544_CS not defined"
#endif
#ifndef TEST_PDC8544_RESET
#error "TEST_PDC8544_RESET not defined"
#endif
#ifndef TEST_PDC8544_MODE
#error "TEST_PDC8544_MODE not defined"
#endif

#include <stdio.h>

#include "vtimer.h"
#include "pdc8544.h"

#define DELAY           (2 * 1000 * 1000U)

int main(void)
{
    pdc8544_t dev;

    puts("PDC8544 LCD display test application\n");
    printf("Initializing PDC8544 LCD at SPI_%i... ", TEST_PDC8544_SPI);
    if (pdc8544_init(&dev, TEST_PDC8544_SPI, TEST_PDC8544_CS,
                     TEST_PDC8544_RESET, TEST_PDC8544_MODE) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    pdc8544_print_riot_logo(&dev);

    while (1) {
        vtimer_usleep(DELAY);
        pdc8544_invert(&dev);
        vtimer_usleep(DELAY);
        pdc8544_set_normal(&dev);
    }

    return 0;
}
