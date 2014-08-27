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

#include "mutex.h"
#include "msg.h"

#include "enc28j60.h"
#include "enc28j60_internal.h"
#include "enc28j60_regs.h"

#define MSG_TYPE_DRIVER_TO_MAC      0x0100

#define BUFFER_SIZE         0x2000      /* the enc28j60 is equipped with 8K rx/tx buffer */

static void switch_bank(enc28j60_t *dev, short bank);
static void init_mac(char *mac);

static char cmd_rcr(enc28j60_t *dev, char reg, short bank);
static void cmd_wcr(enc28j60_t *dev, char reg, short bank, char value);
static void cmd_bfs(enc28j60_t *dev, char reg, short bank, char mask);
static void cmd_bfc(enc28j60_t *dev, char reg, short bank, char mask);
static int cmd_rbm(enc28j60_t *dev, char *data, size_t data_len);
static void cmd_wbm(enc28j60_t *dev, char *data, size_t data_len);
static void cmd_src(enc28j60_t *dev);
static uint16_t cmd_phy_read(enc28j60_t *dev, char reg);
static void cmd_phy_write(enc28j60_t *dev, char reg, uint16_t val);
static void switch_bank(enc28j60_t *dev, short bank);
static void init_mac(char *mac);


int enc28j60_init_device(enc28j60_t *dev)
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
    cmd_wcr(dev, REG_B2_MACON4, 2, reg);
    ptr.addr = ENC28J60_MAX_FRAME_LEN;
    cmd_wcr(dev, REG_B2_MAMXFLL, 2, ptr.bytes.low);
    cmd_wcr(dev, REG_B2_MAMXFLH, 2, ptr.bytes.high);
    reg = ENC28J60_B2B_GAP;
    cmd_wcr(dev, REG_B2_MABBIPG, 2, reg);
    reg = ENC28J60_NB2B_GAP;
    cmd_wcr(dev, REG_B2_MAIPGL, 2, reg);
    init_mac(mac);
    enc28j60_set_mac_addr(dev, mac);

    /* enable hardware flow control */
    cmd_wcr(dev, REG_B3_EFLOCON, 3, EFLOCON_FULDPXS | EFLOCON_FCEN1);

    /* enable auto-increment of read and write pointers for the RBM/WBM commands */
    cmd_bfs(dev, REG_ECON2, -1, ECON2_AUTOINC);

    /* enable receive interrupt */
    cmd_bfs(dev, REG_EIE, -1, EIE_INTIE | EIE_PKTIE);
    /* allow receiving bytes from now on */
    cmd_bfs(dev, REG_ECON1, -1, ECON1_RXEN);

    return 0;
}

void enc28j60_transmit(enc28j60_t *dev,
                       char *data, size_t data_len,
                       uint16_t type,
                       char *dst_mac_addr)
{
    enc28j60_ptr_t start, end;
    start.addr = 0x0000;            /* start at address 0 */
    end.addr = 15 + data_len;       /* 1*control byte + 2*type byte + 2*mac + data length */

    /* wait for any ongoing transmission to finish */
    mutex_lock(&dev->tx_mutex);

    /* set ETXST pointer to address 0x4 */
    cmd_wcr(dev, REG_B0_ETXSTL, 0, start.bytes.low);
    cmd_wcr(dev, REG_B0_ETXSTH, 0, start.bytes.high);

    /* set the write data pointer for WBM command to same start address */
    cmd_wcr(dev, REG_B0_EWRPTL, 0, start.bytes.low);
    cmd_wcr(dev, REG_B0_EWRPTH, 0, start.bytes.high);

    /* use WBM command to write data to chip */
    gpio_clear(dev->cs);
    spi_transfer_reg(dev->spi, CMD_WBM, 0, 0);          /* control byte := 0 */
    spi_transfer_bytes(dev->spi, dst_mac_addr, 0, 6);   /* dst address */
    spi_transfer_bytes(dev->spi, dev->mac, 0, 6);       /* src address */
    spi_transfer_bytes(dev->spi, (char *)((uint8_t)type), 0, 2);   /* Ethernet type filed */
    spi_transfer_bytes(dev->spi, data, 0, data_len);    /* actual data */
    gpio_set(dev->cs);

    /* set the ETXND pointer to the last address we wrote data to */
    cmd_wcr(dev, REG_B0_ETXNDL, 0, end.bytes.low);
    cmd_wcr(dev, REG_B0_ETXNDH, 0, end.bytes.high);

    /* start transmission of data */
    cmd_bfs(dev, REG_ECON1, -1, ECON1_TXRTS);

    /* unlock transmit mutex */
    mutex_unlock(&dev->tx_mutex);
}

int end28j60_receive(enc28j60_t *dev,
                      char *buffer, size_t max_len,
                      char *src_mac_addr, char *dst_mac_addr, uint16_t *type)
{
    int size;
    char status[4];

    /* set the read data pointer for RBM command to start of packet */
    cmd_wcr(dev, REG_B0_ERDPTL, 0, dev->next_pkt.bytes.low);
    cmd_wcr(dev, REG_B0_ERDPTH, 0, dev->next_pkt.bytes.high);

    /* read new next packet pointer and status bytes */
    cmd_rbm(dev, (char *)&dev->next_pkt.bytes.low, 1);
    cmd_rbm(dev, (char *)&dev->next_pkt.bytes.high, 1);
    cmd_rbm(dev, status, 4);

    /* get number of received bytes from status field */
    /* TODO: make this work with other systems then little-endian */
    size = ((int)status[1] << 8) & (int)status[0];
    /* from the size we can deduct the actual amount of payload bytes */
    size -= 18;       /* 2 byte Ethernet type, 12 byte mac addresses, 4 byte CRC */

    /* read addresses */
    cmd_rbm(dev, dst_mac_addr, 6);
    cmd_rbm(dev, src_mac_addr, 6);

    /* read Ethernet type field */
    cmd_rbm(dev, (char *)type, 2);

    /* read the actual data, but just ignore the 4 bytes with the CRC for now */
    cmd_rbm(dev, buffer, size);

    /* and finally free the buffer space used by the packet just read */
    cmd_wcr(dev, REG_B0_ERXRDPTL, 0, dev->next_pkt.bytes.low);
    cmd_wcr(dev, REG_B0_ERXRDPTH, 0, dev->next_pkt.bytes.high);
    cmd_bfs(dev, REG_ECON2, -1, ECON2_PKTDEC);

    return size;
}


void enc28j60_set_mac_addr(enc28j60_t *dev, char *mac)
{
    cmd_wcr(dev, REG_B3_MAADR1, 3, mac[0]);
    cmd_wcr(dev, REG_B3_MAADR2, 3, mac[1]);
    cmd_wcr(dev, REG_B3_MAADR3, 3, mac[2]);
    cmd_wcr(dev, REG_B3_MAADR4, 3, mac[3]);
    cmd_wcr(dev, REG_B3_MAADR5, 3, mac[4]);
    cmd_wcr(dev, REG_B3_MAADR6, 3, mac[5]);
}


void enc28j60_on_int(void *arg)
{
    char eir;
    msg_t msg;
    enc28j60_t *dev = (enc28j60_t *)arg;

    /* read interrupt flat register */
    eir = cmd_rcr(dev, EIR_PKTIF, -1);

    /* see if new packet was received */
    if (eir & EIR_PKTIF) {
        msg.type = MSG_TYPE_DRIVER_TO_MAC;
        msg.content.value = ENC28J60_EVT_RX;
        msg_send(&msg, dev->pid, 0);
    }
    /* ignore all other interrupts for now */
}

char cmd_rcr(enc28j60_t *dev, char reg, short bank)
{
    char res;

    switch_bank(dev, bank);

    gpio_clear(dev->cs);
    spi_transfer_reg(dev->spi, CMD_RCR | reg, 0, &res);
    gpio_set(dev->cs);

    return res;
}

void cmd_wcr(enc28j60_t *dev, char reg, short bank, char value)
{
    switch_bank(dev, bank);

    gpio_clear(dev->cs);
    spi_transfer_reg(dev->spi, CMD_WCR | reg, value, 0);
    gpio_set(dev->cs);
}

void cmd_bfs(enc28j60_t *dev, char reg, short bank, char mask)
{
    switch_bank(dev, bank);

    gpio_clear(dev->cs);
    spi_transfer_reg(dev->spi, CMD_BFS | reg, mask, 0);
    gpio_set(dev->cs);
}

void cmd_bfc(enc28j60_t *dev, char reg, short bank, char mask)
{
    switch_bank(dev, bank);

    gpio_clear(dev->cs);
    spi_transfer_reg(dev->spi, CMD_BFC | reg, mask, 0);
    gpio_set(dev->cs);
}

int cmd_rbm(enc28j60_t *dev, char *data, size_t data_len)
{
    int res;

    gpio_clear(dev->cs);
    res = spi_transfer_regs(dev->spi, CMD_RBM, 0, data, data_len);
    gpio_set(dev->cs);

    return res;
}

void cmd_wbm(enc28j60_t *dev, char *data, size_t data_len)
{
    gpio_clear(dev->cs);
    spi_transfer_regs(dev->spi, CMD_WBM, data, 0, data_len);
    gpio_set(dev->cs);
}

void cmd_src(enc28j60_t *dev)
{
    gpio_clear(dev->cs);
    spi_transfer_byte(dev->spi, CMD_SRC, 0);
    gpio_set(dev->cs);
}

uint16_t cmd_phy_read(enc28j60_t *dev, char reg)
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

void cmd_phy_write(enc28j60_t *dev, char reg, uint16_t val)
{
    enc28j60_ptr_t data;
    data.addr = val;

    cmd_wcr(dev, REG_B2_MIREGADR, 2, reg);
    cmd_wcr(dev, REG_B2_MIWRL, 2, data.bytes.low);
    cmd_wcr(dev, REG_B2_MIWRH, 2, data.bytes.high);
}

static void switch_bank(enc28j60_t *dev, short bank)
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
