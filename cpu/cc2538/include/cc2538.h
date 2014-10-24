/*
 * Copyright (C) 2014 Loci Controls Inc.
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_cc2538_definitions
 * @{
 *
 * @file            cc2538.h
 * @brief           CC2538 MCU interrupt and register definitions
 *
 * @author          Ian Martin <ian@locicontrols.com>
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef _CC2538_
#define _CC2538_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Interrupt Number Definitions
 */
typedef enum IRQn
{
    /******  Cortex-M3 Processor Exceptions Numbers ****************************/
    ResetHandler_IRQn     = -15, /**<  1 Reset Handler                         */
    NonMaskableInt_IRQn   = -14, /**<  2 Non Maskable Interrupt                */
    HardFault_IRQn        = -13, /**<  3 Cortex-M3 Hard Fault Interrupt        */
    MemoryManagement_IRQn = -12, /**<  4 Cortex-M3 Memory Management Interrupt */
    BusFault_IRQn         = -11, /**<  5 Cortex-M3 Bus Fault Interrupt         */
    UsageFault_IRQn       = -10, /**<  6 Cortex-M3 Usage Fault Interrupt       */
    SVCall_IRQn           = - 5, /**< 11 Cortex-M3 SV Call Interrupt           */
    DebugMonitor_IRQn     = - 4, /**< 12 Cortex-M3 Debug Monitor Interrupt     */
    PendSV_IRQn           = - 2, /**< 14 Cortex-M3 Pend SV Interrupt           */
    SysTick_IRQn          = - 1, /**< 15 Cortex-M3 System Tick Interrupt       */
    /******  CC2538 specific Interrupt Numbers *********************************/
    GPIO_PORT_A_IRQn      =   0, /**<    GPIO port A                           */
    GPIO_PORT_B_IRQn      =   1, /**<    GPIO port B                           */
    GPIO_PORT_C_IRQn      =   2, /**<    GPIO port C                           */
    GPIO_PORT_D_IRQn      =   3, /**<    GPIO port D                           */
    UART0_IRQn            =   5, /**<    UART0                                 */
    UART1_IRQn            =   6, /**<    UART1                                 */
    SSI0_IRQn             =   7, /**<    SSI0                                  */
    I2C_IRQn              =   8, /**<    I2C                                   */
    ADC_IRQn              =  14, /**<    ADC                                   */
    WDT_IRQn              =  18, /**<    Watchdog Timer                        */
    GPTIMER_0A_IRQn       =  19, /**<    GPTimer 0A                            */
    GPTIMER_0B_IRQn       =  20, /**<    GPTimer 0B                            */
    GPTIMER_1A_IRQn       =  21, /**<    GPTimer 1A                            */
    GPTIMER_1B_IRQn       =  22, /**<    GPTimer 1B                            */
    GPTIMER_2A_IRQn       =  23, /**<    GPTimer 2A                            */
    GPTIMER_2B_IRQn       =  24, /**<    GPTimer 2B                            */
    ADC_CMP_IRQn          =  25, /**<    Analog Comparator                     */
    RF_RXTX_ALT_IRQn      =  26, /**<    RF TX/RX (Alternate)                  */
    RF_ERR_ALT_IRQn       =  27, /**<    RF Error (Alternate)                  */
    SYS_CTRL_IRQn         =  28, /**<    System Control                        */
    FLASH_CTRL_IRQn       =  29, /**<    Flash memory control                  */
    AES_ALT_IRQn          =  30, /**<    AES (Alternate)                       */
    PKA_ALT_IRQn          =  31, /**<    PKA (Alternate)                       */
    SM_TIMER_ALT_IRQn     =  32, /**<    SM Timer (Alternate)                  */
    MAC_TIMER_ALT_IRQn    =  33, /**<    MAC Timer (Alternate)                 */
    SSI1_IRQn             =  34, /**<    SSI1                                  */
    GPTIMER_3A_IRQn       =  35, /**<    GPTimer 3A                            */
    GPTIMER_3B_IRQn       =  36, /**<    GPTimer 3B                            */
    UDMA_IRQn             =  46, /**<    uDMA software                         */
    UDMA_ERR_IRQn         =  47, /**<    uDMA error                            */
    USB_IRQn              = 140, /**<    USB                                   */
    RF_RXTX_IRQn          = 141, /**<    RF Core Rx/Tx                         */
    RF_ERR_IRQn           = 142, /**<    RF Core Error                         */
    AES_IRQn              = 143, /**<    AES                                   */
    PKA_IRQn              = 144, /**<    PKA                                   */
    SM_TIMER_IRQn         = 145, /**<    SM Timer                              */
    MACTIMER_IRQn         = 146, /**<    MAC Timer                             */

    PERIPH_COUNT_IRQn     = (MACTIMER_IRQn + 1) /**< Number of peripheral IDs */
} IRQn_Type;

/**
 * @brief Configuration of the Cortex-M3 Processor and Core Peripherals
 * @{
 */
#define __CM3_REV              0x0200 /**< CC2538 core revision number ([15:8] revision number, [7:0] patch number) */
#define __MPU_PRESENT          1      /**< CC2538 does provide a MPU */
#define __NVIC_PRIO_BITS       3      /**< CC2538 uses 3 Bits for the Priority Levels */
#define __Vendor_SysTickConfig 0      /**< Set to 1 if different SysTick Config is used */
/*@}*/

#define IEEE_ADDR_MSWORD            ( *(const uint32_t*)0x00280028 ) /**< Most-significant 32 bits of the IEEE address */
#define IEEE_ADDR_LSWORD            ( *(const uint32_t*)0x0028002c ) /**< Least-significant 32 bits of the IEEE address */

typedef volatile uint32_t cc2538_reg_t; /**< Least-significant 32 bits of the IEEE address */

/** @addtogroup Peripheral_memory_map
  * @{
  */
#define FLASH_BASE                  0x00200000 /**< FLASH base address */
#define SRAM_BASE                   0x20000000 /**< SRAM base address */
#define PERIPH_BASE                 0x40000000 /**< Peripheral base address */
#define SRAM_BB_BASE                0x22000000 /**< SRAM base address in the bit-band region */
/** @} */

#define XOSC32M_FREQ                32000000 /**< 32 MHz external oscillator/clock frequency */
#define RCOSC16M_FREQ               16000000 /**< 16 MHz internal RC oscillator frequency */

/* the CC2538 is build on a Cortex-M3, so include the M3 header */
#include <core_cm3.h>



/**
 * @brief RFCORE_XREG register map
 */
typedef struct {
    cc2538_reg_t FRMFILT0;
    cc2538_reg_t FRMFILT1;
    cc2538_reg_t SRCMATCH;
    cc2538_reg_t SRCSHORTEN0;
    cc2538_reg_t SRCSHORTEN1;
    cc2538_reg_t SRCSHORTEN2;
    cc2538_reg_t SRCEXTEN0;
    cc2538_reg_t SRCEXTEN1;
    cc2538_reg_t SRCEXTEN2;
    cc2538_reg_t FRMCTRL0;
    cc2538_reg_t FRMCTRL1;
    cc2538_reg_t RXENABLE;
    cc2538_reg_t RXMASKSET;
    cc2538_reg_t RXMASKCLR;
    cc2538_reg_t FREQTUNE;
    cc2538_reg_t FREQCTRL;
    cc2538_reg_t TXPOWER;
    cc2538_reg_t TXCTRL;
    cc2538_reg_t FSMSTAT0;
    cc2538_reg_t FSMSTAT1;
    cc2538_reg_t FIFOPCTRL;
    cc2538_reg_t FSMCTRL;
    cc2538_reg_t CCACTRL0;
    cc2538_reg_t CCACTRL1;
    cc2538_reg_t RSSI;
    cc2538_reg_t RSSISTAT;
    cc2538_reg_t RXFIRST;
    cc2538_reg_t RXFIFOCNT;
    cc2538_reg_t TXFIFOCNT;
    cc2538_reg_t RXFIRST_PTR;
    cc2538_reg_t RXLAST_PTR;
    cc2538_reg_t RXP1_PTR;
    cc2538_reg_t TXFIRST_PTR;
    cc2538_reg_t TXLAST_PTR;
    cc2538_reg_t RFIRQM0;
    cc2538_reg_t RFIRQM1;
    cc2538_reg_t RFERRM;
    cc2538_reg_t RFRND;
    cc2538_reg_t MDMCTRL0;
    cc2538_reg_t MDMCTRL1;
    cc2538_reg_t FREQEST;
    cc2538_reg_t RXCTRL;
    cc2538_reg_t FSCTRL;
    cc2538_reg_t FSCAL0;
    cc2538_reg_t FSCAL1;
    cc2538_reg_t FSCAL2;
    cc2538_reg_t FSCAL3;
    cc2538_reg_t AGCCTRL0;
    cc2538_reg_t AGCCTRL1;
    cc2538_reg_t AGCCTRL2;
    cc2538_reg_t AGCCTRL3;
    cc2538_reg_t ADCTEST0;
    cc2538_reg_t ADCTEST1;
    cc2538_reg_t ADCTEST2;
    cc2538_reg_t MDMTEST0;
    cc2538_reg_t MDMTEST1;
    cc2538_reg_t DACTEST0;
    cc2538_reg_t DACTEST1;
    cc2538_reg_t DACTEST2;
    cc2538_reg_t ATEST;
    cc2538_reg_t PTEST0;
    cc2538_reg_t PTEST1;
    cc2538_reg_t CSPPROG[24];
    cc2538_reg_t CSPCTRL;
    cc2538_reg_t CSPSTAT;
    cc2538_reg_t CSPX;
    cc2538_reg_t CSPY;
    cc2538_reg_t CSPZ;
    cc2538_reg_t CSPT;
    cc2538_reg_t RFC_OBS_CTRL0;
    cc2538_reg_t RFC_OBS_CTRL1;
    cc2538_reg_t RFC_OBS_CTRL2;
    cc2538_reg_t TXFILTCFG;
} cc2538_rfcore_xreg_t;

/**
 * @brief RFCORE_SFR register map;
 */
typedef struct {
    cc2538_reg_t RFDATA;
    cc2538_reg_t RFERRF;
    cc2538_reg_t RFIRQF1;
    cc2538_reg_t RFIRQF0;
    cc2538_reg_t RFST;
} cc2538_rfcore_sfr_t;

/**
 * @brief SYS_CTRL register map
 */
typedef struct {
    cc2538_reg_t CLOCK_CTRL;
    cc2538_reg_t CLOCK_STA;
    cc2538_reg_t RCGCGPT;
    cc2538_reg_t SCGCGPT;
    cc2538_reg_t DCGCGPT;
    cc2538_reg_t SRGPT;
    cc2538_reg_t RCGCSSI;
    cc2538_reg_t SCGCSSI;
    cc2538_reg_t DCGCSSI;
    cc2538_reg_t SRSSI;
    cc2538_reg_t RCGCUART;
    cc2538_reg_t SCGCUART;
    cc2538_reg_t DCGCUART;
    cc2538_reg_t SRUART;
    cc2538_reg_t RCGCI2C;
    cc2538_reg_t SCGCI2C;
    cc2538_reg_t DCGCI2C;
    cc2538_reg_t SRI2C;
    cc2538_reg_t RCGCSEC;
    cc2538_reg_t SCGCSEC;
    cc2538_reg_t DCGCSEC;
    cc2538_reg_t SRSEC;
    cc2538_reg_t PMCTL;
    cc2538_reg_t SRCRC;
    cc2538_reg_t undefined0[5];
    cc2538_reg_t PWRDBG;
    cc2538_reg_t undefined1[2];
    cc2538_reg_t CLD;
    cc2538_reg_t undefined2[4];
    cc2538_reg_t IWE;
    cc2538_reg_t I_MAP;
    cc2538_reg_t undefined3[3];
    cc2538_reg_t RCGCRFC;
    cc2538_reg_t SCGCRFC;
    cc2538_reg_t DCGCRF;
    cc2538_reg_t EMUOV;
} cc2538_sys_ctrl_t;

/**
 * @brief IOC port component registers
 */
typedef struct {
    cc2538_reg_t PA_SEL[8];
    cc2538_reg_t PB_SEL[8];
    cc2538_reg_t PC_SEL[8];
    cc2538_reg_t PD_SEL[8];
    cc2538_reg_t PA_OVER[8];
    cc2538_reg_t PB_OVER[8];
    cc2538_reg_t PC_OVER[8];
    cc2538_reg_t PD_OVER[8];
    cc2538_reg_t UARTRXD_UART0;
    cc2538_reg_t UARTCTS_UART1;
    cc2538_reg_t UARTRXD_UART1;
    cc2538_reg_t CLK_SSI_SSI0;
    cc2538_reg_t SSIRXD_SSI0;
    cc2538_reg_t SSIFSSIN_SSI0;
    cc2538_reg_t CLK_SSIIN_SSI0;
    cc2538_reg_t CLK_SSI_SSI1;
    cc2538_reg_t SSIRXD_SSI1;
    cc2538_reg_t SSIFSSIN_SSI1;
    cc2538_reg_t CLK_SSIIN_SSI1;
    cc2538_reg_t I2CMSSDA;
    cc2538_reg_t I2CMSSCL;
    cc2538_reg_t GPT0OCP1;
    cc2538_reg_t GPT0OCP2;
    cc2538_reg_t GPT1OCP1;
    cc2538_reg_t GPT1OCP2;
    cc2538_reg_t GPT2OCP1;
    cc2538_reg_t GPT2OCP2;
    cc2538_reg_t GPT3OCP1;
    cc2538_reg_t GPT3OCP2;
} cc2538_ioc_t;

/**
 * @brief SOC_ADC register map
 */
typedef struct {
    union {
        cc2538_reg_t ADCCON1;            /**< ADC Control Register 1 */
        struct {
            cc2538_reg_t RESERVED2 :  2; /**< Reserved bits */
            cc2538_reg_t RCTRL     :  2; /**< Random number generator control */
            cc2538_reg_t STSEL     :  2; /**< Start select */
            cc2538_reg_t ST        :  1; /**< Start conversion */
            cc2538_reg_t EOC       :  1; /**< End of conversion */
            cc2538_reg_t RESERVED1 : 24; /**< Reserved bits */
        } ADCCON1_BITS;
    };
    cc2538_reg_t reserved0[4];
    cc2538_reg_t RNDL;
    cc2538_reg_t RNDH;
} cc2538_soc_adc_t;


/**
 * @brief GPIO port component registers
 */
typedef struct {
    cc2538_reg_t RESERVED1[255];                    /**< Reserved addresses */
    cc2538_reg_t DATA;                              /**< GPIO_A Data Register */
    cc2538_reg_t DIR;                               /**< GPIO_A data direction register */
    cc2538_reg_t IS;                                /**< GPIO_A Interrupt Sense register */
    cc2538_reg_t IBE;                               /**< GPIO_A Interrupt Both-Edges register */
    cc2538_reg_t IEV;                               /**< GPIO_A Interrupt Event Register */
    cc2538_reg_t IE;                                /**< GPIO_A Interrupt mask register */
    cc2538_reg_t RIS;                               /**< GPIO_A Raw Interrupt Status register */
    cc2538_reg_t MIS;                               /**< GPIO_A Masked Interrupt Status register */
    cc2538_reg_t IC;                                /**< GPIO_A Interrupt Clear register */
    cc2538_reg_t AFSEL;                             /**< GPIO_A Alternate Function / mode control select register */
    cc2538_reg_t RESERVED2[63];                     /**< Reserved addresses */
    cc2538_reg_t GPIOLOCK;                          /**< GPIO_A Lock register */
    cc2538_reg_t GPIOCR;                            /**< GPIO_A Commit Register */
    cc2538_reg_t RESERVED3[118];                    /**< Reserved addresses */
    cc2538_reg_t PMUX;                              /**< GPIO_A The PMUX register */
    cc2538_reg_t P_EDGE_CTRL;                       /**< GPIO_A The Port Edge Control register */
    cc2538_reg_t RESERVED4[2];                      /**< Reserved addresses */
    cc2538_reg_t PI_IEN;                            /**< GPIO_A The Power-up Interrupt Enable register */
    cc2538_reg_t RESERVED5[1];                      /**< Reserved addresses */
    cc2538_reg_t IRQ_DETECT_ACK;                    /**< GPIO_A IRQ Detect ACK register */
    cc2538_reg_t USB_IRQ_ACK;                       /**< GPIO_A IRQ Detect ACK for USB */
    cc2538_reg_t IRQ_DETECT_UNMASK;                 /**< GPIO_A IRQ Detect ACK for masked interrupts */
    cc2538_reg_t RESERVED6[567];                    /**< Reserved addresses */
} cc2538_gpio_t;

/**
 * @name Values for IOC_PXX_SEL
 * @{
 */
#define IOC_SEL_UART0_TXD               (0)     /**< UART0 TXD */
#define IOC_SEL_UART1_RTS               (1)     /**< UART1 RTS */
#define IOC_SEL_UART1_TXD               (2)     /**< UART1 TXD */
#define IOC_SEL_SSI0_TXD                (3)     /**< SSI0 TXD */
#define IOC_SEL_SSI0_CLKOUT             (4)     /**< SSI0 CLKOUT */
#define IOC_SEL_SSI0_FSSOUT             (5)     /**< SSI0 FSSOUT */
#define IOC_SEL_SSI0_STXSER_EN          (6)     /**< SSI0 STXSER EN */
#define IOC_SEL_SSI1_TXD                (7)     /**< SSI1 TXD */
#define IOC_SEL_SSI1_CLKOUT             (8)     /**< SSI1 CLKOUT */
#define IOC_SEL_SSI1_FSSOUT             (9)     /**< SSI1 FSSOUT */
#define IOC_SEL_SSI1_STXSER_EN          (10)    /**< SSI1 STXSER EN */
#define IOC_SEL_I2C_CMSSDA              (11)    /**< I2C CMSSDA */
#define IOC_SEL_I2C_CMSSCL              (12)    /**< I2C CMSSCL */
#define IOC_SEL_GPT0_ICP1               (13)    /**< GPT0 ICP1 */
#define IOC_SEL_GPT0_ICP2               (14)    /**< GPT0 ICP2 */
#define IOC_SEL_GPT1_ICP1               (15)    /**< GPT1 ICP1 */
#define IOC_SEL_GPT1_ICP2               (16)    /**< GPT1 ICP2 */
#define IOC_SEL_GPT2_ICP1               (17)    /**< GPT2 ICP1 */
#define IOC_SEL_GPT2_ICP2               (18)    /**< GPT2 ICP2 */
#define IOC_SEL_GPT3_ICP1               (19)    /**< GPT3 ICP1 */
#define IOC_SEL_GPT3_ICP2               (20)    /**< GPT3 ICP2 */
/** @} */

/**
 * @name Values for IOC_PXX_OVER
 * @{
 */
#define IOC_OVER_OE                     (0x00000008)    /**< Output Enable */
#define IOC_OVER_PUE                    (0x00000004)    /**< Pull Up Enable */
#define IOC_OVER_PDE                    (0x00000002)    /**< Pull Down Enable */
#define IOC_OVER_ANA                    (0x00000001)    /**< Analog Enable */
#define IOC_OVER_DIS                    (0x00000000)    /**< Override Disabled */
/** @} */

/**
 * @name SOC_ADC_ADCCON1 bitfields
 * @{
 */
#define SOC_ADC_ADCCON1_RCTRL           (0x0c)
#define SOC_ADC_ADCCON1_RCTRL_POS       (2U)
#define SOC_ADC_ADCCON1_STSEL           (0x30)
#define SOC_ADC_ADCCON1_STSEL_POS       (4U)
#define SOC_ADC_ADCCON1_ST              (0x40)
#define SOC_ADC_ADCCON1_EOC             (0x80)
/** @} */

//             cc2538_reg_t RESERVED2 :  2; /**< Reserved bits */
//             cc2538_reg_t RCTRL     :  2; /**< Random number generator control */
//             cc2538_reg_t STSEL     :  2; /**< Start select */
//             cc2538_reg_t ST        :  1; /**< Start conversion */
//             cc2538_reg_t EOC       :  1; /**< End of conversion */
//             cc2538_reg_t RESERVED1 : 24; /**< Reserved bits */

/**
 * @brief Instantiation of peripherals
 * @{
 */
#define RFCORE_FFSM ((cc2538_rfcore_ffsm_t *)   0x40088580)     /**< RFCORE FFSM instance */
#define RFCORE_XREG ((cc2538_rfcore_xreg_t *)   0x40088600)     /**< RFCORE XREG instance */
#define RFCORE_SFR  ((cc2538_rfcore_sfr_t *)    0x40088828)     /**< RFCORE SFR instance */
#define SYS_CTRL    ((cc2538_sys_ctrl_t *)      0x400d2000)     /**< SYS_CTRL instance */
#define IOC         ((cc2538_ioc_t *)           0x400d4000)     /**< IOC instance */
#define SOC_ADC     ((cc2538_soc_adc_t *)       0x400d7000)     /**< SOC_ADC instance */
#define GPIO_A      ((cc2538_gpio_t *)          0x400d9000)     /**< GPIO Port A instance */
#define GPIO_B      ((cc2538_gpio_t *)          0x400da000)     /**< GPIO Port B instance */
#define GPIO_C      ((cc2538_gpio_t *)          0x400db000)     /**< GPIO Port C instance */
#define GPIO_D      ((cc2538_gpio_t *)          0x400dc000)     /**< GPIO Port D instance */
/** @} */


#ifdef __cplusplus
}
#endif

#endif /* _CC2538_ */
/*@}*/
