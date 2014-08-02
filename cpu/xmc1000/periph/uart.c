/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_xmc1000
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <math.h>

#include "cpu.h"
#include "board.h"
#include "periph_conf.h"
#include "periph/uart.h"


/**
 * @brief Each UART device has to store two callbacks.
 */
typedef struct {
    void (*rx_cb)(char);
    void (*tx_cb)(void);
} uart_conf_t;


/**
 * @brief Unified interrupt handler for all UART devices
 *
 * @param uartnum       the number of the UART that triggered the ISR
 * @param uart          the UART device that triggered the ISR
 */
static inline void irq_handler(uart_t uartnum, USART_TypeDef *uart);


/**
 * @brief Allocate memory to store the callback functions.
 */
/* static uart_conf_t config[UART_NUMOF]; */


int uart_init(uart_t uart, uint32_t baudrate, void (*rx_cb)(char), void (*tx_cb)(void))
{
    return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
    switch (uart) {
        case UART_0:
            /* disable clock gating (enable clock) for USIC0 module */
            SCU_GENERAL->PASSWD = 0x000000C0;
            SCU_CLK->CGATCLR0 = SCU_CLK_CGATCLR_USIC0_Msk;
            SCU_GENERAL->PASSWD = 0x000000c3;

            /* enable USIC0 kernel clock */
            USIC0_CH0->KSCFG |= USIC_CH_KSCFG_MODEN_Ms | USIC_CH_KSCFG_BPMODEN_Msk;

            /* configure baud rate generator */
            /* BAUDRATE = fCTQIN/(BRG.PCTQ x BRG.DCTQ) */
            /* CLKSEL = 0 (fPIN = fFD), CTQSEL = 00b (fCTQIN = fPDIV), PPPEN = 0 (fPPP=fPIN) */
            USIC0_CH0->BRG &= ~(USIC_CH_BRG_PCTQ_Msk | USIC_CH_BRG_DCTQ_Msk |
                                USIC_CH_BRG_PDIV_Msk | USIC_CH_BRG_CLKSEL_Msk |
                                USIC_CH_BRG_PPPEN_Msk);
            USIC0_CH0->BRG |= (BRG_PCTQ << USIC_CH_BRG_PCTQ_Pos) |
                              (BRG_DCTQ << USIC_CH_BRG_DCTQ_Pos) |
                              (BRG_PDIV << USIC_CH_BRG_PDIV_Pos);

            /* configuration of USIC Shift Control */
            /* SCTR.FLE = 8 (Frame Length) */
            /* SCTR.WLE = 8 (Word Length) */
            /* SCTR.TRM = 1 (Transmission Mode) */
            /* SCTR.PDL = 1 (this bit defines the output level at the */
            /* shift data output signal when no data is available for transmission) */
            USIC0_CH0->SCTR &= ~(USIC_CH_SCTR_TRM_Msk | USIC_CH_SCTR_FLE_Msk |
                                 USIC_CH_SCTR_WLE_Msk);
            USIC0_CH0->SCTR |= (USIC_CH_SCTR_PDL_Msk) |
                               (0x01UL << USIC_CH_SCTR_TRM_Pos) |
                               (0x07UL << USIC_CH_SCTR_FLE_Pos) |
                               (0x07UL << USIC_CH_SCTR_WLE_Pos);

            /* Configuration of USIC Transmit Control/Status Register */
            /* TBUF.TDEN = 1 (TBUF Data Enable: A transmission of the data word in */
            /*                                  TBUF can be started if TDV = 1 */
            /* TBUF.TDSSM = 1 (Data Single Shot Mode: allow word-by-word data */
            /*                                        transmission which avoid sending */
            /*                                        the same data several times*/
            USIC0_CH0->TCSR &= ~(USIC_CH_TCSR_TDEN_Msk);
            USIC0_CH0->TCSR |= (USIC_CH_TCSR_TDSSM_Msk) |
                               (0x01UL << USIC_CH_TCSR_TDEN_Pos);

            /* Configuration of Protocol Control Register */
            /* PCR.SMD = 1 (Sample Mode based on majority) */
            /* PCR.STPB = 0 (1x Stop bit) */
            /* PCR.SP = 5 (Sample Point) */
            /* PCR.PL = 0 (Pulse Length is equal to the bit length) */
            USIC0_CH0->PCR &= ~(USIC_CH_PCR_ASCMode_STPB_Msk |
                                USIC_CH_PCR_ASCMode_SP_Msk |
                                USIC_CH_PCR_ASCMode_PL_Msk);
            USIC0_CH0->PCR |= (USIC_CH_PCR_ASCMode_SMD_Msk) |
                              (9 << USIC_CH_PCR_ASCMode_SP_Pos);

            /* Configure Transmit Buffer */
            /* Standard transmit buffer event is enabled */
            /* Define start entry of Transmit Data FIFO buffer DPTR = 0*/
            /* Set Transmit Data Buffer to 32 and set data pointer to position 0*/
            USIC0_CH0->TBCTR &= ~(USIC_CH_TBCTR_SIZE_Msk | USIC_CH_TBCTR_DPTR_Msk);
            USIC0_CH0->TBCTR |= (0x05 << USIC_CH_TBCTR_SIZE_Pos) |
                                (0x00 << USIC_CH_TBCTR_DPTR_Pos);

            /* configure pins */

            /* disable analog more for RX and TX pin */
            PORT2->PDISC &= ~(1 << UART_0_PIN_TX | 1 << UART_0_PIN_RX);

            /* set mode to input for RX pin */
            PORT2->IOCR0 &= ~(0x1f << PORT2_IOCR0_PC2_Pos);

            /* Select P2.2 as input for USIC DX3 -> UCIC DX0 */
            USIC0_CH0->DX3CR &= ~(USIC_CH_DX3CR_DSEL_Msk);

            /* Route USIC DX3 input signal to USIC DX0 (DSEL=DX0G) */
            USIC0_CH0->DX0CR &= ~(USIC_CH_DX0CR_DSEL_Msk);
            USIC0_CH0->DX0CR |= 6 << USIC_CH_DX0CR_DSEL_Pos;

            /* Configure Receive Buffer */
            /* Standard Receive buffer event is enabled */
            /* Define start entry of Receive Data FIFO buffer DPTR = 32*/
            /* Set Receive Data Buffer Size to 32 and set data pointer to position 32*/
            USIC0_CH0->RBCTR &= ~(USIC_CH_RBCTR_SIZE_Msk | USIC_CH_RBCTR_DPTR_Msk);
            USIC0_CH0->RBCTR |= (0x05UL << USIC_CH_RBCTR_SIZE_Pos) |
                                (32 << USIC_CH_RBCTR_DPTR_Pos);

            /* Initialize UART_TX (DOUT)*/
            /* P2.1 as output controlled by ALT6 = U0C0.DOUT0 */
            PORT2->IOCR0 &= ~(0x1f << PORT2_IOCR0_PC1_Pos);
            PORT2->IOCR0 |= (0x16 << PORT2_IOCR0_PC1_Pos);

            /* Configuration of Channel Control Register */
            /* CCR.PM = 00 ( Disable parity generation) */
            /* CCR.MODE = 2 (ASC mode enabled. Note: 0 (USIC channel is disabled)) */
            USIC0_CH0->CCR &= ~(USIC_CH_CCR_PM_Msk | USIC_CH_CCR_MODE_Msk);
            USIC0_CH0->CCR |= 0x02UL << USIC_CH_CCR_MODE_Pos;

            break;
    }

    return 0;
}

void uart_tx_begin(uart_t uart)
{
}

#include <stdio.h>
void uart_tx_end(uart_t uart)
{
}

int uart_write(uart_t uart, char data)
{
    return 0;
}

int uart_read_blocking(uart_t uart, char *data)
{
    return 1;
}

int uart_write_blocking(uart_t uart, char data)
{
    return 1;
}

void uart_poweron(uart_t uart)
{
}

void uart_poweroff(uart_t uart)
{
}
