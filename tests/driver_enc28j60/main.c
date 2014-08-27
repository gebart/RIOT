/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test input and output functionality of low-level GPIO driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "cpu.h"
#include "periph_conf.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "enc28j60.h"

/* for this test there must be at leas 1 SPI and 2 GPIO devices defined */
#if SPI_NUMOF || GPIO_NUMOF

int main(void)
{
    enc28j60_t dev;

    puts("Test for the ENC28J60 Ethernet module driver");

    /* set peripherals to use */
    enc28j60_setup(&dev, SPI_0, GPIO_5, GPIO_6);

    /* initialize the module */
    enc28j60_init((net_dev_t *)&dev);


    while (1) {
        /* nothing so far to do here */
    }

    return 0;
}

#else

int main(void)
{
    puts("No low-level GPIO device defined for this platform.");

    return 0;
}

#endif /* SPI_NUMOF || GPIO_NUMOF */
