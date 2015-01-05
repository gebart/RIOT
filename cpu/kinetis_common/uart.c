/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis_common
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include <math.h>

#include "cpu.h"
#include "thread.h"
#include "sched.h"
#include "periph_conf.h"
#include "periph/uart.h"


/**
 * @brief Each UART device has to store two callbacks.
 */
typedef struct {
    uart_rx_cb_t rx_cb;
    uart_tx_cb_t tx_cb;
    void *arg;
} uart_conf_t;

/**
 * @brief Unified interrupt handler for all UART devices
 *
 * @param uartnum       the number of the UART that triggered the ISR
 * @param uart          the UART device that triggered the ISR
 */
static inline void irq_handler(uart_t uartnum, UART_Type *uart);

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_conf_t config[UART_NUMOF];

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{
    /* do basic initialization */
    int res = uart_init_blocking(uart, baudrate);

    if (res < 0) {
        return res;
    }

    /* remember callback addresses */
    config[uart].rx_cb = rx_cb;
    config[uart].tx_cb = tx_cb;
    config[uart].arg = arg;

    /* enable receive interrupt */
    switch (uart) {
#if UART_0_EN

        case UART_0:
            NVIC_SetPriority(UART_0_IRQ_CHAN, UART_IRQ_PRIO);
            NVIC_EnableIRQ(UART_0_IRQ_CHAN);
            UART_0_DEV->C2 |= (1 << UART_C2_RIE_SHIFT);
            break;
#endif
#if UART_1_EN

        case UART_1:
            NVIC_SetPriority(UART_1_IRQ_CHAN, UART_IRQ_PRIO);
            NVIC_EnableIRQ(UART_1_IRQ_CHAN);
            UART_1_DEV->C2 |= (1 << UART_C2_RIE_SHIFT);
            break;
#endif

        default:
            return -2;
            break;
    }

    return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
    UART_Type *dev;
    PORT_Type *port;
    uint32_t clk;
    uint16_t ubd;
    uint16_t brfa;
    uint8_t tx_pin = 0;
    uint8_t rx_pin = 0;
    uint8_t af;

    switch (uart) {
#if UART_0_EN

        case UART_0:
            dev = UART_0_DEV;
            port = UART_0_PORT;
            clk = UART_0_CLK;
            tx_pin = UART_0_TX_PIN;
            rx_pin = UART_0_RX_PIN;
            af = UART_0_AF;
            UART_0_PORT_CLKEN();
            UART_0_CLKEN();
            break;
#endif
#if UART_1_EN

        case UART_1:
            dev = UART_1_DEV;
            port = UART_1_PORT;
            clk = UART_1_CLK;
            tx_pin = UART_1_TX_PIN;
            rx_pin = UART_1_RX_PIN;
            af = UART_1_AF;
            UART_1_PORT_CLKEN();
            UART_1_CLKEN();
            break;
#endif

        default:
            return -1;
    }

    /* configure RX and TX pins, set pin to use alternative function mode */
    port->PCR[rx_pin] = PORT_PCR_MUX(af);
    port->PCR[tx_pin] = PORT_PCR_MUX(af);

    /* disable transmitter and receiver */
    dev->C2 &= ~(1 << UART_C2_TE_SHIFT | 1 << UART_C2_RE_SHIFT);
    /* set defaults, 8-bit mode, no parity */
    dev->C1 = 0;

    /* calculate baudrate */
    ubd = (uint16_t)(clk / (baudrate * 16));
    /* baudrate fine adjust - brfa */
    brfa = (uint16_t)(((clk * 32) / (baudrate * 16)) - (ubd * 32));

    /* set baudrate */
    dev->BDH = (uint8_t)UART_BDH_SBR(ubd >> 8);
    dev->BDL = (uint8_t)UART_BDL_SBR(ubd);

    /* set brfa */
    /* TODO: handle different uart types */
    (void)brfa;
    //dev->C4 = (uint8_t)UART_C4_BRFA(brfa);

    /* enable transmitter and receiver */
    dev->C2 |= (1 << UART_C2_TE_SHIFT | 1 << UART_C2_RE_SHIFT);
    return 0;
}

void uart_tx_begin(uart_t uart)
{
    switch (uart) {
#if UART_0_EN

        case UART_0:
            UART_0_DEV->C2 |= (1 << UART_C2_TIE_SHIFT);
            break;
#endif
#if UART_1_EN

        case UART_1:
            UART_1_DEV->C2 |= (1 << UART_C2_TIE_SHIFT);
            break;
#endif

        default:
            break;
    }
}

void uart_tx_end(uart_t uart)
{
    switch (uart) {
#if UART_0_EN

        case UART_0:
            UART_0_DEV->C2 &= ~(1 << UART_C2_TIE_SHIFT);
            break;
#endif
#if UART_1_EN

        case UART_1:
            UART_1_DEV->C2 &= ~(1 << UART_C2_TIE_SHIFT);
            break;
#endif

        default:
            break;
    }
}

int uart_write(uart_t uart, char data)
{
    switch (uart) {
#if UART_0_EN

        case UART_0:
            if (UART_0_DEV->S1 & UART_S1_TDRE_MASK) {
                UART_0_DEV->D = (uint8_t)data;
            }

            break;
#endif
#if UART_1_EN

        case UART_1:
            if (UART_1_DEV->S1 & UART_S1_TDRE_MASK) {
                UART_1_DEV->D = (uint8_t)data;
            }

            break;
#endif

        default:
            return -2;
            break;
    }

    return 0;
}

int uart_read_blocking(uart_t uart, char *data)
{
    switch (uart) {
#if UART_0_EN

        case UART_0:
            while (!(UART_0_DEV->S1 & UART_S1_RDRF_MASK));

            *data = (char)UART_0_DEV->D;
            break;
#endif
#if UART_1_EN

        case UART_1:
            while (!(UART_1_DEV->S1 & UART_S1_RDRF_MASK));

            *data = (char)UART_1_DEV->D;
            break;
#endif

        default:
            return -2;
            break;
    }

    return 1;
}

int uart_write_blocking(uart_t uart, char data)
{
    switch (uart) {
#if UART_0_EN

        case UART_0:
            while (!(UART_0_DEV->S1 & UART_S1_TDRE_MASK));

            UART_0_DEV->D = (uint8_t)data;
            break;
#endif
#if UART_1_EN

        case UART_1:
            while (!(UART_1_DEV->S1 & UART_S1_TDRE_MASK));

            UART_1_DEV->D = (uint8_t)data;
            break;
#endif

        default:
            return -2;
            break;
    }

    return 1;
}

#if UART_0_EN
void UART_0_ISR(void)
{
    irq_handler(UART_0, UART_0_DEV);
}
#endif

#if UART_1_EN
void UART_1_ISR(void)
{
    irq_handler(UART_1, UART_1_DEV);
}
#endif

static inline void irq_handler(uint8_t uartnum, UART_Type *dev)
{
    if (dev->S1 & UART_S1_RDRF_MASK) {
        char data = (char)dev->D;
        config[uartnum].rx_cb(config[uartnum].arg, data);
    }
    else if (dev->S1 & UART_S1_TDRE_MASK) {
        if (config[uartnum].tx_cb(config[uartnum].arg) == 0) {
            dev->C2 |= (1 << UART_C2_TIE_SHIFT);
        }
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
