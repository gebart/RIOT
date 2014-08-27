/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  net_dev
 * @{
 *
 * @file        net_dev/base.h
 * @brief       Basic network device driver interface definitions.
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef __NET_DEV_BASE_H_
#define __NET_DEV_BASE_H_

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include "clist.h"

/**
 * @brief Type for @ref msg_t if device fired an event.
 */
#define NET_DEV_MSG_EVENT_TYPE  (0x0100)

/**
 * @brief   Definition of device families.
 */
typedef enum {
    NET_DEV_TYPE_UNKNOWN = 0,   /**< Type was not specified and may not
                                     understand this API */
    NET_DEV_TYPE_BASE,          /**< Device understands this API */
    NET_DEV_TYPE_802154,        /**< Device understands this API and the API
                                     defined in @ref net_dev_802154 */
    NET_DEV_TYPE_NATIVENET,     /**< Device understands this API and uses
                                     @ref native_net_dev_t for device definition */
} net_dev_type_t;

/**
 * @brief   Definition of protocol families to determine which frame types a
 *          device sends
 *
 * @note    XXX: The concrete definition of the values is necessary to work
 *          with super-flexible devices as e.g. @ref nativenet. It was also
 *          decided not to use ethertype since protocols not supplied by it
 *          might be supported
 */
typedef enum {
    NET_DEV_PROTO_UNKNOWN        = 0x0000,   /**< Type was not specified */

    /**
     * @brief   Radio frame protocol
     *
     * @detail  Sends frames as defined by radio_packet_t.
     */
    NET_DEV_PROTO_RADIO          = 0x0001,

    /**
     * @brief   IEEE 802.15.4
     *
     * @detail  Sends frames as defined by ieee802154_frame_t
     */
    NET_DEV_PROTO_802154         = 0x0002,

    /**
     * @brief   IEEE 802.15.4 encapsulation format
     *
     * @detail  Device understands the basic API specified here and sends
     *          IEEE 802.15.4 frames (as defined by ieee802154_frame_t)
     *          encapsulated in frames defined by radio_packet_t. This is
     *          an adaptation layer to send "IPv6 over IEEE 802.15.4" 6LoWPAN
     *          with other radios. It is may not be ensured, that the device
     *          supports the @ref net_dev_802154 API.
     */
    NET_DEV_PROTO_RADIO_802154   = 0x0003,

    NET_DEV_PROTO_6LOWPAN        = 0x0004,   /**< 6LoWPAN */
    NET_DEV_PROTO_IPV6           = 0x0005,   /**< IPv6 */
    NET_DEV_PROTO_UDP            = 0x0006,   /**< UDP */
    NET_DEV_PROTO_TCP            = 0x0007,   /**< TCP */
    NET_DEV_PROTO_CCNL           = 0x0008,   /**< CCN lite */
} net_dev_proto_t;

/**
 * @brief   Definition of basic network device options.
 * @note    Feel free to expand if your device needs/supports more.
 */
typedef enum {
    /**
     * @brief   Communication type for the device as defined by net_dev_proto_t
     *
     * @detail  If a driver does not support the type (but the setting of the
     *          option is supported) it net_dev_t::set_option() shall result
     *          with -EPROTONOSUPPORT.
     *
     *          *value_len* for net_dev_t::get_option() must always be at
     *          least sizeof(net_dev_proto_t).
     */
    NET_DEV_OPT_PROTO = 0,
    NET_DEV_OPT_CHANNEL,            /**< Channel for the device as unsigned value
                                         in host byte order */
    NET_DEV_OPT_ADDRESS,            /**< Hardware address for the device as
                                         unsigned value in host byte order */
    NET_DEV_OPT_NID,                /**< Network ID (e.g. PAN ID in IEEE 802.15.4)
                                         for the device as unsigned value in
                                         host byte order */
    NET_DEV_OPT_ADDRESS_LONG,       /**< Longer hardware address for the device
                                         (e.g. EUI-64) for the device as
                                         unsigned value in host byte order */
    NET_DEV_OPT_TX_POWER,           /**< The output of the device in dB as
                                         signed value in host byte order */
    NET_DEV_OPT_MAX_PACKET_SIZE,    /**< Maximum packet size the device supports
                                         unsigned value in host byte order */
} net_dev_opt_t;

/**
 * @brief   Definition of basic network device.
 * @note    Feel free to expand if your device needs/supports more
 */
typedef enum {
    NET_DEV_STATE_POWER_OFF = 0,        /**< Device is powered off */
    NET_DEV_STATE_POWER_SLEEP,          /**< Device is sleeping */
    NET_DEV_STATE_POWER_IDLE,           /**< Device is idle */
    NET_DEV_STATE_RX_MODE,              /**< Device is in receive mode */
    NET_DEV_STATE_PROMISCUOUS_MODE,     /**< Device is in receive mode and
                                             accepts all packets without regard
                                             for their destination */
} net_dev_state_t;

/**
 * @brief   Circular list type to store a number of protocol headers of
 *          unspecified type to work with @ref clist.h.
 *
 * @extends clist_node_t
 */
typedef struct net_dev_hlist_t {
    struct net_dev_hlist_t *next;   /**< next element in list */
    struct net_dev_hlist_t *prev;   /**< previous element in list */
    void *header;                   /**< the header stored in here */
    size_t header_len;              /**< the length of the header in byte */
} net_dev_hlist_t;

/**
 * @brief   Definition of the network device type
 *
 * @see struct net_dev_t
 *
 * @note    Forward definition to use in net_dev_driver_t
 */
typedef struct net_dev_t net_dev_t;

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
typedef int (*net_dev_rcv_data_cb_t)(net_dev_t *dev, void *src, size_t src_len,
                                     void *dest, size_t dest_len, void *payload,
                                     size_t payload_len);

/**
 * @brief   Network device API definition.
 *
 * @details  This is a set of functions that must be implemented by any driver
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
    int (*init)(net_dev_t *dev);

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
    int (*send_data)(net_dev_t *dev, void *dest, size_t dest_len,
                     net_dev_hlist_t *upper_layer_hdrs, void *data,
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
    int (*add_receive_data_callback)(net_dev_t *dev, net_dev_rcv_data_cb_t cb);

    /**
     * @brief   Unregisters a receive data callback to a given network device.
     *
     * @param[in] dev   the network device.
     * @param[in] cb    the callback.
     *
     * @return  0, on success
     * @return  -ENODEV, if *dev* is not recognized
     */
    int (*rem_receive_data_callback)(net_dev_t *dev, net_dev_rcv_data_cb_t cb);

    /**
     * @brief   Get an option value from a given network device.
     *
     * @param[in] dev           the network device
     * @param[in] opt           the option type
     * @param[out] value        pointer to store the gotten value in
     * @param[in,out] value_len the length of *value*. Must be initialized to the
     *                          available space in value on call.
     * @return  0, on success
     * @return  -ENODEV, if *dev* is not recognized
     * @return  -ENOTSUP, if *opt* is not supported for the device with this
     *          operation
     * @return  -EOVERFLOW, if length of *value* is longer then *value_len*.
     * @return  any other fitting negative errno if the ones stated above
     *          are not sufficient
     */
    int (*get_option)(net_dev_t *dev, net_dev_opt_t opt, void *value,
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
     * @return  -EPROTONOSUPPORT, if *opt* was NET_DEV_OPT_PROTO and type is
     *          not supported.
     * @return  any other fitting negative errno if the ones stated above
     *          are not sufficient
     */
    int (*set_option)(net_dev_t *dev, net_dev_opt_t opt, void *value,
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
    int (*get_state)(net_dev_t *dev, net_dev_state_t *state);

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
    int (*set_state)(net_dev_t *dev, net_dev_state_t state);

    /**
     * @brief   Must be called by a controlling thread if a message of type
     *          NET_DEV_MSG_EVENT_TYPE was received
     *
     * @param[in] dev           the network device that fired the event.
     * @param[in] event_type    Event type. Values are free to choose for the
     *                          driver. Must be given in the msg_t::content.value
     *                          of the received message
     */
    void (*event)(net_dev_t *dev, uint32_t event_type);
} net_dev_driver_t;

/**
 * @brief   Definition of the network device type
 *
 * @note    Your driver may overload this with additional information (e.g.
 *          how the device is connected)
 */
struct net_dev_t {
    net_dev_type_t type;                /**< The type of this device */
    const net_dev_driver_t *driver;     /**< The driver for this device */
};

/**
 * @brief   Helper function to calculate the total length of the headers in
 *          *hlist*.
 *
 * @param[in]   hlist   The header list.
 *
 * @return  Length in number of bytes of all headers in *hlist*.
 */
size_t net_dev_get_hlist_len(const net_dev_hlist_t *hlist);

/**
 * @brief Advance the header list
 *
 * @see clist_advance
 *
 * @param[in,out] list  The list to work upon.
 */
static inline void net_dev_hlist_advance(net_dev_hlist_t **list)
{
    clist_advance((clist_node_t **)list);
}

/**
 * @brief Add *node* to list after element referenced by *list*
 *
 * @see clist_add
 *
 * @param[in,out] list  The list to add *node* to
 * @param[in] node      The node to add to the list
 */
static inline void net_dev_hlist_add(net_dev_hlist_t **list,
                                     net_dev_hlist_t *node)
{
    clist_add((clist_node_t **)list, (clist_node_t *)node);
}

/**
 * @brief Removes *node* from list *list*
 *
 * @see clist_remove
 *
 * @param[in,out] list  The list to remove *node* from
 * @param[in] node      The node to remove from the list
 */
static inline void net_dev_hlist_remove(net_dev_hlist_t **list,
                                        net_dev_hlist_t *node)
{
    clist_remove((clist_node_t **)list, (clist_node_t *)node);
}

#endif /* __NET_DEV_BASE_H_ */
/**
 * @}
 */
