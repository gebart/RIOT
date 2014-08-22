/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_enc28j60
 * @{
 *
 * @file
 * @brief       Internal functions for the ENC28J60 driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "enc28j60.h"
#include "enc28j60_internal.h"
#include "enc28j60_regs.h"

#define BUFFER_SIZE         0x2000      /* the enc28j60 is equipped with 8K rx/tx buffer */

static void switch_bank(enc28j60_dev_t *dev, short bank);
static void init_mac(char *mac);

void enc28j60_init_device(enc28j60_dev_t *dev)
{
    char mac[6];
    enc28j60_ptr_t ptr;
    char reg;

    /* setup receive buffer */
    ptr.addr = BUFFER_SIZE - ENC28J60_RX_BUF_SIZE;
    wcr(dev, REG_B0_ERXSTL, 0, ptr.bytes.low);
    wcr(dev, REG_B0_ERXSTH, 0, ptr.bytes.high);
    ptr.addr = BUFFER_SIZE - 1;
    wcr(dev, REG_B0_ERXNDL, 0, ptr.bytes.low);
    wcr(dev, REG_B0_ERXNDH, 0, ptr.bytes.high);


    /* setup receiver filters */
    reg = 0;    /* accept everything */
    wcr(dev, REG_B1_ERXFCON, 1, reg);

    /* wait for oscillator to be stable (by polling ESTAT.CLKRDY bit) */


    /* setup MAC address and MAC behavior */
    init_mac(mac);
    enc28j60_set_mac_addr(dev, mac);

    /* setup PHY */
}


void enc28j60_set_mac_addr(enc28j60_dev_t *dev, char *mac)
{
    enc28j60_wcr(dev, REG_B2_MAADR1, 2, mac[0]);
    enc28j60_wcr(dev, REG_B2_MAADR2, 2, mac[1]);
    enc28j60_wcr(dev, REG_B2_MAADR3, 2, mac[2]);
    enc28j60_wcr(dev, REG_B2_MAADR4, 2, mac[3]);
    enc28j60_wcr(dev, REG_B2_MAADR5, 2, mac[4]);
    enc28j60_wcr(dev, REG_B2_MAADR6, 2, mac[5]);
}


void enc28j60_on_int(void *arg)
{
    enc28j60_dev_t *dev = (enc28j60_dev_t *)arg;

}

char enc28j60_rcr(enc28j60_dev_t *dev, char reg, short bank)
{
    char res;

    switch_bank(dev, bank);

    gpio_clear(dev->cs);
    spi_transfer_reg(dev->spi, CMD_RCR | reg, 0, &res);
    gpio_set(dev->cs);

    return res;
}

void enc28j60_wcr(enc28j60_dev_t *dev, char reg, short bank, char value)
{
    switch_bank(dev, bank);

    gpio_clear(dev->cs);
    spi_transfer_reg(dev->spi, CMD_WCR | reg, value, 0);
    gpio_set(dev->cs);
}

void enc28j60_bfs(enc28j60_dev_t *dev, char reg, short bank, char mask)
{
    switch_bank(dev, bank);

    gpio_clear(dev->cs);
    spi_transfer_reg(dev->spi, CMD_BFS | reg, mask, 0);
    gpio_set(dev->cs);
}

void enc28j60_bfs(enc28j60_dev_t *dev, char reg, short bank, char mask)
{
    switch_bank(dev, bank);

    gpio_clear(dev->cs);
    spi_transfer_reg(dev->spi, CMD_BFC | reg, mask, 0);
    gpio_set(dev->cs);
}


void enc28j60_src(enc28j60_dev_t *dev)
{
    gpio_clear(dev->cs);
    spi_transfer_byte(dev->spi, CMD_SRC);
    gpio_set(dev->cs);
}


static void switch_bank(enc28j60_dev_t *dev, short bank)
{
    char ctrl_reg;

    /* only switch bank if needed */
    if (dev->active_bank == bank) {
        return;
    }

    gpio_clear(dev->cs);
    spi_transfer_reg(dev->spi, CMD_RCR | REG_ECON1, 0, &ctrl_reg);
    gpio_set(dev->cs);

    ctrl_reg &= ~ECON1_BSEL_MASK;
    ctrl_reg |= (bank & 0x03);

    gpio_clear(dev->cs);
    spi_transfer_reg(dev->spi, CMD_WCR | REG_ECON1, ctrl_reg, 0);
    gpio_set(dev->cs);

    dev->active_bank = bank;
}

static void init_mac(char *mac)
{
    uint16_t a = ENC28J60_MAC1;
    uint16_t b = ENC28J60_MAC2;
    uint16_t c = ENC28J60_MAC3;

    mac[0] = (0xff & a);
    mac[1] = (0xff & (a >> 8));
    mac[2] = (0xff & b);
    mac[3] = (0xff & (b >> 8));
    mac[4] = (0xff & c);
    mac[5] = (0xff & (c >> 8));
}
