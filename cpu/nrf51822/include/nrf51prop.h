/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        driver_nrf51 NRF51 radio
 * @ingroup         drivers
 * @brief           Driver for the NRF51 radio included in the NRF51822 SoC
 * @{
 *
 * @file
 * @brief           Interface definition for the NRF51 radio
 *
 * The current implementation of the radio driver only implements a simple transfer mechanism
 * without MAC and ACKing. It uses 16-bit addresses.
 *
 * Known issues:
 * - source addresses are not recorded
 * - no over-the-air data frame format is used, just plain data is send
 * - no MAC layer functions: pre-loading, medium busy test...
 * - no netdev interface implemented, yet
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __NRF_RADIO_H
#define __NRF_RADIO_H

#include <stdint.h>

#include "kernel_types.h"
#include "netdev.h"


#define NRF51_CONF_MAX_PAYLOAD_LENGTH   (256U)


/**
 * @brief   The in-memory structure of an NRF51 radio packet
 */
typedef struct __attribute__((packed)) {
    uint8_t length;                 /**< length field (can be optional as well) */
    uint16_t src_addr;              /**< source address of the packet */
    uint8_t payload[NRF51_CONF_MAX_PAYLOAD_LENGTH];     /**< actual payload of the packet */
} nrf51prop_packet_t;

/**
 * @brief   Netdev compatible device descriptor for the NRF51 proprietary radio implementation
 */
typedef struct __attribute__ ((packed)) {
    /* netdev interface */
    netdev_driver_t *driver;            /**< pointer to the drivers interface */
    netdev_event_cb_t event_cb;         /**< netdev event callback */
    kernel_pid_t mac_pid;               /**< PID of the thread, the driver is running in */
    /* driver specific configuration */
    uint8_t rx_buf_next;                /**< pointer to free RX buffer */
    volatile uint8_t state;             /**< the current state of the device */
    uint8_t old_state;                  /**< saves the old state before sending */
    uint16_t own_addr;                  /**< configured 16-bit RX address */
    uint16_t options;                   /**< bitfiels to save run-time options */
    nrf51prop_packet_t tx_buf;          /**< transmission buffer */
    nrf51prop_packet_t rx_buf[2];       /**< double buffered RX buffer */
} nrf51prop_t;

/**
 * @brief   Initialize the NRF51 radio
 *
 * The initialization uses static configuration values.
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int nrf51prop_init(nrf51prop_t *dev);

#endif /* __NRF_RADIO_H */
/** @} */
