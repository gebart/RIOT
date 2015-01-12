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
 * @file        netdev/base.h
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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type for @ref msg_t if device fired an event.
 */
#define NETDEV_MSG_EVENT_TYPE   (0x0100)

/**
 * @brief   Definition of basic network device options.
 * @note    Feel free to expand if your device needs/supports more.
 */
typedef enum {
    /**
     * @brief   Communication type for the device as defined by @ref netdev_proto_t
     *
     * @details If a driver does not support the type (but the setting of the
     *          option is supported) it @ref netdev_driver_t::set_option() shall result
     *          with -EPROTONOSUPPORT.
     *
     *          *value_len* for @ref netdev_driver_t::get_option() must always be at
     *          least `sizeof(netdev_proto_t)`.
     */
    NETDEV_OPT_PROTO = 0,
    NETDEV_OPT_CHANNEL,             /**< Channel for the device as unsigned value
                                         in host byte order */
    NETDEV_OPT_ADDRESS,             /**< Hardware address for the device as
                                         unsigned value in host byte order */
    NETDEV_OPT_NID,                 /**< Network ID (e.g. PAN ID in IEEE 802.15.4)
                                         for the device as unsigned value in
                                         host byte order */
    NETDEV_OPT_ADDRESS_LONG,        /**< Longer hardware address for the device
                                         (e.g. EUI-64) for the device as
                                         unsigned value in host byte order */
    NETDEV_OPT_TX_POWER,            /**< The output of the device in dB as
                                         signed value in host byte order */
    NETDEV_OPT_MAX_PACKET_SIZE,     /**< Maximum packet size the device supports
                                         unsigned value in host byte order */
    NETDEV_OPT_SRC_LEN,             /**< Default mode the source address is
                                         set to as value of `size_t`. (e.g.
                                         either PAN-centric 16-bit address or
                                         EUI-64 in IEEE 802.15.4) */

    /**
     * @brief   Last value for @ref netdev_opt_t defined here
     *
     * @details Specific devices or modules like @ref netapi that utilize these
     *          values to may define higher values, but they must be greater
     *          or equal to @ref NETDEV_OPT_LAST.
     */
    NETDEV_OPT_LAST,
} netdev_opt_t;

/**
 * @brief   Definition of basic network device.
 * @note    Feel free to expand if your device needs/supports more
 */
typedef enum {
    NETDEV_STATE_POWER_OFF = 0,     /**< Device is powered off */
    NETDEV_STATE_POWER_SLEEP,       /**< Device is sleeping */
    NETDEV_STATE_POWER_IDLE,        /**< Device is idle */
    NETDEV_STATE_RX_MODE,           /**< Device is in receive mode */
    NETDEV_STATE_PROMISCUOUS_MODE,  /**< Device is in receive mode and
                                         accepts all packets without regard
                                         for their destination */
    NETDEV_STATE_TX,                /**< Device is transmitting all available
                                         data in the TX buffer */
    NETDEV_STATE_TX_BURST,          /**< Device is burst sending and
                                         does not accept packets */
} netdev_state_t;

/**
 * @brief   Circular list type to store a number of protocol headers of
 *          unspecified type to work with @ref clist.h.
 *
 * @extends clist_node_t
 */
typedef struct __attribute__((packed)) netdev_hlist_t {
    struct netdev_hlist_t *next;    /**< next element in list */
    struct netdev_hlist_t *prev;    /**< previous element in list */
    netdev_proto_t protocol;        /**< protocol of the header */
    void *header;                   /**< the header stored in here */
    size_t header_len;              /**< the length of the header in byte */
} netdev_hlist_t;

/**
 * @brief   Definition of the network device type
 *
 * @see struct netdev_t
 *
 * @note    Forward definition to use in @ref netdev_driver_t
 */
typedef struct netdev_t netdev_t;

/**
 * Receive data callback for data frames from given network device.
 *
 * @param[in] dev           the network device the frame came from.
 * @param[in] src           the (hardware) source address of the frame in host
 *                          byte order.
 * @param[in] src_len       the length of *src* in byte
 * @param[in] dest          the (hardware) destination address of the frame in
 *                          host byte order.
 * @param[in] dest_len      the length of *dest* in byte
 * @param[in] payload       the payload of the frame.
 * @param[in] payload_len   the length of *payload* in byte
 *
 * @return  the number of bytes in payload actually processed by the callback,
 *          on success
 * @return  a fitting negative errno on failure
 */
typedef int (*netdev_rcv_data_cb_t)(netdev_t *dev, void *src, size_t src_len,
                                    void *dest, size_t dest_len, void *payload,
                                    size_t payload_len);

/**
 * @brief   Network device API definition.
 *
 * @details This is a set of functions that must be implemented by any driver
 *           for a network device.
 */
typedef struct {
    /**
     * @brief Initialize a given network device.
     *
     * @param[in] dev           the device to initialize
     *
     * @return  0 on success
     * @return  -ENODEV if *dev* can not be initialized
     */
    int (*init)(netdev_t *dev);

    /**
     * @brief Send data via a given network device
     *
     * @param[in] dev               the network device
     * @param[in] dest              the (hardware) destination address for the data
     *                              in host byte order.
     * @param[in] dest_len          the length of *dest* in byte
     * @param[in] upper_layer_hdrs  header data from higher network layers from
     *                              highest to lowest layer. Must be prepended to
     *                              the data stream by the network device. May be
     *                              NULL if there are none.
     * @param[in] data              the data to send
     * @param[in] data_len          the length of *data* in byte
     *
     * @return  the number of byte actually send on success
     * @return  -EAFNOSUPPORT if address of length dest_len is not supported
     *          by the device *dev*
     * @return  -ENODEV if *dev* is not recognized
     * @return  -EMSGSIZE if the total frame size is too long to fit in a frame
     *          of the device *dev*
     * @return  a fitting negative other errno on other failure
     */
    int (*send_data)(netdev_t *dev, void *dest, size_t dest_len,
                     netdev_hlist_t *upper_layer_hdrs, void *data,
                     size_t data_len);

    /**
     * @brief   Registers a receive data callback to a given network device.
     *
     * @param[in] dev   the network device.
     * @param[in] cb    the callback.
     *
     * @return  0, on success
     * @return  -ENODEV, if *dev* is not recognized
     * @return  -ENOBUFS, if maximum number of registrable callbacks is exceeded
     */
    int (*add_receive_data_callback)(netdev_t *dev, netdev_rcv_data_cb_t cb);

    /**
     * @brief   Unregisters a receive data callback to a given network device.
     *
     * @param[in] dev   the network device.
     * @param[in] cb    the callback.
     *
     * @return  0, on success
     * @return  -ENODEV, if *dev* is not recognized
     */
    int (*rem_receive_data_callback)(netdev_t *dev, netdev_rcv_data_cb_t cb);

    /**
     * @brief   Get an option value from a given network device.
     *
     * @param[in] dev           the network device
     * @param[in] opt           the option type
     * @param[out] value        pointer to store the gotten value in
     * @param[in,out] value_len the length of *value*. Must be initialized to the
     *                          available space in *value* on call.
     * @return  0, on success
     * @return  -ENODEV, if *dev* is not recognized
     * @return  -ENOTSUP, if *opt* is not supported for the device with this
     *          operation
     * @return  -EOVERFLOW, if available space in *value* given in *value_len*
     *          is not big enough to store the option value.
     * @return  any other fitting negative errno if the ones stated above
     *          are not sufficient
     */
    int (*get_option)(netdev_t *dev, netdev_opt_t opt, void *value,
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
    int (*set_option)(netdev_t *dev, netdev_opt_t opt, void *value,
                      size_t value_len);

    /**
     * @brief   Get state from a given network device.
     *
     * @param[in] dev       the network device
     * @param[out] state    the network device
     *
     * @return  0, on success
     * @return  -ENODEV, if *dev* is not recognized
     * @return  -ETIME, if device timed out on trying to acquire state
     */
    int (*get_state)(netdev_t *dev, netdev_state_t *state);

    /**
     * @brief   Set state from a given network device.
     *
     * @param[in] dev      the network device
     * @param[in] state    the network device
     *
     * @return  0, on success
     * @return  -ENODEV, if *dev* is not recognized
     * @return  -ENOTSUP, if *state* is not supported
     * @return  -ETIME, if device timed out on trying to change state
     */
    int (*set_state)(netdev_t *dev, netdev_state_t state);

    /**
     * @brief   Must be called by a controlling thread if a message of type
     *          NETDEV_MSG_EVENT_TYPE was received
     *
     * @param[in] dev           the network device that fired the event.
     * @param[in] event_type    Event type. Values are free to choose for the
     *                          driver. Must be given in the @ref msg_t::value
     *                          of the received message
     */
    void (*event)(netdev_t *dev, uint32_t event_type);

    /**
     * @brief   Pointer to device specific configuration data, e.g. peripherals
     *          used, internal state, etc.
     */
    void *device;
} netdev_t;

#ifdef __cplusplus
}
#endif

#endif /* __NETDEV_H_ */
/** @} */
