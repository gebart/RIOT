/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_netdev Network device driver interface
 * @addtogroup  net
 * @{
 *
 * @file
 * @brief       Network device driver interface definition.
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __NETDEV_H_
#define __NETDEV_H_

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include "pkt.h"
#include "netconf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type for @ref msg_t if device fired an event.
 */
#define NETDEV_MSG_EVENT_TYPE   (0x0100)

/**
 * TODO: document...
 */
typedef enum {
    NETDEV_EVENT_RX_STARTED     = 0x0001,
    NETDEV_EVENT_RX_COMPLETE    = 0x0002,
    NETDEV_EVENT_TX_STARTED     = 0x0004,
    NETDEV_EVENT_TX_COMPLETE    = 0x0008
    /* probably more? */
} netdev_event_t;

/**
 * @brief   Event callback for signaling event to a MAC layer.
 *
 * @param[in] type          type of the event
 * @param[in] arg           event argument, can e.g. contain a pktsnip_t pointer
 */
typedef void (*netdev_event_cb_t)(netdev_event_t type, void *arg);

/**
 * @brief   The netdev data-structure holds the minimum information needed for
 *          interaction with MAC layers
 *
 * The netdev structure is the parent for all network device driver descriptors.
 */
typedef struct netdev_t netdev_t;

/**
 * @brief   Network device API definition.
 *
 * @details This is a set of functions that must be implemented by any driver
 *           for a network device.
 */
typedef struct {
    /**
     * @brief Send data via a given network device
     *
     * @param[in] dev               the network device
     * @param[in] pkt              pointer to the data to sent
     *
     * @return  the number of byte actually send on success
     * @return  -EAFNOSUPPORT if address of length dest_len is not supported
     *          by the device *dev*
     * @return  -ENODEV if *dev* is not recognized
     * @return  -EMSGSIZE if the total frame size is too long to fit in a frame
     *          of the device *dev*
     * @return  a fitting negative other errno on other failure
     */
    int (*send_data)(netdev_t *dev, pktsnip_t *pkt);

    /**
     * @brief   Registers an event callback to a given network device.
     *
     * @param[in] dev   the network device.
     * @param[in] cb    the callback.
     *
     * @return  0, on success
     * @return  -ENODEV, if *dev* is not recognized
     * @return  -ENOBUFS, if maximum number of registrable callbacks is exceeded
     */
    int (*add_event_callback)(netdev_t *dev, netdev_event_cb_t cb);

    /**
     * @brief   Unregisters an event callback to a given network device.
     *
     * @param[in] dev   the network device.
     * @param[in] cb    the callback.
     *
     * @return  0, on success
     * @return  -ENODEV, if *dev* is not recognized
     */
    int (*rem_event_callback)(netdev_t *dev, netdev_event_cb_t cb);

    /**
     * @brief   Get an option value from a given network device.
     *
     * @param[in] dev           the network device
     * @param[in] opt           the option type
     * @param[out] value        pointer to store the gotten value in
     * @param[in,out] value_len the length of *value*. Must be initialized to
     *                          the available space in *value* on call.
     * @return  0, on success
     * @return  -ENODEV, if *dev* is not recognized
     * @return  -ENOTSUP, if *opt* is not supported for the device with this
     *          operation
     * @return  -EOVERFLOW, if available space in *value* given in *value_len*
     *          is not big enough to store the option value.
     * @return  any other fitting negative errno if the ones stated above
     *          are not sufficient
     */
    int (*get_option)(netdev_t *dev, netconf_opt_t opt, void *value,
                      size_t *value_len);

    /**
     * @brief   Set an option value for a given network device.
     *
     * @param[in] dev           the network device
     * @param[in] opt           the option type
     * @param[in] value         the value to set
     * @param[in] value_len     the length of *value*
     *
     * @return  0, on success
     * @return  -EINVAL, if *value* is not in a required format
     * @return  -ENODEV, if *dev* is not recognized
     * @return  -ENOTSUP, if *opt* is not supported for the device with this
     *          operation
     * @return  -EOVERFLOW, if *value_len* is larger then the device expects.
     * @return  -EPROTONOSUPPORT, if *opt* was NETDEV_OPT_PROTO and type is
     *          not supported.
     * @return  any other fitting negative errno if the ones stated above
     *          are not sufficient
     */
    int (*set_option)(netdev_t *dev, netconf_opt_t opt, void *value,
                      size_t value_len);

    /**
     * @brief   Must be called by a controlling thread if a message of type
     *          NETDEV_MSG_EVENT_TYPE was received
     *
     * @param[in] dev           the network device that fired the event.
     * @param[in] event_type    Event type. Values are free to choose for the
     *                          driver. Must be given in the @ref msg_t::value
     *                          of the received message
     */
    void (*isr_event)(netdev_t *dev, uint16_t event_type);
} netdev_driver_t;

struct netdev_t {
    netdev_driver_t *driver;    /**< pointer to the devices interface */
    netdev_event_cb_t event_cb; /**< netdev event callback */
    kernel_pid_t mac_pid;       /**< the driver's thread's PID */
};

#ifdef __cplusplus
}
#endif

#endif /* __NETDEV_H_ */
/** @} */
