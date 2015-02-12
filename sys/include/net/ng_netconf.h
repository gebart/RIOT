/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_netconf Configuration options for network APIs
 * @ingroup     net
 * @brief       List of available configuration options for network APIs
 * @{
 *
 * @file
 * @brief       Definition of global configuration options for netdev and netapi
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NG_NET_CONF_H_
#define NG_NET_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Global list of configuration options available throughout the
 *          network stack, e.g. by netdev and netapi
 */
typedef enum {
    NETCONF_OPT_CHANNEL,            /**< get/set channel as uint16_t in host
                                         byte order */
    NETCONF_OPT_IS_CHANNEL_CLR,     /**< check if channel is clear */
    NETCONF_OPT_ADDRESS,            /**< get/set link layer address in host byte
                                         order */
    NETCONF_OPT_ADDRESS_LONG,       /**< long link layer address in host byte
                                         order (e.g. IEEE802.15.4 EUI-64) */
    NETCONF_OPT_ADDR_LEN,           /**< get the default address length a
                                         network device expects as uint16_t in
                                         host byte order */
    NETCONF_OPT_NID,                /**< get/set the network ID (e.g. PAN ID in
                                         IEEE 802.15.4) as uint16_t in host
                                         byte order */
    NETCONF_OPT_TX_POWER,           /**< get/set the output power for radio
                                         devices in dBm as int16_t in host byte
                                         order */
    NETCONF_OPT_MAX_PACKET_SIZE,    /**< get/set the maximum packet size a
                                         network module can handle as uint16_t
                                         in host byte order */
    NETCONF_OPT_PRELOADING,         /**< en/disable preloading or read the
                                         current state. Preload using
                                         *send_data*, send setting state to
                                         *NETCONF_STATE_TX* */
    NETCONF_OPT_PROMISCUOUSMODE,    /**< en/disable promiscuous mode or read
                                         the current state */
    NETCONF_OPT_AUTOACK,            /**< en/disable link layer auto ACKs or read
                                         the current state */
    NETCONF_OPT_PROTO,              /**< get/set the protocol for the layer
                                         as type *ng_nettype_t*. */
    NETCONF_OPT_STATE,              /**< get/set the state of network devices as
                                         type *net_netconf_state_t* */
    /* add more options if needed */
} ng_netconf_opt_t;

/**
 * @brief   Binary parameter for enabling and disabling options
 */
typedef enum {
    NETCONF_DISABLE = 0,            /**< disable a given option */
    NETCONF_ENABLE = 1,             /**< enable a given option */
} ng_netconf_enable_t;

/**
 * @brief   Option parameter to be used with *NETCONF_OPT_STATE* to set or get
 *          the state of a network device or protocol implementation
 */
typedef enum {
    NETCONF_STATE_OFF = 0,          /**< powered off */
    NETCONF_STATE_SLEEP,            /**< sleep mode */
    NETCONF_STATE_IDLE,             /**< idle mode */
    NETCONF_STATE_RX,               /**< receive mode */
    NETCONF_STATE_TX,               /**< triggers a transmition */
    NETCONF_STATE_RESET,            /**< triggers a hardware reset. The resulting
                                         state of the radio device is NETCONF_STATE_IDLE */
    /* add other states if needed */
} ng_netconf_state_t;

#ifdef __cplusplus
}
#endif

#endif /* NG_NET_CONF_H_ */
/** @} */
