/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_netapi Configuration options for network APIs
 * @ingroup     net
 * @brief       List of available configuration options used by netdev and netapi
 * @{
 *
 * @file
 * @brief       Definition of global configuration options for netdev and netapi
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NET_CONF_H_
#define NET_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Global list of configuration options available throughout the
 *          network stack
 *
 * @detail  A protocol should at least always answer with an acknowledgement of
 *          result @ref netapi NETAPI_STATUS_OK if the operation was
 *          successfull or with errno -ENOTSUP if the operation is not
 *          supported.
 */
typedef enum {
    NETCONF_OPT_CHANNEL,             /**< Channel for the device as unsigned value
                                         in host byte order */
    NETCONF_OPT_IS_CHANNEL_CLR,      /**< Check if channel is clear */
    NETCONF_OPT_ADDRESS,             /**< Hardware address for the device as
                                         unsigned value in host byte order */
    NETCONF_OPT_NID,                 /**< Network ID (e.g. PAN ID in IEEE 802.15.4)
                                         for the device as unsigned value in
                                         host byte order */
    NETCONF_OPT_ADDRESS_LONG,        /**< Longer hardware address for the device
                                         (e.g. EUI-64) for the device as
                                         unsigned value in host byte order */
    NETCONF_OPT_TX_POWER,            /**< The output of the device in dB as
                                         signed value in host byte order */
    NETCONF_OPT_MAX_PACKET_SIZE,     /**< Maximum packet size the device supports
                                         unsigned value in host byte order */
    NETCONF_OPT_ADDR_LEN,             /**< Default mode the source address is
                                         set to as value of `size_t`. (e.g.
                                         either PAN-centric 16-bit address or
                                         EUI-64 in IEEE 802.15.4) */
    NETCONF_OPT_EN_PRELOADING,       /**< Enable pre-loading of data, transfer
                                         data to device using send_data(), send
                                         by calling trigger(NETDEV_ACTION_TX) */
    NETCONF_OPT_EN_PROMISCUOUSMODE,  /**< Enable promiscuous mode */
    NETCONF_OPT_EN_AUTOACK,          /**< Automatically send link-layer ACKs */
    NETCONF_OPT_RSSI,                /**< Read the RSSI value from the last transfer */
    NETCONF_OPT_LQI,                 /**< Read the link quality indicator */
    /**
     * @brief   Set or get the protocol for the layer as of type netdev_proto_t.
     */
    NETCONF_OPT_PROTO,
    NETCONF_OPT_STATE,
} netconf_opt_t;


#ifdef __cplusplus
}
#endif

#endif /* NET_CONF_H_ */
/** @} */
