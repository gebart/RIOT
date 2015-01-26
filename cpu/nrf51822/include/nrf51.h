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

/**
 * @brief Define the size of the receive buffer used in the driver (number of packets)
 */
#define NRF51_RX_BUFSIZE                (3U)

/**
 * @name Pre-configure the radio with these values
 * @{
 */
#define NRF51_CONF_LEN                  (8U)
#define NRF51_CONF_S0                   (0U)
#define NRF51_CONF_S1                   (0U)
#define NRF51_CONF_MAX_PAYLOAD_LENGTH   (256U)
#define NRF51_CONF_STATLEN              (0U)
#define NRF51_CONF_BASE_ADDR_LEN        (4U)
#define NRF51_CONF_ENDIAN               RADIO_PCNF1_ENDIAN_Big
#define NRF51_CONF_WHITENING            RADIO_PCNF1_WHITEEN_Disabled
#define NRF51_CONF_TXPOWER              NRF51_TXPWR_0DBM
#define NRF51_CONF_CHANNEL              (7U)        /* -> 2407MHz */
#define NRF51_CONF_MODE                 RADIO_MODE_MODE_Nrf_2Mbit
#define NRF51_CONF_CRC_LEN              (2U)        /* 1 to 3 bytes */
#define NRF51_CONF_CRC_POLY             (0x11021UL) /* x^16 + x^12 + x^5 + 1 */
/** @} */

/**
 * @name Pre-configured addresses, change with caution
 * @{
 */
#define NRF51_CONF_PREFIX0              (0xC4C3C2E7UL)
#define NRF51_CONF_PREFIX1              (0xC5C6C7C8UL)
#define NRF51_CONF_BASE0                (0xE7E70000UL)
#define NRF51_CONF_BASE1                (0xC2C20000UL)
#define NRF51_CONF_ADDR_BROADCAST       (0U)
#define NRF51_CONF_ADDR_OWN             (23U)
/** @} */

/**
 * @brief States the device can be put into
 */
typedef enum {
    NRF51_STATE_OFF = 0,            /**< turn the radio off */
    NRF51_STATE_RX =  1,            /**< listen for incoming packets */
} nrf51prop_state_t;

/**
 * @brief Possible transmit power values, ranging from +4dBm to -30dBm
 */
typedef enum {
    NRF51_TXPWR_P4DBM     = 0x0f,   /**< transmit with  +4dBm */
    NRF51_TXPWR_0DBM      = 0x00,   /**< transmit with   0dBm */
    NRF51_TXPWR_M4DBM     = 0xfc,   /**< transmit with  -4dBm */
    NRF51_TXPWR_M8DBM     = 0xf8,   /**< transmit with  -8dBm */
    NRF51_TXPWR_M12DBM    = 0xf4,   /**< transmit with -12dBm */
    NRF51_TXPWR_M16DBM    = 0xf0,   /**< transmit with -16dBm */
    NRF51_TXPWR_M20DBM    = 0xec,   /**< transmit with -20dBm */
    NRF51_TXPWR_M30DBM    = 0xd8    /**< transmit with -30dBm */
} nrf51prop_txpower_t;

/**
 * @brief The in-memory structure of an NRF51 radio packet
 */
typedef struct __attribute__((packed)) {
    uint8_t length;                 /**< length field (can be optional as well) */
    uint8_t payload[NRF51_CONF_MAX_PAYLOAD_LENGTH];     /**< actual payload of the packet */
} nrf51prop_packet_t;

/**
 * @brief Device descriptor for the NRF51 proprietary radio implementation
 */
typedef struct __attribute__ ((packed)) {
    uint8_t rx_buf_next;                /**< pointer to free rx buffer */
    uint8_t state;                      /**< the current state of the device */
    uint16_t own_addr;                  /**< configured 16-bit RX address */
    netdev_event_cb_t event_cb;         /**< netdev event callback */
    nrf51prop_packet_t tx_buf;          /**< transmission buffer */
    nrf51prop_packet_t rx_buf[2];       /**< double buffered RX buffer */
} nrf51prop_t;

/**
 * @brief Initialize the NRF51 radio
 *
 * The initialization uses static configuration values.
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int nrf51prop_init(void);

/**
 * @brief Send data using the radio
 *
 * @param[in] addr          address to send the data to
 * @param[in] data          pointer to the data to send
 * @param[in] length        number of bytes to send
 *
 * @return                  number of bytes transferred
 */
int nrf51prop_send(pktsnip_t *snip);

/**
 * @brief Set the state of the radio
 *
 * Possible values are
 * - NRF_51_STATE_OFF: device is in stand-by
 * - NRF_51_STATE_RX:  radio listens for incoming packets
 *
 * @param[in] state         targeted state
 *
 * @return                  the previous state
 * @return                  -1 on error
 */
int nrf51prop_set_state(nrf51prop_state_t new_state);

/**
 * @brief Get the current state of the radio
 *
 * @return                  current state of the radio
 */
nrf51prop_state_t nrf51prop_get_state(void);

/**
 * @brief Set the radio's address
 *
 * @param[in] address       new address
 *
 * @return                  address that was set
 * @return                  -1 on error
 */
int nrf51prop_set_address(uint16_t address);

/**
 * @brief Get the current address
 *
 * @return                  current address
 */
uint16_t nrf51prop_get_address(void);

/**
 * @brief Set the address on which the radio listens for broadcast messages
 *
 * @param[in] address       broadcast address
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int nrf51prop_set_broadcast_address(uint16_t address);

/**
 * @brief Get the radio's broadcast address
 *
 * @return                  broadcast address
 */
uint16_t nrf51prop_get_broadcast_address(void);

/**
 * @brief Set the radio channel to use
 *
 * The actual used frequency is 2400 + channel MHz
 *
 * @param[in] channel       channel to set
 *
 * @return                  new channel
 * @return                  -1 on error
 */
int nrf51prop_set_channel(uint8_t channel);

/**
 * @brief Get the currently configured channel
 *
 * @return                  the current channel
 */
int nrf51prop_get_channel(void);

/**
 * @brief Set transmit power
 *
 * @param[in] power         power to use when transmitting
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int nrf51prop_set_txpower(nrf51prop_txpower_t power);

/**
 * @brief Get the currently set transmit power
 *
 * @return                  configured transmit power
 */
nrf51prop_txpower_t nrf51prop_get_txpower(void);

/**
 * @brief Power on the radio
 */
void nrf51prop_poweron(void);

#endif /* __NRF_RADIO_H */
/** @} */
