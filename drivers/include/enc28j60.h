/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    driver_enc28j60 ENC28J60
 * @ingroup     drivers
 * @brief       Driver for the ENC28J60 Ethernet Adapter
 * @{
 *
 * @file
 * @brief       Interface definition for the ENC28J60 driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __ENC28J60_H
#define __ENC28J60_H

typedef struct {
    spi_t spi;
    gpio_t cs;
    gpio_t int_pin;
    short active_bank;
} enc28j60_dev_t;


int enc28j60_setup(enc28j60_dev_t *dev, spi_t spi, gpio_t cs_pin, gpio_t int_pin);

int enc28j60_init(net_dev_t *dev);

int enc28j60_send_data(net_dev_t *dev,
              void *dest,
              size_t dest_len,
              net_dev_hlist_t *upper_layer_hdrs,
              void *data,
              size_t data_len);

int enc28j60_add_rcv_cb(net_dev_t *dev, net_dev_rcv_data_cb_t cb);

int enc28j60_rem_rcv_cb(net_dev_t *dev, net_dev_rcv_data_cb_t cb);

int enc28j60_get_option(net_dev_t, net_dev_opt_t opt, void *value, size_t *value_len);

int enc28j60_set_option(net_dev_t, net_dev_opt_t opt, void *value, size_t value_len);

int enc28j60_get_state(net_dev_t *dev, net_dev_state_t *state);

int enc28j60_set_state(net_dev_t *dev, net_dev_state_t state);

#endif /* __ENC28J60_H */
/** @} */
