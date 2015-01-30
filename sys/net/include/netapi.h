/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    netapi Basic network interface
 * @ingroup     net
 * @brief       Basic general interface to communicate with a network protocol.
 * @details     The idea of this module is that every network layer provides
 *              a basic set of commands to communicate with its lower and
 *              upper layer and the system. In this model every layer has some
 *              sort of control thread with which the other layers and the system
 *              communicates (illustrated as cohesive boxes below).
 *
 *                     +-----+  +-----+   +-----+
 *                     | App |  | App |   | App |
 *                     +-----+  +-----+   +-----+
 *                        ^        ^         ^
 *                      netapi   netapi    netapi   // alternatively the normal socket API
 *                        v        v         v
 *                    +--------------------------+
 *                    | Socket | Socket | Socket |
 *                    |      Transport layer     |
 *                    +--------------------------+
 *                                 ^
 *                               netapi
 *                                 v
 *                    +--------------------------+
 *                    |      Network layer       |
 *                    +--------------------------+
 *                       ^         ^         ^
 *                     netapi    netapi    netapi
 *                       v         v         v
 *                   +-------+ +-------+ +-------+
 *                   |  MAC  | |  MAC  | |  MAC  |
 *                   +-------+ +-------+ +-------+
 *                   | Radio | | Radio | | Radio |
 *                   +-------+ +-------+ +-------+
 *
 * @{
 *
 * @file        netapi.h
 * @brief       Basic general interface to communicate with a network layer.
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef __NETAPI_H_
#define __NETAPI_H_

#include <stdlib.h>

#include "kernel.h"
#include "thread.h"
#include "netconf.h"
#include "pkt.h"

/**
 * @brief Definition for value of successful result for @ref NETAPI_CMD_ACK.
 */
#define NETAPI_STATUS_OK        (0)

/**
 * @brief Definition of type value for msg_t type field.
 */
#define NETAPI_MSG_TYPE         (0x0200)

/**
 * @brief Message type for passing data up the network stack
 */
#define NETAPI_MSG_TYPE_RCV     (0x02001)

/**
 * @brief Message type for passing data down the network stack
 */
#define NETAPI_MSG_TYPE_SND     (0x0202)

/**
 * @brief   Basic communication types with a network layer.
 * @details This list may be expanded by a some layer (using values
 *          > `SND_PKT + 4`), but no guarantee can be given, that other layers
 *          interpret it exactly as the former. The values are chosen to be
 *          as compatible as possible with the legacy tranceiver module.
 */
typedef enum {
    NETAPI_CMD_RCV,     /**< Packet was received from lower layer. */
    NETAPI_CMD_SND,     /**< Packet was send from upper layer or is send from
                         *   this layer to lower layer. */
    NETAPI_CMD_GET,     /**< Get an option of this layer. */
    NETAPI_CMD_SET,     /**< Set an option of this layer. */
    /**
     * @brief   Registers an upper layer control thread to this layer for
     *          receiving.
     *
     * @details If there is no space left in the control threads registry,
     *          the result of the acknowledgement must be errno -ENOBUFS.
     */
    NETAPI_CMD_REG,
    /**
     * @brief   Unregisteres a previously receiving upper layer control thread
     *          to this layer.
     *
     * @details Control thread does not need to notify, if the thread in
     *          question was previously known. Unknown PIDs are just ignored.
     */
    NETAPI_CMD_UNREG,
    NETAPI_CMD_ACK,     /**< Acknowledges to a previous command. */

#ifdef MODULE_NETDEV_DUMMY  /* commands for testing */
    /**
     * @brief   Fires a receive event in the control thread via
     *          unittest_netdev_dummy_fire_rcv_event, iff it handles a
     *          netdev_dummy testing device.
     *
     * @details Use this command with the netapi_rcv_pkt_t command type.
     */
    NETAPI_CMD_FIRE_RCV,
#endif
} netapi_cmd_type_t;

/**
 * @brief   Basic message definition.
 */
typedef struct {
    /* cppcheck-suppress unusedStructMember because interface is not used yet */
    netapi_cmd_type_t type;     /**< Type of the command. */
} netapi_msg_t;

/**
 * @brief   Message definition for acknowledgements.
 * @extends netapi_msg_t
 */
typedef struct {
    netapi_cmd_type_t type;     /**< Type of the message. Must be NETAPI_CMD_ACK. */

    netapi_cmd_type_t orig;     /**< Originating command type of this acknowledgement.
                                 *   Must not be NETAPI_CMD_ACK. */
    /**
     * @brief   Result code of the originating command.
     * @details Valid values are NETAPI_STATUS_OK or a fitting negative
     *          <a href="http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/errno.h.html">
     *          errno</a> value in case of error. If an operation is not supported
     *          the control thread **must** answer with result of errno -ENOTSUP.
     *          Furthermore, in case of NETAPI_CMD_SND, NETAPI_CMD_RCV, or
     *          NETAPI_CMD_GET, this value needs to be the actual length of
     *          the data send or received respectively.
     */
    int result;
} netapi_ack_t;

/**
 * @brief   Basic command definition.
 * @extends netapi_msg_t
 */
typedef struct {
    /* cppcheck-suppress unusedStructMember because interface is not used yet */
    netapi_cmd_type_t type;     /**< Type of the command. Must not be NETAPI_CMD_ACK. */
    netapi_ack_t *ack;          /**< Pointer to where the acknowledgement to this
                                     message is stored. Must not be NULL. */
} netapi_cmd_t;

/**
 * @brief   Command definition for getting and setting packets.
 * @extends netapi_cmd_t
 */
typedef struct {
    /* cppcheck-suppress unusedStructMember because interface is not used yet */
    netapi_cmd_type_t type;     /**< Type of the command. Must be either
                                 *   NETAPI_CMD_GET or NETAPI_CMD_SET. */
    netapi_ack_t *ack;          /**< Pointer to where the acknowledgement to this
                                 *   message is stored. Must not be NULL. */
    /* cppcheck-suppress unusedStructMember because interface is not used yet */
    netconf_opt_t param;   /**< Parameter type. */

    /**
     * @brief Data of the parameter.
     * @details In case of NETAPI_CMD_GET the data will be written here
     *          by the addressed layer, in case of NETAPI_CMD_SET the data
     *          want to set must be written here (if not specified otherwise).
     */
    void *data;
    /* cppcheck-suppress unusedStructMember because interface is not used yet */
    size_t data_len;            /**< Length of netapi_conf_t::data. */
} netapi_conf_t;

/**
 * @brief   Type for demultiplexing context of registration command.
 * @note    Introduce own type for easy upscaling in the future
 */
typedef uint16_t netapi_reg_demux_ctx_t;

/**
 * @brief   Command definition for upper layer registration.
 * @extends netapi_cmd_t
 */
typedef struct {
    /* cppcheck-suppress unusedStructMember because interface is not used yet */
    netapi_cmd_type_t type; /**< Type of the command. Must be NETAPI_CMD_REG or
                             *   NETAPI_CMD_UNREG. */
    netapi_ack_t *ack;      /**< Pointer to where the acknowledgement to this
                             *   message is stored. Must not be NULL. */
    kernel_pid_t reg_pid;   /**< The PID of the upper layer thread you want
                             *   to (un)register. If the PID is duplicate /
                             *   was not known it is silently ignored. */
    /**
     * @brief Protocol specific context information for demultiplex (e.g. next
     *        header value in IP, port in UDP/TCP, etc).
     *
     * @details For NETAPI_CMD_UNREG this field is always ignored. Some protocols
     *          might also ignore it silently on NETAPI_CMD_REG.
     */
    netapi_reg_demux_ctx_t demux_ctx;
} netapi_reg_t;


#ifdef MODULE_TRANSCEIVER
/**
 * @brief PID for a netapi layer, when used with the transceiver module.
 *
 * @details This variable is only needed, iff this definitions are used with
 *          the transceiver module in legacy applications. Needs to be set
 *          externaly (e. g. the layer, that uses netapi). Guard it against
 *          other layers using netapi with the transceiver with
 *          the macro `HAS_NETAPI_PID`, the transceiver module will check
 *          if it is there and only than allow adapter capabilities. Since
 *          only one netapi layer can be used with this hack, it is
 *          advised to let only layers below network layer use this.
 */
extern kernel_pid_t netapi_pid;
#endif  /* MODULE_TRANSCEIVER */

#ifdef MODULE_NETAPI /* Only include if module netapi is compiled */
/**
 * @brief Sends a command to a protocol's control thread.
 *
 * @param[in] pid   The PID of the protocol's control thread.
 * @param[in] cmd   The command you like to send.
 *
 * @return  result of the acknowledgement.
 * @return  -ENOMSG if wrong acknowledgement was received or was no
 *          acknowledgement at all.
 */
int netapi_send_command(kernel_pid_t pid, netapi_cmd_t *cmd);

/**
 * @brief Sends data over a protocol layer identified by *pid* with upper layer
 *        information attached.
 *
 * @note    Wraps IPC call of NETAPI_CMD_SND.
 *
 * @details The upper layer information will be prepended
 *          on the lowest possible level.
 *
 * @param[in] pid       The PID of the protocol's control thread.
 * @param[in] upper_layer_hdrs  Headers of upper layer protocols, lowest layer
 *                      first, highest layer last.
 * @param[in] addr      The address you want the data send to. If the control
 *                      thread knows for some reason where to send the data
 *                      (e.g. a connected TCP socket), *addr* may be NULL.
 * @param[in] addr_len  Length of *addr*. If the control thread knows for some
 *                      reason where to send the data (e.g. a connected TCP
 *                      socket), *addr_len* may be 0.
 * @param[in] data      The data you want to send over the protocol layer
 *                      controled by *pid* (without upper layer protocol
 *                      information).
 * @param[in] data_len  Length of *data_len*.
 *
 * @see netapi_snd_pkt_t
 *
 * @return  result of the acknowledgement.
 * @return  -ENOMSG if wrong acknowledgement was received or was no
 *          acknowledgement at all.
 */
int netapi_send_packet(kernel_pid_t pid, pktsnip_t *pkt);

/**
 * @brief Get an option of a protocol layer identified by *pid*.
 *
 * @note    Wraps IPC call of NETAPI_CMD_GET.
 *
 * @param[in] pid       The PID of the protocol's control thread.
 * @param[in] data      Buffer you want to store *param*'s' data in.
 * @param[in] data_len  Length of *data*, must be initialized with
 *                      the original length of *data*.
 *
 * @return  result of the acknowledgement.
 * @return  -ENOMSG if wrong acknowledgement was received or was no
 *          acknowledgement at all.
 */
int netapi_get_option(kernel_pid_t pid, netconf_opt_t param,
                      void *data, size_t data_len);

/**
 * @brief Set an option of a protocol layer identified by *pid*.
 *
 * @note    Wraps IPC call of NETAPI_CMD_SET.
 *
 * @param[in] pid       The PID of the protocol's control thread.
 * @param[in] data      Buffer with the data you want to set *param* to.
 * @param[in] data_len  Length of *data*.
 *
 * @return  result of the acknowledgement.
 * @return  -ENOMSG if wrong acknowledgement was received or was no
 *          acknowledgement at all.
 */
int netapi_set_option(kernel_pid_t pid, netconf_opt_t param,
                      void *data, size_t data_len);

/**
 * @brief Register an upper layer control thread for reception to a protocol
 *        layer identified by *pid*.
 *
 * @note    Wraps IPC call of NETAPI_CMD_REG.
 *
 * @param[in] pid       The PID of the protocol's control thread.
 * @param[in] reg_pid   The PID of the upper layer protocol's control thread.
 * @param[in] demux_ctx Protocol specific demuxing context.
 *
 * @return  result of the acknowledgement.
 * @return  -ENOMSG if wrong acknowledgement was received or was no
 *          acknowledgement at all.
 */
int netapi_register(kernel_pid_t pid, kernel_pid_t reg_pid,
                    netapi_reg_demux_ctx_t demux_ctx);

/**
 * @brief Registers the current thread as upper layer control thread for
 *        reception to a protocol layer identified by *pid*.
 *
 * @note    Wraps IPC call of NETAPI_CMD_REG with
 *          `netapi_reg_t::reg_pid == thread_getpid()`.
 *
 * @param[in] pid       The PID of the protocol's control thread.
 * @param[in] demux_ctx Protocol specific demuxing context.
 *
 * @return  result of the acknowledgement.
 * @return  -ENOMSG if wrong acknowledgement was received or was no
 *          acknowledgement at all.
 */
static inline int netapi_register_current_thread(kernel_pid_t pid,
        netapi_reg_demux_ctx_t demux_ctx)
{
    return netapi_register(pid, thread_getpid(), demux_ctx);
}

/**
 * @brief Unregister an upper layer control thread for reception to a protocol
 *        layer identified by *pid*.
 *
 * @note    Wraps IPC call of NETAPI_CMD_UNREG.
 *
 * @param[in] pid       The PID of the protocol's control thread.
 * @param[in] reg_pid   The PID of the upper layer protocol's control thread.
 *
 * @return  result of the acknowledgement.
 * @return  -ENOMSG if wrong acknowledgement was received or was no
 *          acknowledgement at all.
 */
int netapi_unregister(kernel_pid_t pid, kernel_pid_t reg_pid);

/**
 * @brief Unregisters the current thread as upper layer control thread for
 *        reception to a protocol layer identified by *pid*.
 *
 * @note    Wraps IPC call of NETAPI_CMD_UNREG with
 *          `netapi_reg_t::reg_pid == thread_getpid()`.
 *
 * @param[in] pid       The PID of the protocol's control thread.
 *
 * @return  result of the acknowledgement.
 * @return  -ENOMSG if wrong acknowledgement was received or was no
 *          acknowledgement at all.
 */
static inline int netapi_unregister_current_thread(kernel_pid_t pid)
{
    return netapi_unregister(pid, thread_getpid());
}

#ifdef MODULE_NETDEV_DUMMY
/**
 * @brief   Fire receive event in the control thread via
 *          @ref unittest_netdev_dummy_fire_rcv_event, iff it handles a
 *          netdev_dummy testing device.
 *
 * @param[in] pid           The PID of the protocol's control thread.
 * @param[in] src           Source address of the received packet.
 * @param[in] src_len       Length of *src*.
 * @param[in] dest          Destination address of the received packet.
 * @param[in] dest_len      Length of *dest*.
 * @param[in] data          Content of the received packet.
 * @param[in] data_len      Length of *data*.
 *
 * @return  result of the acknowledgement.
 * @return  -ENOMSG if wrong acknowledgement was received or was no
 *          acknowledgement at all.
 */
int netapi_fire_receive_event(kernel_pid_t pid, void *src, size_t src_len,
                              void *dest, size_t dest_len, void *data,
                              size_t data_len);
#endif /* MODULE_NETDEV_DUMMY */
#endif /* MODULE_NETAPI */

#ifdef __cplusplus
}
#endif

#endif /* __NETAPI_H_ */
/**
 * @}
 */
