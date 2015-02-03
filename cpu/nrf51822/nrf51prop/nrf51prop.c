/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_nrf51
 * @{
 *
 * @file
 * @brief       Implementation of the NRF51 radio driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "cpu.h"
#include "mutex.h"
#include "thread.h"
#include "sched.h"
#include "kernel_types.h"
#include "nrf51prop.h"
#include "periph_conf.h"
#include "pktbuf.h"
#include "ll_gen_frame.h"

#ifdef MODULE_TRANSCEIVER
#include "transceiver.h"
#endif

#define ENABLE_DEBUG    (1)
#include "debug.h"

/**
 * @name Pre-configure the radio with these values
 * @{
 */
#define NRF51_CONF_LEN                  (8U)
#define NRF51_CONF_S0                   (0U)
#define NRF51_CONF_S1                   (0U)
#define NRF51_CONF_STATLEN              (0U)
#define NRF51_CONF_BASE_ADDR_LEN        (4U)
#define NRF51_CONF_ENDIAN               RADIO_PCNF1_ENDIAN_Big
#define NRF51_CONF_WHITENING            RADIO_PCNF1_WHITEEN_Disabled
#define NRF51_CONF_TXPOWER              NRF51_TXPWR_0DBM
#define NRF51_CONF_CHANNEL              (7U)        /* -> 2407MHz */
#define NRF51_CONF_MODE                 RADIO_MODE_MODE_Nrf_2Mbit
#define NRF51_CONF_CRC_LEN              (2U)        /* 1 to 3 bytes */
#define NRF51_CONF_CRC_POLY             (0x11021UL) /* x^16 + x^12 + x^5 + 1 */
/** @} */

/**
 * @name Pre-configured addresses, change with caution
 * @{
 */
#define NRF51_CONF_PREFIX0              (0xC4C3C2E7UL)
#define NRF51_CONF_PREFIX1              (0xC5C6C7C8UL)
#define NRF51_CONF_BASE0                (0xE7E70000UL)
#define NRF51_CONF_BASE1                (0xC2C20000UL)
/** @} */

/**
 * @brief Driver specific (interrupt) events
 * @{
 */
#define ISR_EVENT_RX_START              (0x0001)
#define ISR_EVENT_RX_DONE               (0x0002)
#define ISR_EVENT_TX_START              (0x0004)
#define ISR_EVENT_TX_DONE               (0x0008)
#define ISR_EVENT_WRONG_CHKSUM          (0x0010)
/** @} */

/**
 * @brief The drivers internal states
 * @{
 */
#define STATE_OFF                       (0x01)
#define STATE_IDLE                      (0x02)
#define STATE_RX                        (0x04)
#define STATE_TX                        (0x08)
/** @} */

/**
 * @brief Possible transmit power values, ranging from +4dBm to -30dBm
 */
typedef enum {
    NRF51_TXPWR_P4DBM     = 0x0f,   /**< transmit with  +4dBm */
    NRF51_TXPWR_0DBM      = 0x00,   /**< transmit with   0dBm */
    NRF51_TXPWR_M4DBM     = 0xfc,   /**< transmit with  -4dBm */
    NRF51_TXPWR_M8DBM     = 0xf8,   /**< transmit with  -8dBm */
    NRF51_TXPWR_M12DBM    = 0xf4,   /**< transmit with -12dBm */
    NRF51_TXPWR_M16DBM    = 0xf0,   /**< transmit with -16dBm */
    NRF51_TXPWR_M20DBM    = 0xec,   /**< transmit with -20dBm */
    NRF51_TXPWR_M30DBM    = 0xd8    /**< transmit with -30dBm */
} nrf51prop_txpower_t;

/**
 * @brief Set the radio's address
 *
 * @param[in] address       new address
 *
 * @return                  address that was set
 * @return                  -1 on error
 */
int _set_address(nrf51prop_t *dev, uint16_t address);

/**
 * @brief Get the current address
 *
 * @return                  current address
 */
uint16_t _get_address(nrf51prop_t *dev);

/**
 * @brief Set the address on which the radio listens for broadcast messages
 *
 * @param[in] address       broadcast address
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
// int _set_broadcast_address(nrf51prop_t *dev, uint16_t address);

/**
 * @brief Get the radio's broadcast address
 *
 * @return                  broadcast address
 */
// uint16_t _get_broadcast_address(nrf51prop_t *dev);

/**
 * @brief Set the radio channel to use
 *
 * The actual used frequency is 2400 + channel MHz
 *
 * @param[in] channel       channel to set
 *
 * @return                  new channel
 * @return                  -1 on error
 */
int _set_channel(nrf51prop_t *dev, uint8_t channel);

/**
 * @brief Get the currently configured channel
 *
 * @return                  the current channel
 */
int _get_channel(void);

/**
 * @brief Set transmit power
 *
 * @param[in] power         power to use when transmitting
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int _set_txpower(nrf51prop_txpower_t power);

/**
 * @brief Get the currently set transmit power
 *
 * @return                  configured transmit power
 */
nrf51prop_txpower_t _get_txpower(void);

/**
 * @brief Power on the radio
 */
void _poweron(nrf51prop_t *dev);

/**
 * @brief Power off the radio
 */
void _poweroff(nrf51prop_t *dev);

/**
 * @brief Change the radios internal state to DISABLED
 */
static void _switch_to_idle(nrf51prop_t *dev);

/**
 * @brief Put the radio into receiving state
 */
static void _switch_to_rx(nrf51prop_t *dev);

static int _send(netdev_t *dev, pktsnip_t *pkt);

static int _add_event_cb(netdev_t *dev, netdev_event_cb_t cb);

int _rem_event_cb(netdev_t *dev, netdev_event_cb_t cb);

int _get_option(netdev_t *dev, netconf_opt_t opt, void *value, size_t *value_len);

int _set_option(netdev_t *dev, netconf_opt_t opt, void *value, size_t value_len);

int _get_state(netdev_t *dev, netdev_state_t *state);

int _trigger(netdev_t *dev, netdev_action_t action);

void _isr_event(netdev_t *dev, uint16_t event_type);


/**
 * @brief   Save a pointer to the radios device descriptor to access it from
 *          interrupt context
 */
static nrf51prop_t *_radio;

const netdev_driver_t nrf51prop_driver = {
    .send_data = _send,
    .add_event_callback = _add_event_cb,
    .rem_event_callback = _rem_event_cb,
    .get_option = _get_option,
    .set_option = _set_option,
    .get_state = _get_state,
    .trigger = _trigger,
    .isr_event = _isr_event,
};

int nrf51prop_init(nrf51prop_t *dev)
{
    DEBUG("nrf51: init()\n");

    /* save a pointer to the configured device */
    _radio = dev;

    /* link the driver interface */
    dev->driver = &nrf51prop_driver;

    /* power on the NRFs radio */
    NRF_RADIO->POWER = 1;

    /* configure the basic (default) radio parameters */
    NRF_RADIO->TXPOWER = NRF51_CONF_TXPOWER;
    NRF_RADIO->FREQUENCY = NRF51_CONF_CHANNEL;
    NRF_RADIO->MODE = RADIO_MODE_MODE_Nrf_2Mbit;

    /* pre-configure radio addresses */
    NRF_RADIO->PREFIX0 = NRF51_CONF_PREFIX0;
    NRF_RADIO->PREFIX1 = NRF51_CONF_PREFIX1;
    NRF_RADIO->BASE0   = NRF51_CONF_BASE0 | NRF51ROP_DEFAULT_ADDR;
    NRF_RADIO->BASE1   = NRF51_CONF_BASE1 | NRF51ROP_BROADCASTCAST_ADDR;
    NRF_RADIO->TXADDRESS = 0x00UL;      /* always send from address 0 */
    NRF_RADIO->RXADDRESSES = 0x03UL;    /* listen to logical addresses 0 (own) and 1 (broadcast) */

    /* configure data fields and packet length whitening and endianess */
    NRF_RADIO->PCNF0 = (NRF51_CONF_S1 << RADIO_PCNF0_S1LEN_Pos) |
                       (NRF51_CONF_S0 << RADIO_PCNF0_S0LEN_Pos) |
                       (NRF51_CONF_LEN << RADIO_PCNF0_LFLEN_Pos);
    NRF_RADIO->PCNF1 = (NRF51_CONF_WHITENING << RADIO_PCNF1_WHITEEN_Pos) |
                       (NRF51_CONF_ENDIAN << RADIO_PCNF1_ENDIAN_Pos) |
                       (NRF51_CONF_BASE_ADDR_LEN << RADIO_PCNF1_BALEN_Pos) |
                       (NRF51_CONF_STATLEN << RADIO_PCNF1_STATLEN_Pos) |
                       (NRF51PROP_MAX_PAYLOAD_LENGTH << RADIO_PCNF1_MAXLEN_Pos);

    /* set shortcuts for more efficient transfer */
    NRF_RADIO->SHORTS = (1 << RADIO_SHORTS_READY_START_Pos);

    /* configure CRC unit */
    NRF_RADIO->CRCCNF = NRF51_CONF_CRC_LEN;
    NRF_RADIO->CRCPOLY = NRF51_CONF_CRC_POLY;
    NRF_RADIO->CRCINIT = 0xf0f0f0;

    /* enable END interrupt */
    NRF_RADIO->EVENTS_END = 0;
    NRF_RADIO->INTENSET = (1 << RADIO_INTENSET_END_Pos);

    /* enable interrupts */
    NVIC_SetPriority(RADIO_IRQn, RADIO_IRQ_PRIO);
    NVIC_EnableIRQ(RADIO_IRQn);
    return 0;
}

int _set_address(nrf51prop_t *dev, uint16_t address)
{
    DEBUG("nrf51prop: attempting to set address to %i\n", (int)address);
    /* make sure any ongoing transmit is finished */
    while (dev->state == STATE_TX);
    /* save old state */
    uint16_t state = dev->state;
    if (state == STATE_RX) {
        _switch_to_idle(dev);
    }
    /* set address */
    NRF_RADIO->BASE0 &= ~(0xffff);
    NRF_RADIO->BASE0 |= address;
    dev->own_addr = address;
    /* restore old state */
    if (state == STATE_RX) {
        _switch_to_rx(dev);
    }
    DEBUG("nrf51prop: set address to %i\n", (int)dev->own_addr);
    return address;
}

uint16_t _get_address(nrf51prop_t *dev)
{
    return dev->own_addr;
}

int nrf51prop_set_channel(nrf51prop_t *dev, uint8_t channel)
{
    uint8_t chan = channel & 0x3f;
    if (chan == channel) {
        /* make sure any ongoing transmit is finished */
        while (dev->state == STATE_TX);
        uint16_t state = dev->state;
        if (state == STATE_RX) {
            _switch_to_idle(dev);
        }
        NRF_RADIO->FREQUENCY = chan;
        if (state == STATE_RX) {
            _switch_to_rx(dev);
        }
        return (int)chan;
    }
    return -1;
}

int _get_channel(void)
{
    return (uint8_t)(0x3f & NRF_RADIO->FREQUENCY);
}

int _set_txpower(nrf51prop_txpower_t power)
{
    NRF_RADIO->TXPOWER = (power & 0xff);
    return 0;
}

nrf51prop_txpower_t _get_txpower(void)
{
    return (NRF_RADIO->TXPOWER & 0xff);
}

static void _receive_data(nrf51prop_t *dev)
{
    DEBUG("nrf51prop: reading data from memory and posting it to the MAC\n");

    if (dev->event_cb) {
        nrf51prop_packet_t *data = &(dev->rx_buf[(dev->rx_buf_next +1) & 0x01]);
        pktsnip_t *llhead, *payload;
        ll_gen_frame_t *frame;

        /* allocate memory */
        llhead = pktbuf_alloc(sizeof(ll_gen_frame_t) + 4);
        if (llhead == NULL) {
            DEBUG("Unable to allocate memory for the incoming link layer header\n");
            return;
        }
        payload = pktbuf_alloc(data->length - 2);
        if (payload == NULL) {
            pktbuf_release(llhead);
            DEBUG("Unable to allocate memory for the incoming payload\n");
            return;
        }

        /* fill generic link layer header */
        frame = (ll_gen_frame_t *)llhead->data;
        memset(frame, 0, sizeof(ll_gen_frame_t));
        frame->addr_len = 2;
        if (NRF_RADIO->RXMATCH == 0) {
            /* packet was send unicast */
            ll_gen_set_dst_addr(frame, (uint8_t *)&(dev->own_addr), 2);
        }
        else {
            /* packet was send broadcast */
            uint16_t bcast_addr = NRF51ROP_BROADCASTCAST_ADDR;
            ll_gen_set_dst_addr(frame, (uint8_t *)&(bcast_addr), 2);
        }
        ll_gen_set_src_addr(frame, (uint8_t *)&(data->src_addr), 2);

        /* copy and linke payload */
        memcpy(payload->data, data->payload, data->length - 2);
        llhead->next = payload;
        /* hand data over to MAC layer */
        dev->event_cb(NETDEV_EVENT_RX_COMPLETE, llhead);
    }
}

static void _switch_to_idle(nrf51prop_t *dev)
{
    NRF_RADIO->EVENTS_DISABLED = 0;
    NRF_RADIO->TASKS_DISABLE = 1;
    while (NRF_RADIO->EVENTS_DISABLED == 0);
    NRF_RADIO->EVENTS_DISABLED = 0;
    dev->state = STATE_IDLE;
}

static void _switch_to_rx(nrf51prop_t *dev)
{
    DEBUG("nrf51prop: switching to RX mode\n");
    /* set pointer to receive buffer */
    NRF_RADIO->PACKETPTR = (uint32_t)&(dev->rx_buf[dev->rx_buf_next]);
    /* set address */
    NRF_RADIO->BASE0 &= ~(0xffff);
    NRF_RADIO->BASE0 |= dev->own_addr;
    /* switch int RX mode */
    NRF_RADIO->TASKS_RXEN = 1;
    dev->state = STATE_RX;
}

static void _power_on(nrf51prop_t *dev)
{
    NRF_RADIO->POWER = 1;
    dev->state = STATE_IDLE;
}

static void _power_off(nrf51prop_t *dev)
{
    /* disable device before powering off */
    _switch_to_idle(dev);
    NRF_RADIO->POWER = 0;
    dev->state = STATE_OFF;
}

/*
 * Radio interrupt routine
 */
void isr_radio(void)
{
    msg_t msg;
    msg.type = NETDEV_MSG_EVENT_TYPE;

    DEBUG("nrf51prop ISR\n");

    if (NRF_RADIO->EVENTS_END == 1) {
        NRF_RADIO->EVENTS_END = 0;
        /* did we just send or receive something? */
        if (_radio->state == STATE_RX) {
            /* check if CRC was OK */
            if (NRF_RADIO->CRCSTATUS == 1) { /* ok */
                DEBUG("nrf51prop ISR: RX and CRC ok\n");
                msg.content.value = ISR_EVENT_RX_DONE;
                msg_send_int(&msg, _radio->mac_pid);
                /* switch buffer */
                _radio->rx_buf_next = (_radio->rx_buf_next + 1) & 0x01;
                NRF_RADIO->PACKETPTR = (uint32_t)&_radio->rx_buf[_radio->rx_buf_next];
                /* go back into receive mode */
                NRF_RADIO->TASKS_START = 1;
                return;
            }
            else { /* not ok */
                if (_radio->options & ISR_EVENT_WRONG_CHKSUM) {
                    msg.content.value = ISR_EVENT_WRONG_CHKSUM;
                    msg_send_int(&msg, _radio->mac_pid);
                }
            }
        }
        else if (_radio->state == STATE_TX) {
            /* disable radio again */
            _switch_to_idle(_radio);
            DEBUG("nrf51prop ISR: tx done\n");
            /* if radio was receiving before, go back into RX state */
            if (_radio->old_state == STATE_RX) {
                DEBUG("nrf51prop ISR: going back to RX\n");
                _switch_to_rx(_radio);
            }
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}

/*
 * Define the netdev interface
 */

int _send(netdev_t *dev, pktsnip_t *pkt)
{
    nrf51prop_t *radio = (nrf51prop_t *)dev;
    int count = 0;
    pktsnip_t *current = pkt->next;
    uint16_t dst_addr;

    DEBUG("nrf51prop_send: sending data now!\n");

#if DEVELHELP
    if (pkt->type != PKT_PROTO_LL_GEN) {
        DEBUG("nrf51prop_send: given header is not of type LL_GEN\n");
        return 0;
    }
#endif

    /* prepare TX buffer */
    ll_gen_frame_t *llheader = (ll_gen_frame_t *)pkt->data;
    ll_gen_get_dst_addr(llheader, (uint8_t *)&dst_addr, 2);

    /* make sure there is no transfer in progress, otherwise wait for it to finish */
    while (radio->state == STATE_TX);

    radio->tx_buf.length = pkt_len(pkt->next) + 2; /* 2 byte src address */
    radio->tx_buf.src_addr = radio->own_addr;
    while (current != NULL) {
        memcpy((&radio->tx_buf.payload + count), current->data, current->size);
        count += current->size;
        current = current->next;
    }
    //pktbuf_release(pkt);

    if (count > NRF51PROP_MAX_PAYLOAD_LENGTH) {
        DEBUG("nrf51 TX: payload too large, dropping package\n");
        return 0;
    }

    DEBUG("nrf51 TX: sending %i byte to addr %i\n", radio->tx_buf.length, dst_addr);

    /* switch back to disabled in case we are in RX state */
    if (radio->state == STATE_RX) {
        _switch_to_idle(radio);
        radio->old_state = STATE_RX;
    }
    else {
        radio->old_state = STATE_IDLE;
    }
    /* point radio to the packet that it has to send */
    NRF_RADIO->PACKETPTR = (uint32_t)(&radio->tx_buf);
    /* set destination address */
    NRF_RADIO->BASE0 &= ~(0xffff);
    NRF_RADIO->BASE0 |= dst_addr;
    /* send out packet */
    DEBUG("nrf51 STATE: TX\n");
    radio->state = STATE_TX;
    NRF_RADIO->TASKS_TXEN = 1;
    return (int)count;
}

int _add_event_cb(netdev_t *dev, netdev_event_cb_t cb)
{
    DEBUG("nrf51prop: adding event callback %p\n", cb);
    nrf51prop_t *radio = (nrf51prop_t *)dev;

    /* test if no other callback is already registered */
    if (radio->event_cb) {
        return -1;
    }
    radio->event_cb = cb;
    DEBUG("nrf51prop: cb set ok\n");
    return 0;
}

int _rem_event_cb(netdev_t *dev, netdev_event_cb_t cb)
{
    (void)cb;
    nrf51prop_t *radio = (nrf51prop_t *)dev;

    radio->event_cb = NULL;
    return 0;
}

int _get_option(netdev_t *dev, netconf_opt_t opt, void *value, size_t *value_len)
{
    return -1;
}

int _set_option(netdev_t *dev, netconf_opt_t opt, void *value, size_t value_len)
{
    nrf51prop_t *radio = (nrf51prop_t *)dev;

    printf("nrf51prop: set option: %i\n", (int)opt);

    switch (opt) {
        case NETCONF_OPT_ADDRESS:
            return _set_address(radio, *((uint16_t *)value));
        case NETCONF_OPT_STATE:
            return _trigger(dev, *((int *)value));
        default:
            return -100;
    }
}

int _get_state(netdev_t *dev, netdev_state_t *state)
{
    return -1;
}

int _trigger(netdev_t *dev, netdev_action_t action)
{
    nrf51prop_t *radio = (nrf51prop_t *)dev;

#if DEVELHELP
    if (!radio) {
        return -ENODEV;
    }
#endif

    switch (action) {
        case NETDEV_ACTION_POWEROFF:
            _power_off(radio);
            break;
        case NETDEV_ACTION_POWERON:
            _power_on(radio);
            break;
        case NETDEV_ACTION_IDLE:
            _switch_to_idle(radio);
            break;
        case NETDEV_ACTION_RX:
            _switch_to_rx(radio);
            break;
        default:
            return -ENOTSUP;
    }
    return 0;
}

void _isr_event(netdev_t *dev, uint16_t event_type)
{
    DEBUG("nrf51prop: after isr event\n");
    switch (event_type) {
        case ISR_EVENT_RX_DONE:
            _receive_data((nrf51prop_t *)dev);
            break;
        case ISR_EVENT_RX_START:
            /* do nothing for now */
            break;
        default:
            /* do nothing */
            return;
    }
}
