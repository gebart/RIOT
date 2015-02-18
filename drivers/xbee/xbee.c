/*
 * Copyright (C) 2014 INRIA
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_xbee
 * @{
 *
 * @file
 * @brief       High-level driver implementation for the XBee 802.15.4 modem
 *
 * @author      Kévin Roussel <kevin.roussel@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "xbee.h"
#include "mutex.h"
#include "hwtimer.h"
#include "net/ng_ifhdr.h"
#include "periph/uart.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

/**
 * @brief   maximum length of AT commands
 *
 * The largest known length for parameters is 20 byte, so this value should
 * be sufficient.
 */
#define MAX_AT_CMD_LEN          (32U)


#define API_MODE_START_DELIMITER    (0x7e)

#define API_ID_MODEM_STATUS        (0x8a)
#define API_ID_AT                  (0x08)
#define API_ID_AT_QUEUE            (0x09)
#define API_ID_AT_RESP             (0x88)
#define API_ID_TX_LONG_ADDR        (0x00)
#define API_ID_TX_SHORT_ADDR       (0x01)
#define API_ID_TX_RESP             (0x89)
#define API_ID_RX_LONG_ADDR        (0x80)
#define API_ID_RX_SHORT_ADDR       (0x81)






/*****************************************************************************/
/*                    Driver's internal utility functions                    */
/*****************************************************************************/
static uint8_t _cksum(uint8_t *buf, uint16_t size)
{
    uint8_t res = 0;
    for (int i = 3; i < size; i++) {
        res += buf[i];
    }
    return 0xff - res;
}

static void _at_cmd(xbee_t *dev, const char *cmd)
{
    for (int i = 0; cmd[i] != '\0'; i++) {
        uart_write_blocking(dev->uart, cmd[i]);
    }
}

// static void _api_cmd(xbee_t *dev, uint8_t cmd_id, const uint8_t *data,
//                      uint16_t length)
// {
//     /* send start delimiter and frame length and command ID */
//     DEBUG("_api_cmd: send start and length\n");
//     uart_write_blocking(dev->uart, (char)API_MODE_START_DELIMITER);
//     uart_write_blocking(dev->uart, (char)(length >> 8));
//     uart_write_blocking(dev->uart, (char)(length & 0xff));
//     uart_write_blocking(dev->uart, (char)cmd_id);
//     /* send frame data */
//     dev->tx_cksum = cmd_id;
//     dev->tx_buf = data;
//     dev->tx_limit = length;
//     uart_tx_begin(dev->uart);
//     DEBUG("_api_cmd: wait for TX to complete\n");
//     while (dev->tx_count < dev->tx_limit) {
//         mutex_lock(&dev->tx_lock);
//     }
//     /* send checksum */
//     DEBUG("_api_cmd: send cksum: 0x%x\n", dev->tx_cksum);
//     uart_write_blocking(dev->uart, (char)dev->tx_cksum);
// }

static void _api_at_cmd(xbee_t *dev, const char *cmd)
{
    uint8_t buf[MAX_AT_CMD_LEN];
    uint16_t size;

    /* get size of AT command */
    size = strlen(cmd) + 2;
    /* construct API frame */
    buf[0] = API_MODE_START_DELIMITER;
    buf[1] = size >> 8;
    buf[2] = size & 0xff;
    buf[3] = API_ID_AT;
    buf[4] = dev->frame_id;
    memcpy(buf + 5, cmd, strlen(cmd));
    buf[3 + size] = _cksum(buf, size + 3);
    /* send UART data and for it to finish */
    dev->tx_limit = 6 + size;
    dev->tx_count = 0;
    dev->tx_buf = buf;
    uart_tx_begin(dev->uart);
    while (dev->tx_count < dev->tx_limit) {
        mutex_lock(&dev->tx_lock);
    }

    /* TODO: wait for results */

    /* increment frame id */
    dev->frame_id = (++dev->frame_id == 0) ? 1 : dev->frame_id;
}



int _send_foo(xbee_t *dev, const char *foo)
{
    uint8_t buf[100];
    uint16_t size;

    /* get size of AT command */
    size = strlen(foo) + 5;
    /* construct API frame */
    buf[0] = API_MODE_START_DELIMITER;
    buf[1] = size >> 8;
    buf[2] = size & 0xff;
    buf[3] = API_ID_TX_SHORT_ADDR;
    buf[4] = dev->frame_id;
    buf[5] = 0x00;
    buf[6] = 0x23;
    buf[7] = 0x01;
    memcpy(buf + 8, foo, strlen(foo));
    buf[3 + size] = _cksum(buf, size + 3);
    /* send UART data and for it to finish */
    dev->tx_limit = 8 + size;
    dev->tx_count = 0;
    dev->tx_buf = buf;
    uart_tx_begin(dev->uart);
    while (dev->tx_count < dev->tx_limit) {
        mutex_lock(&dev->tx_lock);
    }

    /* TODO: wait for results */

    /* increment frame id */
    dev->frame_id = (++dev->frame_id == 0) ? 1 : dev->frame_id;
    return 0;
}



/*
 * interrupt callbacks
 */
int _tx_cb(void *arg)
{
    xbee_t *dev = (xbee_t *)arg;
    if (dev->tx_count < dev->tx_limit) {
        char c = (char)dev->tx_buf[dev->tx_count++];
        DEBUG("tx: 0x%02x\n", c);
        uart_write(dev->uart, c);
        dev->tx_cksum += (uint8_t)c;
        return 1;
    }
    DEBUG("tx: done\n");
    /* unlock TX mutex when transmission is done */
    mutex_unlock(&dev->tx_lock);
    return 0;
}

void _rx_cb(void *arg, char c)
{
    xbee_t *dev = (xbee_t *)arg;

    DEBUG("rx: (0x%02x) %c\n", c, c);

    switch (dev->rx_state) {
        case XBEE_RX_STATE_IDLE:
            /* check for beginning of new data frame */
            if (c == API_MODE_START_DELIMITER) {
                dev->rx_state = XBEE_RX_STATE_SIZE1;
            }
            break;
        case XBEE_RX_STATE_SIZE1:
            dev->rx_len = ((uint16_t)c) << 8;
            dev->rx_state = XBEE_RX_STATE_SIZE2;
            break;
        case XBEE_RX_STATE_SIZE2:
            dev->rx_len += (uint8_t)c;
            dev->rx_cksum = 0;
            dev->rx_state = XBEE_RX_STATE_DATA;
            break;
        case XBEE_RX_STATE_DATA:

            break;

    }
}


/*****************************************************************************/
/*                        Driver's "public" functions                        */
/*****************************************************************************/

int xbee_init(xbee_t *dev, uart_t uart, uint32_t baudrate,
              gpio_t sleep_pin, gpio_t status_pin)
{

    /* check device and bus parameters */
    if (dev == NULL) {
        return -ENODEV;
    }
    if (uart >= UART_NUMOF) {
        return -ENXIO;
    }
    /* initialize device descriptor */
    dev->uart = uart;
    dev->sleep_pin = sleep_pin;
    dev->status_pin = status_pin;
    dev->frame_id = 1;
    dev->options = 0;
    dev->tx_count = 0;
    /* initialize mutexes */
    mutex_init(&dev->tx_lock);
    /* initialize UART and GPIO pins */
    if (uart_init(uart, baudrate, _rx_cb, _tx_cb, dev) < 0) {
        DEBUG("xbee: error initializing UART\n");
        return -ENXIO;
    }
    if (sleep_pin < GPIO_NUMOF) {
        if (gpio_init_out(sleep_pin, GPIO_NOPULL) < 0) {
            DEBUG("xbee: error initializing SLEEP pin\n");
            return -ENXIO;
        }
    }
    if (status_pin < GPIO_NUMOF) {
        if (gpio_init_in(status_pin, GPIO_NOPULL) < 0) {
            DEBUG("xbee: error initializing STATUS pin\n");
            return -ENXIO;
        }
    }

     /* put the XBee device into command mode */
    hwtimer_wait(HWTIMER_TICKS(1500 * 1000));
    DEBUG("+++\n");
    _at_cmd(dev, "+++");
    hwtimer_wait(HWTIMER_TICKS(1500 * 1000));

    /* test: get ID */
    DEBUG("ATID\n");
    _at_cmd(dev, "ATID\r");
    hwtimer_wait(HWTIMER_TICKS(250 * 1000));
    /* disable non IEEE802.15.4 extensions */
    DEBUG("ATMM2\n");
    _at_cmd(dev, "ATMM2\r");
    hwtimer_wait(HWTIMER_TICKS(250 * 1000));
    /* REMOVE */
    DEBUG("ATAP\n");
    _at_cmd(dev, "ATAP\r");
    hwtimer_wait(HWTIMER_TICKS(250 * 1000));
    /* put XBee module in "API mode without escaped characters" */
    DEBUG("ATAP1\n");
    _at_cmd(dev, "ATAP1\r");
    hwtimer_wait(HWTIMER_TICKS(250 * 1000));
    /* REMOVE */
    DEBUG("ATAP\n");
    _at_cmd(dev, "ATAP\r");
    hwtimer_wait(HWTIMER_TICKS(250 * 1000));
    /* apply AT commands */
    DEBUG("ATAC\n");
    _at_cmd(dev, "ATAC\r");
    hwtimer_wait(HWTIMER_TICKS(250 * 1000));
    /* exit command mode */
    DEBUG("ATCN\n");
    _at_cmd(dev, "ATCN\r");


    hwtimer_wait(HWTIMER_TICKS(1500 * 1000));
    DEBUG("+++\n");
    _at_cmd(dev, "+++");
    hwtimer_wait(HWTIMER_TICKS(1500 * 1000));
    /* REMOVE */
    DEBUG("ATAP\n");
    _at_cmd(dev, "ATAP\r");
    hwtimer_wait(HWTIMER_TICKS(250 * 1000));
    /* exit command mode */
    DEBUG("ATFR\n");
    _at_cmd(dev, "ATFR\r");


    hwtimer_wait(HWTIMER_TICKS(1500 * 1000));

    /* send some data */
    _send_foo(dev, "Hello");
    hwtimer_wait(HWTIMER_TICKS(1000 * 1000));


    /* test: get ID via API format */
    DEBUG("API - ATID\n");
    _api_at_cmd(dev, "ID");
    hwtimer_wait(HWTIMER_TICKS(250 * 1000));
    /* set address to 23 */
    DEBUG("API - ATMY23\n");
    _api_at_cmd(dev, "MY23");
    hwtimer_wait(HWTIMER_TICKS(250 * 1000));
    /* read address */
    DEBUG("API - ATMY\n");
    _api_at_cmd(dev, "MY");
    hwtimer_wait(HWTIMER_TICKS(250 * 1000));

    return 0;
}

int _send(ng_netdev_t *dev, ng_pktsnip_t *pkt)
{
    // xbee_t *xbee = (xbee_t *)dev;
    // ng_pktsnip_t *snip = pkt->next;
    // ng_l2hdr_t *l2hdr;
    // uint8_t buffer[11];                     /* tmp for address and hdr field */
    // uint16_t size = 3;                      /* cmd + 2 byte size field */
    // uint8_t cmd_id;

    // /* test arguments for validity */
    // if (xbee == NULL) {
    //     return -ENODEV;
    // }
    // if (pkt == NULL) {
    //     return -ENOMSG;
    // }
    // /* add payload size to overall frame size */
    // size += ng_pkt_len(pkt->next);          /* payload size w/o l2hdr */
    // /* account for destination address */
    // l2hdr = (l2hdr_t *)pkt->data;
    // dst_addr = l2hdr_get_dst_addr(l2hdr);
    // if (l2hdr->dst_addr_len == 2) {
    //     cmd_id = 0x01;
    //     size += 2;
    // }
    // else if (l2hdr->dst_addr_len == 8) {
    //     cmd_id = 0x00;
    //     size += 8;

    // }
    // else {
    //     DEBUG("xbee: error: packet to send has invalid destination address\n");
    //     return -ENOMSG;
    // }

    // /* reset checksum */
    // dev->tx_cksum = 0;
    // /* send API command and frame size */
    // uart_write_blocking(dev->uart, API_CMD_SND);
    // uart_write_blocking(dev->uart, (char)(size >> 8));
    // uart_write_blocking(dev->uart, (char)(size & 0xff));
    // /* write header fields and destination address to temporary buffer */
    // buffer[0] = cmd_id;
    // buffer[1] = dev->frame_id;
    // memcpy(buffer + 2, l2hdr_get_dst_addr(l2hdr), l2hdr->dst_addr_len);
    // if (dev->option & OPT_AUTOACK) {
    //     buffer[2 + l2hdr->dst_addr_len] = 0x01;
    // }
    // else {
    //     buffer[2 + l2hdr->dst_addr_len] = 0x00;
    // }
    // /* send out data */
    // dev->tx_limit = 3 + l2hdr->dst_addr_len;
    // dev->tx_buf = buffer;
    // uart_tx_begin(dev->uart);
    // while (&dev->tx_count > 0) {
    //     mutex_lock(&dev->tx_lock);
    // }
    // /* send payload */
    // while (snip) {
    //     dev->tx_limit = snip->size;
    //     dev->tx_buf = (uint8_t *)snip->data;
    //     uart_tx_begin(dev->uart);
    //     while (&dev->tx_count > 0) {
    //         mutex_lock(&dev->tx_lock);
    //     }
    //     snip = snip->next;
    // }
    // /* and finish up by sending the frames checksum */
    // uart_write_blocking(dev->uart, (char)(0xff - dev->tx_cksum));

    // /* wait for result */

    // /* clean-up and return */
    // ++dev->frame_id;
    // return count;
    return 0;
}

int _add_cb(ng_netdev_t *dev, ng_netdev_event_cb_t cb)
{
    if (dev == NULL) {
        return -ENODEV;
    }
    if (dev->event_cb != NULL) {
        return -ENOBUFS;
    }
    dev->event_cb = cb;
    return 0;
}

int _rem_cb(ng_netdev_t *dev, ng_netdev_event_cb_t cb)
{
    if (dev == NULL) {
        return -ENODEV;
    }
    if (dev->event_cb != cb) {
        return -ENOENT;
    }
    dev->event_cb = NULL;
    return 0;
}

int _get(ng_netdev_t *dev, ng_netconf_opt_t opt, void *value, size_t *value_len)
{
    return -1;
}

int _set(ng_netdev_t *dev, ng_netconf_opt_t opt, void *value, size_t value_len)
{
    return -1;
}

void _isr_event(ng_netdev_t *dev, uint16_t event_type)
{
    /* nothing to do here, yet */
}

/* implementation of the netdev interface */
const ng_netdev_driver_t xbee_driver = {
    .send_data = _send,
    .add_event_callback = _add_cb,
    .rem_event_callback = _rem_cb,
    .get = _get,
    .set = _set,
    .isr_event = _isr_event,
};
