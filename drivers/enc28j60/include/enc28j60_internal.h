/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_enc28j60
 * @{
 *
 * @file
 * @brief       Internal interfaces of the ENC28J60 driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __ENC28J60_INTERNAL_H
#define __ENC28J60_INTERNAL_H

#include "enc28j60.h"


#define ENC28J60_SPI_SPEED      SPI_SPEED_5MHZ      /* make even faster? */

#define ENC28J60_MAC1    0xabcd
#define ENC28J60_MAC2    0xef12
#define ENC28J60_MAC3    0x3456

#define ENC28J60_RX_BUF_SIZE    (0x1000)             /* 4K -> half of available 8K */



typedef union {
    uint16_t addr;
    struct {
        char low;
        char high;
    } bytes;
} enc28j60_ptr_t;

char enc28j60_rcr(enc28j60_dev_t *dev, char reg, short bank);

void enc28j60_wcr(enc28j60_dev_t *dev, char reg, short bank, char value);

void enc28j60_bfs(enc28j60_dev_t *dev, char reg, short bank, char mask);

void enc28j60_bfs(enc28j60_dev_t *dev, char reg, short bank, char mask);

void enc28j60_src(enc28j60_dev_t *dev);

#endif /* __ENC28J60_INTERNAL_H */
/** @} */
