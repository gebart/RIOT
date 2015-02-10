/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_net_udp UDP
 * @ingroup     net
 * @brief       RIOT's implementation of the UDP protocol
 *
 * @{
 *
 * @file
 * @brief       UDP interface definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NG_UDP_H_
#define NG_UDP_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default stack size to use for the UDP thread
 */
#ifndef NG_UDP_STACK_SIZE
#define NG_UDP_STACK_SIZE       KERNEL_CONF_STACKSIZE_DEFAULT
#endif

/**
 * @brief   Default name for the UDP thread
 */
#ifndef NG_UDP_THREAD_NAME
#define NG_UDP_THREAD_NAME      "UDP"
#endif

/**
 * @brief   Size of a UDP header
 */
#define NG_UDP_HDR_LENGTH       (8U)

/**
 * @brief   UDP header definition
 */
typedef struct __attribute__((packed)) {
    network_uint16_t src_port;      /**< source port, in network byte order */
    network_uint16_t dst_port;      /**< destination port, network byte order */
    network_uint16_t length;        /**< payload length (including the header),
                                         network byte order */
    uint16_t checksum;      /**< checksum */
} ng_udp_hdr_t;

/**
 * @brief   Initialize and start UDP
 *
 * @param[in] priority      priority to use for the UDP thread
 *
 * @return                  0 on success
 * @return                  negative value on error
 */
int ng_udp_init(char priority);

#endif /* NG_UDP_H_ */
/** @} */
