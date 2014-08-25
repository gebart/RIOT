/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_k60
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se
 *
 * @}
 */

#include "cpu.h"
#include "thread.h"
#include "sched.h"
#include "periph_conf.h"
#include "periph/uart.h"

/* guard file in case no UART device was specified */
#if UART_NUMOF

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
static uart_conf_t uart_config[UART_NUMOF];

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{
    /* do basic initialization */
    int res = uart_init_blocking(uart, baudrate);
    if (res < 0) {
        return res;
    }

    /* remember callback addresses */
    uart_config[uart].rx_cb = rx_cb;
    uart_config[uart].tx_cb = tx_cb;
    uart_config[uart].arg = arg;

    /* enable receive interrupt */
    switch (uart) {
#if UART_0_EN
        case UART_0:
            NVIC_SetPriority(UART_0_IRQ_CHAN, UART_IRQ_PRIO);
            NVIC_EnableIRQ(UART_0_IRQ_CHAN);
            UART_0_DEV->C2 |= UART_C2_RIE_MASK;
            break;
#endif
#if UART_1_EN
        case UART_1:
            NVIC_SetPriority(UART_1_IRQ_CHAN, UART_IRQ_PRIO);
            NVIC_EnableIRQ(UART_1_IRQ_CHAN);
            UART_1_DEV->C2 |= UART_C2_RIE_MASK;
            break;
#endif
    }

    return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
    UART_Type *dev = 0;
    PORT_Type *port = 0;
    uint32_t tx_pin = 0;
    uint32_t rx_pin = 0;
    uint8_t tx_mux = 0;
    uint8_t rx_mux = 0;
    uint32_t clk = 0;
    uint8_t sbr = 0;
    uint8_t brfa = 0;

    switch (uart) {
#if UART_0_EN
        case UART_0:
            dev = UART_0_DEV;
            port = UART_0_PORT;
            clk = UART_0_CLK;
            tx_pin = UART_0_TX_PIN;
            rx_pin = UART_0_RX_PIN;
            tx_mux = UART_0_TX_PCR_MUX;
            rx_mux = UART_0_RX_PCR_MUX;
            UART_0_CLKEN();
            UART_0_PORT_CLKEN();
            break;
#endif
#if UART_1_EN
        case UART_1:
            dev = UART_1_DEV;
            port = UART_1_PORT;
            clk = UART_1_CLK;
            tx_pin = UART_1_TX_PIN;
            rx_pin = UART_1_RX_PIN;
            tx_mux = UART_1_TX_PCR_MUX;
            rx_mux = UART_1_RX_PCR_MUX;
            UART_1_CLKEN();
            UART_1_PORT_CLKEN();
            break;
#endif
    }

    /* Select the correct pin function in the mux */
    port->PCR[rx_pin] &= ~(PORT_PCR_MUX_MASK);
    port->PCR[rx_pin] |= PORT_PCR_MUX(rx_mux);
    port->PCR[tx_pin] &= ~(PORT_PCR_MUX_MASK);
    port->PCR[tx_pin] |= PORT_PCR_MUX(tx_mux);

    /* Make sure that the transmitter and receiver are disabled while we
     * change settings.
     */
    dev->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);
    /* Configure the UART for 8-bit mode, no parity */
    dev->C1 = 0; /* We need all default settings, so entire register is cleared */

    /* Module clock divisor */
    sbr = (clk / (baudrate * 16));
    /*
     * The below calculation will yield a fine adjust value rounded to the nearest
     * configurable fraction.
     */
    //~ brfa = (((64ull * clk) / (baudrate * 16) + 1) / 2) % 32;
    brfa = ((((4 * clk) / baudrate) + 1) / 2) % 32;

    /* Set baud rate registers */
    dev->BDH &= ~(UART_BDH_SBR(0xFF));
    dev->BDH |= UART_BDH_SBR(sbr >> 8);
    dev->BDL &= ~(UART_BDL_SBR(0xFF));
    dev->BDL |= UART_BDL_SBR(sbr);
    dev->C4 &= ~(UART_C4_BRFA(0xFF));
    dev->C4 |= UART_C4_BRFA(brfa);

    /* Enable transmitter and receiver */
    dev->C2 |= UART_C2_TE_MASK | UART_C2_RE_MASK;

    return 0;
}

void uart_tx_begin(uart_t uart)
{
    switch (uart) {
#if UART_0_EN
        case UART_0:
            UART_0_DEV->C2 |= UART_C2_TCIE_MASK;
            break;
#endif
#if UART_1_EN
        case UART_1:
            UART_1_DEV->C2 |= UART_C2_TCIE_MASK;
            break;
#endif
    }
}

int uart_write(uart_t uart, char data)
{
    UART_Type *dev = 0;

    switch (uart) {
#if UART_0_EN
        case UART_0:
            dev = UART_0_DEV;
            break;
#endif
#if UART_1_EN
        case UART_1:
            dev = UART_1_DEV;
            break;
#endif
    }

    /* Check if space is available in TX buffer */
    if (dev->S1 & UART_S1_TDRE_MASK) {
        dev->D = (uint8_t)data;
    }

    return 0;
}

int uart_read_blocking(uart_t uart, char *data)
{
    UART_Type *dev = 0;

    switch (uart) {
#if UART_0_EN
        case UART_0:
            dev = UART_0_DEV;
            break;
#endif
#if UART_1_EN
        case UART_1:
            dev = UART_1_DEV;
            break;
#endif
    }

    while (!(dev->S1 & UART_S1_RDRF_MASK));
    *data = (char)dev->D;

    return 1;
}

int uart_write_blocking(uart_t uart, char data)
{
    UART_Type *dev = 0;

    switch (uart) {
#if UART_0_EN
        case UART_0:
            dev = UART_0_DEV;
            break;
#endif
#if UART_1_EN
        case UART_1:
            dev = UART_1_DEV;
            break;
#endif
    }

    while (!(dev->S1 & UART_S1_TDRE_MASK));
    dev->D = (uint8_t)data;

    return 1;
}

void uart_poweron(uart_t uart)
{
    switch (uart) {
#if UART_0_EN
        case UART_0:
            UART_0_CLKEN();
            break;
#endif
#if UART_1_EN
        case UART_1:
            UART_1_CLKEN();
            break;
#endif
    }
}

void uart_poweroff(uart_t uart)
{
    switch (uart) {
#if UART_0_EN
        case UART_0:
            UART_0_CLKDIS();
            break;
#endif
#if UART_1_EN
        case UART_1:
            UART_1_CLKDIS();
            break;
#endif
    }
}

#if UART_0_EN
__attribute__((naked)) void UART_0_ISR(void)
{
    ISR_ENTER();
    irq_handler(UART_0, UART_0_DEV);
    ISR_EXIT();
}
#endif

#if UART_1_EN
__attribute__((naked)) void UART_1_ISR(void)
{
    ISR_ENTER();
    irq_handler(UART_1, UART_1_DEV);
    ISR_EXIT();
}
#endif

static inline void irq_handler(uint8_t uartnum, UART_Type *dev)
{
    if (dev->S1 & UART_S1_RDRF_MASK) {
        char data = (char)dev->D;
        uart_config[uartnum].rx_cb(uart_config[uartnum].arg, data);
    }
    else if (dev->S1 & UART_S1_TDRE_MASK) {
        if (uart_config[uartnum].tx_cb(uart_config[uartnum].arg) == 0) {
            dev->C2 &= ~(UART_C2_TIE_MASK);
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}

#endif /* UART_NUMOF */
