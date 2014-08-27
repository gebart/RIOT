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

#include "ethernet.h"
#include "enc28j60.h"
#include "enc28j60_internal.h"
#include "enc28j60_regs.h"

static char rx_buffer[1500];        /* TODO: this should not be here at all -> just for testing */

int enc28j60_setup(enc28j60_dev_t *dev, spi_t spi, gpio_t cs_pin, gpio_t int_pin)
{
    dev->spi = spi;
    dev->cs = cs_pin;
    dev->int_pin = int_pin;
}

int enc28j60_init(net_dev_t *dev)
{
    enc28j60_dev_t *enc = (enc28j60_dev_t *)dev;

    /* setup the low-level interfaces */
    gpio_init_out(enc->cs, GPIO_PULLUP);
    gpio_init_int(enc->int_pin, GPIO_PULLUP, GPIO_FALLING, enc28j60_on_int, dev);
    spi_init_master(enc->spi, SPI_CONF_FIRST_RISING, ENC28J60_SPI_SPEED);

    /* setup the device itself */
    enc28j60_init_device(enc);
}


int enc28j60_send_data(net_dev_t *dev,
              void *dest,
              size_t dest_len,
              net_dev_hlist_t *upper_layer_hdrs,
              void *data,
              size_t data_len)
{
    uint16_t e_type;
    enc28j60_dev_t *enc = (enc28j60_dev_t *)dev;

    /* read type of data -> IPv6? */
    type = ETHERNET_TYPE_IPV6;      /* TODO: read this from upper_layer_hdrs */

    enc28j60_transmit(enc, data, data_len, type, (char*)dest);  /* TODO: dst_mac from dest? */

    return data_len;
}

int enc28j60_add_rcv_cb(net_dev_t *dev, net_dev_rcv_data_cb_t cb)
{
    enc28j60_dev_t *enc = (enc28j60_dev_t *)dev;

    enc->rx_cb = cb;
    return 0;
}

int enc28j60_rem_rcv_cb(net_dev_t *dev, net_dev_rcv_data_cb_t cb)
{
    enc28j60_dev_t *enc = (enc28j60_dev_t *)dev;

    if (enc->rx_cb == cb) {
        enc->rx_cb = 0;
        return 0;
    }
    return -1;
}

int enc28j60_get_option(net_dev_t *dev, net_dev_opt_t opt, void *value, size_t value_len)
{
    enc28j60_dev_t *enc = (enc28j60_dev_t *)dev;

}

int enc28j60_set_option(net_dev_t *dev, net_dev_opt_t opt, void *value, size_t value_len)
{
    enc28j60_dev_t *enc = (enc28j60_dev_t *)dev;

}

int enc28j60_get_state(net_dev_t *dev, net_dev_state_t *state)
{
    enc28j60_dev_t *enc = (enc28j60_dev_t *)dev;

}

int enc28j60_set_state(net_dev_t *dev, net_dev_state_t state)
{
    enc28j60_dev_t *enc = (enc28j60_dev_t *)dev;

}

void enc28j60_event(net_dev_t *dev, uint32_t event_type)
{
    enc28j60_dev_t *enc = (enc28j60_dev_t *)dev;
    char src_mac[6];
    uint16_t ethernet_type;
    int length;

    switch (event_type) {
        case ENC28J60_EVT_RX:
            length = end28j60_receive(enc, rx_buffer, 1500, src_mac, dst_mac, &ethernet_type);
            dev->rx_cb(dev, src_mac, 6, dst_mac, 6, rx_buffer, length);
            break;
    }
}

net_dev_driver_t enc28j60 = {
    enc28j60_init,
    enc28j60_send_data,
    enc28j60_add_rcv_cb,
    enc28j60_rem_rcv_cb,
    enc28j60_get_option,
    enc28j60_set_option,
    enc28j60_get_state,
    enc28j60_set_state,
    enc28j60_event
};
