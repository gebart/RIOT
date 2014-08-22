/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_enc28j60
 * @{
 *
 * @file
 * @brief       Implementation of ENC28J60 driver interfaces
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "enc28j60.h"
#include "enc28j60_internal.h"
#include "enc28j60_regs.h"



int enc28j60_setup(enc28j60_dev_t *dev, spi_t spi, gpio_t cs_pin, gpio_t int_pin)
{
    dev->spi = spi;
    dev->cs = cs_pin;
    dev->int_pin = int_pin;
}

int enc28j60_init(net_dev_t *dev)
{
    /* setup the low-level interfaces */
    gpio_init_out(dev->cs, GPIO_PULLUP);
    gpio_init_int(dev->int_pin, GPIO_PULLUP, GPIO_FALLING, enc28j60_on_int, dev);
    spi_init_master(dev->spi, SPI_CONF_FIRST_RISING, ENC28J60_SPI_SPEED);

    /* setup the device itself */
}


int send_data(net_dev_t *dev,
              void *dest,
              size_t dest_len,
              net_dev_hlist_t *upper_layer_hdrs,
              void *data,
              size_t data_len)
{

}

int enc28j60_set_option(net_dev_t, net_dev_opt_t opt, void *value, size_t value_len)
{

}
