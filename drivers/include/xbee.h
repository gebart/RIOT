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
    char tx_mem[XBEE_FRAME_SIZE];   /**< transmit data buffer */
    ringbuffer_t tx_buf;            /**< transmit ringbuffer */
    mutex_t response_lock;          /**< lock for synchronizing AT command
                                         responses */
    xbee_at_response_t at_response; /**< last AT command response */
    mutex_t tx_report_lock;         /**< lock for synchronizing TX reports */
    xbee_tx_report_t tx_report;     /**< last TX report */
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
