/*
 * Copyright (C) 2009 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * Parts taken from FeuerWhere-Project, lpc2387.h.
 */

#ifndef __LPC2387_H
#define __LPC2387_H

#include "lpc23xx.h"
#include "bitarithm.h"

/**
 * @name Interrupt vector definitions
 * @{
 */
#define WDT_IRQn         0
#define SWI_IRQn         1
#define ARM_CORE0_IRQn   2
#define ARM_CORE1_IRQn   3
#define TIMER0_IRQn      4
#define TIMER1_IRQn      5
#define UART0_IRQn       6
#define UART1_IRQn       7
#define PWM0_1_IRQn      8
#define I2C0_IRQn        9
#define SPI0_IRQn        10          /* SPI and SSP0 share VIC slot */
#define SSP0_IRQn        10
#define SSP1_IRQn        11
#define PLL_IRQn         12
#define RTC_IRQn         13
#define EINT0_IRQn       14
#define EINT1_IRQn       15
#define EINT2_IRQn       16
#define EINT3_IRQn       17
#define ADC0_IRQn        18
#define I2C1_IRQn        19
#define BOD_IRQn         20
#define EMAC_IRQn        21
#define USB_IRQn         22
#define CAN_IRQn         23
#define MCI_IRQn         24
#define GPDMA_IRQn       25
#define TIMER2_IRQn      26
#define TIMER3_IRQn      27
#define UART2_IRQn       28
#define UART3_IRQn       29
#define I2C2_IRQn        30
#define I2S_IRQn         31
/** @} */

/**
 * @name Timer Symbols
 * @{
 */
#define MR0I BIT0
#define MR0R BIT1
#define MR0S BIT2
#define MR1I BIT3
#define MR1R BIT4
#define MR1S BIT5
#define MR2I BIT6
#define MR2R BIT7
#define MR2S BIT8
#define MR3I BIT9
#define MR3R BIT10
#define MR3S BIT11
/** @} */

/**
 * @name RTC constants
 * @{
 */
#define IMSEC       0x00000001
#define IMMIN       0x00000002
#define IMHOUR      0x00000004
#define IMDOM       0x00000008
#define IMDOW       0x00000010
#define IMDOY       0x00000020
#define IMMON       0x00000040
#define IMYEAR      0x00000080

#define AMRSEC      0x00000001  /* Alarm mask for Seconds */
#define AMRMIN      0x00000002  /* Alarm mask for Minutes */
#define AMRHOUR     0x00000004  /* Alarm mask for Hours */
#define AMRDOM      0x00000008  /* Alarm mask for Day of Month */
#define AMRDOW      0x00000010  /* Alarm mask for Day of Week */
#define AMRDOY      0x00000020  /* Alarm mask for Day of Year */
#define AMRMON      0x00000040  /* Alarm mask for Month */
#define AMRYEAR     0x00000080  /* Alarm mask for Year */

#define ILR_RTCCIF  BIT0
#define ILR_RTCALF  BIT1
#define ILR_RTSSF   BIT2

#define CCR_CLKEN   0x01
#define CCR_CTCRST  0x02
#define CCR_CLKSRC  0x10
/** @} */

/**
 * @name WD constants
 * @{
 */
#define WDEN    BIT0
#define WDRESET BIT1
#define WDTOF   BIT2
#define WDINT   BIT3
/** @} */

/**
 * @name    PCONP Constants
 * @{
 */
#define PCTIM0      BIT1
#define PCTIM1      BIT2
#define PCUART0     BIT3
#define PCUART1     BIT4
#define PCPWM1      BIT6
#define PCI2C0      BIT7
#define PCSPI       BIT8
#define PCRTC       BIT9
#define PCSSP1      BIT10
#define PCEMC       BIT11
#define PCAD        BIT12
#define PCAN1       BIT13
#define PCAN2       BIT14
#define PCI2C1      BIT19
#define PCSSP0      BIT21
#define PCTIM2      BIT22
#define PCTIM3      BIT23
#define PCUART2     BIT24
#define PCUART3     BIT25
#define PCI2C2      BIT26
#define PCI2S       BIT27
#define PCSDC       BIT28
#define PCGPDMA     BIT29
#define PCENET      BIT30
#define PCUSB       BIT31
/** @} */

/**
 * @name    PCON Constants
 * @{
 */
#define PM0         BIT0
#define PM1         BIT1
#define BODPDM      BIT2
#define BOGD        BIT3
#define BORD        BIT4
#define PM2         BIT7

#define PM_IDLE         (PM0)
#define PM_SLEEP        (PM2|PM0)
#define PM_POWERDOWN    (PM1)
/** @} */

/**
 * @name    INTWAKE Constants
 * @{
 */
#define EXTWAKE0    BIT0
#define EXTWAKE1    BIT1
#define EXTWAKE2    BIT2
#define EXTWAKE3    BIT3
#define ETHWAKE     BIT4
#define USBWAKE     BIT5
#define CANWAKE     BIT6
#define GPIO0WAKE   BIT7
#define GPIO2WAKE   BIT8
#define BODWAKE     BIT14
#define RTCWAKE     BIT15
/** @} */

/**
 * @name UART Constants
 * @{
 */
#define ULSR_RDR        BIT0
#define ULSR_OE     BIT1
#define ULSR_PE     BIT2
#define ULSR_FE     BIT3
#define ULSR_BI     BIT4
#define ULSR_THRE   BIT5
#define ULSR_TEMT   BIT6
#define ULSR_RXFE   BIT7

#define UIIR_INT_STATUS     (BIT0)              ///< Interrupt Status
#define UIIR_THRE_INT       (BIT1)              ///< Transmit Holding Register Empty
#define UIIR_RDA_INT        (BIT2)              ///< Receive Data Available
#define UIIR_RLS_INT        (BIT1 | BIT2)       ///< Receive Line Status
#define UIIR_CTI_INT        (BIT2 | BIT3)       ///< Character Timeout Indicator
#define UIIR_ID_MASK        (BIT1 | BIT2 | BIT3)
#define UIIR_ABEO_INT       BIT8
#define UIIR_ABTO_INT       BIT9
/** @} */

/**
 * @name    SSP Status Register Constants
 * @{
 */
#define SSPSR_TFE           BIT0                ///< Transmit FIFO Empty. This bit is 1 is the Transmit FIFO is empty, 0 if not.
#define SSPSR_TNF           BIT1                ///< Transmit FIFO Not Full. This bit is 0 if the Tx FIFO is full, 1 if not.
#define SSPSR_RNE           BIT2                ///< Receive FIFO Not Empty. This bit is 0 if the Receive FIFO is empty, 1 if not.
#define SSPSR_RFF           BIT3                ///< Receive FIFO Full. This bit is 1 if the Receive FIFO is full, 0 if not.
#define SSPSR_BSY           BIT4                ///< Busy. This bit is 0 if the SSPn controller is idle, or 1 if it is currently sending/receiving a frame and/or the Tx FIFO is not empty.
/** @} */

/**
 * @name Timer register offsets
 * @{
 */
#define TXIR           0x00
#define TXTCR          0x04
#define TXTC           0x08                     ///< Timer counter
#define TXPR           0x0C
#define TXPC           0x10
#define TXMCR          0x14
#define TXMR0          0x18
#define TXMR1          0x1C
#define TXMR2          0x20
#define TXMR3          0x24
#define TXCCR          0x28
#define TXCR0          0x2C
#define TXCR1          0x30
#define TXCR2          0x34
#define TXCR3          0x38
#define TXEMR          0x3C
#define TXCTCR         0x70
/** @} */
/** @} */

#endif // __LPC2387_H
