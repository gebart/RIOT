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
#include "net/ng_pkt.h"
#include "periph/uart.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"


#define ISR_EVENT_RX_DONE           (0x0001)

#define ENTER_CMD_MODE_DELAY        HWTIMER_TICKS(1100 * 1000)

#define API_START_DELIMITER         (0x7e)

#define API_ID_MODEM_STATUS         (0x8a)
#define API_ID_AT                   (0x08)
#define API_ID_AT_QUEUE             (0x09)
#define API_ID_AT_RESP              (0x88)
#define API_ID_TX_LONG_ADDR         (0x00)
#define API_ID_TX_SHORT_ADDR        (0x01)
#define API_ID_TX_RESP              (0x89)
#define API_ID_RX_LONG_ADDR         (0x80)
#define API_ID_RX_SHORT_ADDR        (0x81)



/*****************************************************************************/
/*                    Driver's internal utility functions                    */
/*****************************************************************************/
static void _dump(uint8_t *buf, int size)
{
    for (int i = 0; i < size; i++) {
        printf("0x%02x ", buf[i]);
    }
    puts("");
}


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
    DEBUG("AT_CMD: %s\n", cmd);

    for (int i = 0; cmd[i] != '\0'; i++) {
        uart_write_blocking(dev->uart, cmd[i]);
    }
}

// static void _api_cmd(xbee_t *dev, uint8_t cmd_id, const uint8_t *data,
//                      uint16_t length)
// {
//     /* send start delimiter and frame length and command ID */
//     DEBUG("_api_cmd: send start and length\n");
//     uart_write_blocking(dev->uart, (char)API_START_DELIMITER);
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

static int _api_at_cmd(xbee_t *dev, const char *cmd)
{
    uint16_t size;
    int res;

    DEBUG("API_AT_CMD: frame %u - %s\n", dev->frame_id, cmd);

    /* get size of AT command */
    size = strlen(cmd) + 2;
    /* construct API frame */
    dev->tx_buf[0] = API_START_DELIMITER;
    dev->tx_buf[1] = size >> 8;
    dev->tx_buf[2] = size & 0xff;
    dev->tx_buf[3] = API_ID_AT;
    dev->tx_buf[4] = dev->frame_id;
    memcpy(dev->tx_buf + 5, cmd, strlen(cmd));
    dev->tx_buf[3 + size] = _cksum(dev->tx_buf, size + 3);
    /* send UART data and for it to finish */
    dev->tx_limit = 6 + size;
    dev->tx_count = 0;
    uart_tx_begin(dev->uart);

    /* wait for results */
    while (dev->rx_state != XBEE_RX_STATE_RESP_PENDING) {
        mutex_lock(&dev->rx_lock);
    }
    /* increment frame id */
    dev->frame_id = (++dev->frame_id == 0) ? 1 : dev->frame_id;
    /* return result code */
    if (dev->rx_buf[0] != API_ID_AT_RESP) {
        DEBUG("APPI_AT_CMD: invalid response\n");
        res = -1;
    }
    else {
        DEBUG("API_AT_CMD: frame %u - response code: %x\n", dev->rx_buf[1], dev->rx_buf[4]);
        res = (int)dev->rx_buf[4];
    }

    dev->rx_state = XBEE_RX_STATE_IDLE;
    return res;
}



int _send_foo(xbee_t *dev, const char *foo)
{
    uint16_t size;

    DEBUG("API_TX: frame %u - sending out %s\n", dev->frame_id, foo);

    /* get size of AT command */
    size = strlen(foo) + 5;
    /* construct API frame */
    dev->tx_buf[0] = API_START_DELIMITER;
    dev->tx_buf[1] = size >> 8;
    dev->tx_buf[2] = size & 0xff;
    dev->tx_buf[3] = API_ID_TX_SHORT_ADDR;
    dev->tx_buf[4] = dev->frame_id;
    dev->tx_buf[5] = 0x00;
    dev->tx_buf[6] = 0x23;
    dev->tx_buf[7] = 0x01;
    memcpy(dev->tx_buf + 8, foo, strlen(foo));
    dev->tx_buf[3 + size] = _cksum(dev->tx_buf, size + 3);
    /* send UART data and for it to finish */
    dev->tx_limit = 8 + size;
    dev->tx_count = 0;
    uart_tx_begin(dev->uart);

    /* wait for results */
    while (dev->rx_state != XBEE_RX_STATE_RESP_PENDING) {
        mutex_lock(&dev->rx_lock);
    }
    /* increment frame id */
    dev->frame_id = (++dev->frame_id == 0) ? 1 : dev->frame_id;
    /* return result code */
    if (dev->rx_buf[0] != API_ID_TX_RESP) {
        DEBUG("API_TX: invalid response\n");
        return -1;
    }
    else {
        DEBUG("API_TX: frame %u - response code: %x\n", dev->rx_buf[1], dev->rx_buf[2]);
        return dev->rx_buf[2];
    }
}



/*
 * interrupt callbacks
 */
int _tx_cb(void *arg)
{
    xbee_t *dev = (xbee_t *)arg;
    if (dev->tx_count < dev->tx_limit) {
        char c = (char)dev->tx_buf[dev->tx_count++];
        uart_write(dev->uart, c);
        return 1;
    }
    return 0;
}

void _rx_cb(void *arg, char c)
{
    xbee_t *dev = (xbee_t *)arg;
    msg_t msg;

    switch (dev->rx_state) {
        case XBEE_RX_STATE_IDLE:
            /* check for beginning of new data frame */
            if (c == API_START_DELIMITER) {
                dev->rx_state = XBEE_RX_STATE_SIZE1;
            }
            break;
        case XBEE_RX_STATE_SIZE1:
            dev->rx_limit = ((uint16_t)c) << 8;
            dev->rx_state = XBEE_RX_STATE_SIZE2;
            break;
        case XBEE_RX_STATE_SIZE2:
            dev->rx_limit += (uint8_t)c;
            dev->rx_count = 0;
            dev->rx_cksum = 0;
            dev->rx_state = XBEE_RX_STATE_DATA;
            break;
        case XBEE_RX_STATE_DATA:
            dev->rx_buf[dev->rx_count++] = (uint8_t)c;
            dev->rx_cksum += (uint8_t)c;
            if (dev->rx_count == dev->rx_limit) {
                dev->rx_state = XBEE_RX_STATE_CKSUM;
            }
            break;
        case XBEE_RX_STATE_CKSUM:
            dev->rx_cksum += (uint8_t)c;
            if (dev->rx_cksum == 0xff) {
                /* checksum correct, process packet */
                DEBUG("RX: checksum correct\n");
                switch (dev->rx_buf[0]) {
                    case API_ID_AT_RESP:
                    case API_ID_TX_RESP:
                        DEBUG("RX: got TX or AT cmd response\n");
                        dev->rx_state = XBEE_RX_STATE_RESP_PENDING;
                        mutex_unlock(&dev->rx_lock);
                        break;
                    case API_ID_RX_SHORT_ADDR:
                    case API_ID_RX_LONG_ADDR:
                        DEBUG("RX: received packet\n");
                        dev->rx_state = XBEE_RX_STATE_DATA_PENDING;
                        msg.type = NG_NETDEV_MSG_TYPE_EVENT;
                        msg.content.value = ISR_EVENT_RX_DONE;
                        msg_send_int(&msg, dev->mac_pid);
                        break;
                    default:
                        DEBUG("RX: got un-interesting data\n");
                        dev->rx_state = XBEE_RX_STATE_IDLE;
                        break;
                }
            }
            else {
                /* checksum wrong, drop packet */
                DEBUG("RX: checksum incorrect\n");
                dev->rx_state = XBEE_RX_STATE_IDLE;
            }
            break;
        default:
            /* nothing to do */
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
    mutex_init(&dev->rx_lock);
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
    hwtimer_wait(HWTIMER_TICKS(ENTER_CMD_MODE_DELAY));
    _at_cmd(dev, "+++");
    hwtimer_wait(HWTIMER_TICKS(ENTER_CMD_MODE_DELAY));
    /* disable non IEEE802.15.4 extensions */
    _at_cmd(dev, "ATMM2\r");
    /* put XBee module in "API mode without escaped characters" */
    _at_cmd(dev, "ATAP1\r");
    /* apply AT commands */
    _at_cmd(dev, "ATAC\r");
    /* exit command mode */
    _at_cmd(dev, "ATCN\r");

    /* send some data */
    // _send_foo(dev, "Hello");
    // hwtimer_wait(HWTIMER_TICKS(1000 * 1000));


    /* test: get ID via API format */
    _api_at_cmd(dev, "ID");
    /* set address to 23 */
    _api_at_cmd(dev, "MY23");
    /* read address */
    _api_at_cmd(dev, "MY");


    _send_foo(dev, "Hello World!");

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

void _isr_event(ng_netdev_t *netdev, uint16_t event_type)
{
    xbee_t *dev = (xbee_t *)netdev;
    ng_ifhdr_t *hdr = (ng_ifhdr_t *)dev->rx_data->data;
    uint8_t *data = dev->rx_data->next->data;

    /* test if there is actually data waiting in the RX buffer */
    if (dev->rx_state != XBEE_RX_STATE_DATA_PENDING) {
        return;
    }

    /* copy payload and fill interface header */
    if (dev->rx_buf[0] == API_ID_RX_SHORT_ADDR) {
        ng_ifhdr_init(hdr, 2, 2);
        ng_ifhdr_set_src_addr(hdr, dev->rx_buf + 1, 2);
        hdr->rssi = dev->rx_buf[3];
        memcpy(data, dev->rx_buf + 5, dev->rx_limit - 5);
    }
    else {
        ng_ifhdr_init(hdr, 8, 2);
        ng_ifhdr_set_src_addr(hdr, dev->rx_buf + 1, 8);
        hdr->rssi = dev->rx_buf[9];
        memcpy(data, dev->rx_buf + 11, dev->rx_limit - 11);
    }
    ng_ifhdr_set_dst_addr(hdr, (uint8_t *)&dev->own_addr, 2);
    /* mark data as processed */
    dev->rx_state = XBEE_RX_STATE_IDLE;
    /* forward data to MAC layer */
    dev->event_cb(NETDEV_EVENT_RX_COMPLETE, dev->rx_data);
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
