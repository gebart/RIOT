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
 * @brief Message type for passing data up the network stack
 */
#define NETAPI_MSG_TYPE_RCV     (0x0201)

/**
 * @brief Message type for passing data down the network stack
 */
#define NETAPI_MSG_TYPE_SND     (0x0202)

#define NETAPI_MSG_TYPE_SETOPT  (0x0203)

#define NETAPI_MSG_TYPE_GETOPT  (0x0204)

#define NETAPI_MSG_TYPE_ACK     (0x0205)



typedef struct {
    netconf_opt_t type;
    void *data;
    uint16_t data_len;
    uint16_t param;
} netapi_opt_t;


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

#ifdef __cplusplus
}
#endif

#endif /* __NETAPI_H_ */
/**
 * @}
 */
