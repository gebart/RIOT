/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file    netapi.c
 * @brief   You do not necessarily need this file (leave `netapi` out of
 *          `USEMODULE`, the netapi.h file will still be available for
 *          inclusion). It supplies you however with some helper functions
 *          for the thread communication.
 * @}
 */

#include <errno.h>

#include "kernel.h"
#include "msg.h"

#include "netapi.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

int netapi_send_packet(kernel_pid_t pid, pktsnip_t *pkt)
{
    msg_t msg;
    msg.type = NETAPI_MSG_TYPE_SND;
    msg.content.ptr = (void *)pkt;
    return msg_send(&msg, pid);
}

static int _get_set_option(kernel_pid_t pid, uint16_t type,
                           netconf_opt_t param, void *data, size_t data_len)
{
    msg_t cmd;
    msg_t ack;
    netapi_opt_t opt;

    opt.type = param;
    opt.data = data;
    opt.data_len = data_len;

    cmd.type = type;
    cmd.content.ptr = (void *)&opt;

    msg_send_receive(&cmd, &ack, pid);

    return (int)ack.content.value;
}

int netapi_get_option(kernel_pid_t pid, netconf_opt_t param,
                      void *data, size_t data_len)
{
    return _get_set_option(pid, NETAPI_MSG_TYPE_GETOPT, param, data, data_len);
}

int netapi_set_option(kernel_pid_t pid, netconf_opt_t param,
                      void *data, size_t data_len)
{
    return _get_set_option(pid, NETAPI_MSG_TYPE_SETOPT, param, data, data_len);
}
