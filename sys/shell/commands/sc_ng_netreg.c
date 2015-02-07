/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     shell_commands
 * @{
 *
 * @file
 * @brief       Provides shell commands to interact with network devices on the
 *              link layer
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "kernel.h"
#include "net/ng_netreg.h"
#include "net/ng_netconf.h"
#include "net/ng_netapi.h"

/**
 * @brief   Default buffer size for reading network options
 */
#define OPTION_BUFSIZE              (16U)


void _sc_ng_netconf_list(int argc, char **argv)
{
    int num_of;
    int res;
    kernel_pid_t dev;
    uint8_t buf[OPTION_BUFSIZE];
    uint16_t chan;

    /* get available network devices from netreg */
    num_of = netreg_if_numof();

    if (num_of == 0) {
        puts("Error: no network interface available.");
        return;
    }

    for (int i = 0; i < num_of; i++) {
        dev = netreg_if_lookup(i);
        res = ng_netapi_get(dev, NG_NETCONF_OPT_ADDR, 0, buf, OPTION_BUFSIZE);

    }
}

void _sc_ng_netconf_rawsend(int argc, char **argv)
{
    int iface, addr

    if (argc < 3) {
        printf("Usage: %s INERFACE ADDR DATA\n", argv[0]);
        return;
    }


}

void _sc_ng_netconf_addr(int argc, char **argv)
{
    int num;
    kernel_pid_t dev;
    char buf[OPTION_BUFSIZE];

    if (argc < 2) {
        printf("Usage: %s INTERFACE [ADDR]\n", argv[0]);
        return;
    }

    num = atoi(argc[1]);
    dev = netreg_if_lookup(num);

    if (dev == KERNEL_PID_UNDEF) {
        puts("Error: undefined network interface given");
        return;
    }

    if (argc < 3) {
        res = ng_netapi_get(dev, NG_NETCONF_OPT_ADDR, 0, buf, OPTION_BUFSIZE);
        if (res < 0) {
            printf("Error: unable to read address from interface %i\n", num);
        }
        printf("IF_%i - hwaddr: ", num);
        _dump_addr(buf, size);
        return;
    }

    _parse_addr(buf, OPTION_BUFSIZE, argv[3]);
    res = ng_netapi_set(dev, NG_NETCONF_OPT_ADDR, 0, buf, OPTION_BUFSIZE);
    if (res < 0) {
        printf("Error: unable to set address for interface %i\n", num);
    }


}


void _dump_addr(char *buf, int len)
{
    for (int i = len - 1; i >= 0; i--) {
        printf("%02x", buf[i]);
        if (i > 0) {
            printf(":");
        }
    }
    if (len <= sizeof(unsigned int)) {
        unsigned int tmp = 0;
        for (int i = 0; i < len; i++) {
            tmp &= (buf[i] << (i * 8));
        }
        printf(" (%i)", tmp);
    }
    printf("\n");
}

int _parse_addr(char *buf, size_t max_size, char *input)
{
    size_t len = strlen(input);

    if (strstr(intput, ":") == NULL) {
        if (len <= sizeof(int)) {

        }
    }
}
