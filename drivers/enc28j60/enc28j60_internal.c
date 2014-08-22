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
    cmd_wcr(dev, REG_B0_ERXSTL, 0, ptr.bytes.low);
    cmd_wcr(dev, REG_B0_ERXSTH, 0, ptr.bytes.high);
    ptr.addr = BUFFER_SIZE - 1;
    cmd_wcr(dev, REG_B0_ERXNDL, 0, ptr.bytes.low);
    cmd_wcr(dev, REG_B0_ERXNDH, 0, ptr.bytes.high);

    /* setup receive filters */
    reg = 0;    /* accept everything */
    cmd_wcr(dev, REG_B1_ERXFCON, 1, reg);

    /* wait for oscillator to be stable (by polling ESTAT.CLKRDY bit) */
    do {
        reg = cmd_rcr(dev, REG_ESTAT, -1);
    } while (!(reg & ESTAT_CLKRDY));

    /* setup MAC address and MAC behavior */
    reg = MACON1_TXPAUS | MACON1_RXPAUS | MACON1_MARXEN;
    cmd_wcr(dev, REG_B2_MACON1, 2, reg);
    reg = MACON3_FULDPX | MACON3_PADCFG0 | MACON3_TXCRCEN;
    cmd_wcr(dev, REG_B2_MACON3, 2, reg);
    reg = MACON4_DEFER;
    cmd_wrc(dev, REG_B2_MACON4, 2, reg);
    ptr.addr = ENC28J60_MAX_FRAME_LEN;
    cmd_wcr(dev, REG_B2_MAMXFLL, 2, ptr.bytes.low);
    cmd_wcr(dev, REG_B2_MAMXFLH, 2, ptr.bytes.high);
    reg = ENC28J60_B2B_GAP;
    cmd_wcr(dev, REG_B2_MABBIPG, 2, reg);
    reg = ENC28J60_NB2B_GAP;
    cmd_wcr(dev, REG_B2_MAIPGL, 2, reg);
    init_mac(mac);
    enc28j60_set_mac_addr(dev, mac);
}

void enc28j60_transmit(enc28j60_dev_t *dev,
                       char *data, size_t data_len,
                       char *dst_mac_addr)
{


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

char cmd_rcr(enc28j60_dev_t *dev, char reg, short bank)
{
    char res;

    switch_bank(dev, bank);

    gpio_clear(dev->cs);
    spi_transfer_reg(dev->spi, CMD_RCR | reg, 0, &res);
    gpio_set(dev->cs);

    return res;
}

void cmd_wcr(enc28j60_dev_t *dev, char reg, short bank, char value)
{
    switch_bank(dev, bank);

    gpio_clear(dev->cs);
    spi_transfer_reg(dev->spi, CMD_WCR | reg, value, 0);
    gpio_set(dev->cs);
}

void cmd_bfs(enc28j60_dev_t *dev, char reg, short bank, char mask)
{
    switch_bank(dev, bank);

    gpio_clear(dev->cs);
    spi_transfer_reg(dev->spi, CMD_BFS | reg, mask, 0);
    gpio_set(dev->cs);
}

void cmd_bfc(enc28j60_dev_t *dev, char reg, short bank, char mask)
{
    switch_bank(dev, bank);

    gpio_clear(dev->cs);
    spi_transfer_reg(dev->spi, CMD_BFC | reg, mask, 0);
    gpio_set(dev->cs);
}

int cmd_rbm(enc28j60_dev_t *dev, char *data, size_t data_len)
{
    int res;

    gpio_clear(dev->cs);
    res = spi_transfer_regs(dev->spi, 0, data, data_len);
    gpio_set(dev->cs);

    return res;
}

void cmd_wbm(enc28j60_dev_t *dev, char *data, size_t data_len)
{
    gpio_clear(dev->cs);
    spi_transfer_regs(dev->spi, CMD_WBM, data, 0, data_len);
    gpio_set(dev->cs);
}

void cmd_src(enc28j60_dev_t *dev)
{
    gpio_clear(dev->cs);
    spi_transfer_byte(dev->spi, CMD_SRC);
    gpio_set(dev->cs);
}

uint16_t cmd_phy_read(enc28j60_dev_t *dev, char reg)
{
    enc28j60_ptr_t data;

    cmd_wcr(dev, REG_B2_MIREGADR, 2, reg);
    cmd_bfs(dev, REG_B2_MICMD, 2, MICMD_MIIRD);
    do {
        reg = cmd_rcr(dev, REG_B3_MISTAT, 3);
    } while (reg & MISTAT_BUSY);
    cmd_bfc(dev, REG_B2_MICMD, 2, MICMD_MIIRD);
    data.bytes.low = cmd_rcr(dev, REG_B2_MIRDL, 2);
    data.bytes.high = cmd_rcr(dev, REG_B2_MIRDH, 2);

    return data.addr;
}

void cmd_phy_write(enc28j60_dev_t *dev, char reg, uint16_t val)
{
    enc28j60_ptr_t data = val;

    cmd_wcr(dev, REG_B2_MIREGADR, 2, reg);
    cmd_wcr(dev, REG_B2_MIWRL, 2, data.bytes.low);
    cmd_wcr(dev, REG_B2_MIWRH, 2, data.bytes.high);
}

static void switch_bank(enc28j60_dev_t *dev, short bank)
{
    char ctrl_reg;

    /* only switch bank if needed */
    if (dev->active_bank == bank || bank < 0) {
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
