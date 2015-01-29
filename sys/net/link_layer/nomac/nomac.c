/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#include <errno.h>
#include <string.h>

#include "netapi.h"
#include "msg.h"
#include "netdev.h"
#include "thread.h"

#include "nomac.h"

#ifdef MODULE_NETDEV_DUMMY
#include "netdev_dummy.h"
#endif

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define NOMAC_MSG_QUEUE_SIZE    (16)

kernel_pid_t who_to_hack;

static void _event_cb(netdev_event_t event, void *pkt)
{
    msg_t msg;

    switch (event) {
        case NETDEV_EVENT_RX_COMPLETE:
            /* TODO: get list of recipients from somewhere */

            msg.type = NETAPI_MSG_TYPE_RCV;
            msg.content.ptr = pkt;

            msg_send(&msg, who_to_hack);
            break;
        default:
            /* do just nothing */
            return;
    }
}

// static int _nomac_get_option(netdev_t *dev, netapi_conf_t *conf)
// {
//     int res;

//     switch ((nomac_conf_type_t)conf->param) {
//         case NOMAC_PROTO:
//         case NOMAC_CHANNEL:
//         case NOMAC_ADDRESS:
//         case NOMAC_NID:
//         case NOMAC_MAX_PACKET_SIZE:
//         case NOMAC_ADDRESS2:
//             if ((res = dev->driver->get_option(dev, (netdev_opt_t)conf->param,
//                                                conf->data, &(conf->data_len))
//                 ) == 0) {
//                 return (int)conf->data_len;
//             }
//             else {
//                 return res;
//             }

//         case NOMAC_REGISTRY:
//             return _nomac_get_registry(conf);

//         default:
//             break;
//     }

//     return -ENOTSUP;
// }

// static int _nomac_set_option(netdev_t *dev, netapi_conf_t *conf)
// {
//     switch ((nomac_conf_type_t)(conf->param)) {
//         case NOMAC_PROTO:
//         case NOMAC_CHANNEL:
//         case NOMAC_ADDRESS:
//         case NOMAC_NID:
//         case NOMAC_ADDRESS2:
//             return dev->driver->set_option(dev, (netdev_opt_t)conf->param,
//                                            conf->data, conf->data_len);

//         default:
//             break;
//     }

//     return -ENOTSUP;
// }

static void *_nomac_runner(void *args)
{
    netdev_t *dev = (netdev_t *)args;
    msg_t msg_cmd, msg_queue[NOMAC_MSG_QUEUE_SIZE];

    /* setup the MAC layers message queue */
    msg_init_queue(msg_queue, NOMAC_MSG_QUEUE_SIZE);

    /* connect to the device driver */
    dev->mac_pid = thread_getpid();
    dev->driver->add_event_callback(dev, _event_cb);

    // netapi_cmd_t *cmd;
    // msg_ack.type = NETAPI_MSG_TYPE;


    while (1) {
        msg_receive(&msg_cmd);

        if (msg_cmd.type == NETDEV_MSG_EVENT_TYPE) {
            dev->driver->isr_event(dev, msg_cmd.content.value);
        }
        else if (msg_cmd.type == NETAPI_MSG_TYPE_SND) {
            dev->driver->send_data(dev, (pkt_t *)msg_cmd.content.ptr);
        }

        // else if (msg_cmd.type == NETAPI_MSG_TYPE) {
        //     cmd = (netapi_cmd_t *)(msg_cmd.content.ptr);
        //     ack = cmd->ack;
        //     msg_ack.content.ptr = (char *)ack;

        //     switch (cmd->type) {
        //         case NETAPI_CMD_SND:
        //             ack->result = _nomac_send(dev, (netapi_snd_pkt_t *)cmd);
        //             break;

        //         case NETAPI_CMD_GET:
        //             ack->result = _nomac_get_option(dev, (netapi_conf_t *)cmd);
        //             break;

        //         case NETAPI_CMD_SET:
        //             ack->result = _nomac_set_option(dev, (netapi_conf_t *)cmd);
        //             break;

        //         case NETAPI_CMD_REG:
        //             ack->result = -ENOBUFS;

        //             for (int i = 0; i < NOMAC_REGISTRY_SIZE; i++) {
        //                 if (_nomac_registry[i].registrar_pid == KERNEL_PID_UNDEF) {
        //                     netapi_reg_t *reg = (netapi_reg_t *)cmd;

        //                     _nomac_registry[i].registrar_pid = thread_getpid();
        //                     _nomac_registry[i].recipient_pid = reg->reg_pid;
        //                     ack->result = NETAPI_STATUS_OK;

        //                     break;
        //                 }
        //             }

        //             break;

        //         case NETAPI_CMD_UNREG:
        //             ack->result = NETAPI_STATUS_OK;

        //             for (int i = 0; i < NOMAC_REGISTRY_SIZE; i++) {
        //                 netapi_reg_t *reg = (netapi_reg_t *)cmd;

        //                 if (_nomac_registry[i].registrar_pid == thread_getpid() &&
        //                     _nomac_registry[i].recipient_pid == reg->reg_pid) {
        //                     _nomac_registry[i].recipient_pid = KERNEL_PID_UNDEF;

        //                     break;
        //                 }

        //             }

        //             break;

        //         default:
        //             ack->result = -ENOTSUP;
        //             break;
        //     }

        //     ack->type = NETAPI_CMD_ACK;
        //     ack->orig = cmd->type;
        //     msg_reply(&msg_cmd, &msg_ack);
        // }
    }

    /* never reached */
    return NULL;
}

kernel_pid_t nomac_init(char *stack, int stacksize, char priority,
                        const char *name, netdev_t *dev)
{
    return thread_create(stack, stacksize, priority, CREATE_STACKTEST,
                         _nomac_runner, (void *)dev, name);
}
