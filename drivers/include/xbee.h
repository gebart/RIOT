/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_xbee XBee driver
 * @ingroup     drivers
 * @brief       High-level driver for the XBee 802.15.4 modem
 * @{
 *
 * @file
 * @brief       High-level driver for the XBee 802.15.4 modem
 *
 * @author      Kévin Roussel <kevin.roussel@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef XBEE_H_
#define XBEE_H_

#include <stdint.h>

#include "kernel.h"
#include "mutex.h"
#include "ringbuffer.h"
#include "periph/uart.h"
#include "periph/gpio.h"
#include "net/ng_netdev.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief   The maximum payload length that can be send
 */
#define XBEE_MAX_PKT_DATA_LENGTH    (100U)


/**
 * @brief   Maximum number of bytes that can be send to the Xbee device in a
 *          single transaction
 */
#define XBEE_FRAME_SIZE             (256U)

/* We implement a finite state machine (FSM) to handle the incoming data,
   reconstruct the packets, extract their payload, and handle its
   processing and/or forward it to the upper layers.
   The following enum lits the states of this FSM. */
typedef enum {
    XBEE_RX_STATE_IDLE,                /* no data stored for now */
    XBEE_RX_STATE_SIZE1,               /* waiting for the first byte (MSB)
                                     of the size of the packet to come */
    XBEE_RX_STATE_SIZE2,               /* waiting for the second byte (LSB)
                                     of the size of the packet to come */
    XBEE_RX_STATE_DATA,             /* a packet (whose size we know) has begun
                                     to arrive, but isn't complete yet */
    // XBEE_RX_STATE_DATA,     /* we now have a complete packet,
                                     // wait for checksum byte */
} xbee_rx_state_t;

/**
 * @brief   XBee device descriptor
 */
typedef struct {
    /* netdev fields */
    ng_netdev_driver_t *driver;     /**< pointer to the devices interface */
    ng_netdev_event_cb_t event_cb;  /**< netdev event callback */
    kernel_pid_t mac_pid;           /**< the driver's thread's PID */
    /* device driver specific fields */
    uart_t uart;                    /**< UART interfaced used */
    gpio_t sleep_pin;               /**< GPIO pin connected to SLEEP */
    gpio_t status_pin;              /**< GPIO pin for reading the status */
    uint8_t options;                /**< options field */
    uint8_t frame_id;               /**< next ID for sent frames */
    mutex_t tx_lock;                /**< lock for writing to the device */
    mutex_t inner_tx_lock;          /**< lock for blocks of TX data */
    uint8_t *tx_buf;                 /**< transmit data buffer */
    uint16_t tx_count;              /**< counter for ongoing transmission */
    uint16_t tx_limit;              /**< number of bytes to transmit */
    uint8_t tx_cksum;               /**< checksum for transmitted data */
    xbee_rx_state_t rx_state;       /**< current state in the RX state machine */
    uint16_t rx_len;
    uint8_t rx_cksum;

} xbee_t;

/**
 * @brief   Reference to the XBee driver interface
 */
extern const ng_netdev_driver_t xbee_driver;

/**
 * @brief   Initialize the given Xbee device
 *
 * @param[out] dev          Xbee device to initialize
 * @param[in]  uart         UART interfaced the device is connected to
 * @param[in]  baudrate     baudrate to use
 * @param[in]  sleep_pin    GPIO pin that is connected to the SLEEP pin, set to
 *                          GPIO_NUMOF if not used
 * @param[in]  status_pin   GPIO pin that is connected to the STATUS pin, set to
 *                          GPIO_NUMOR if not used
 *
 * @return                  0 on success
 * @return                  -ENODEV on invalid device descriptor
 * @return                  -ENXIO on invalid UART
 */
int xbee_init(xbee_t *dev, uart_t uart, uint32_t baudrate,
              gpio_t sleep_pin, gpio_t status_pin);

#ifdef __cplusplus
}
#endif

#endif /* XBEE_H_ */
/** @} */
