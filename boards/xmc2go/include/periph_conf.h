/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_xmc2go
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the STM32F0discovery board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

/**
 * @name Clock system configuration
 * @{
 */
#define CLOCK_CORE          (32000000U)         /* desired core clock frequency */

/* fixed and computed values */
#define CLOCK_DCO1CLK       (64000000U)
#define CLOCK_IDIV          (CLOCK_DCO1CLK / CLOCK_CORE / 2)
#define CLOCK_FDIV          (0)
#define CLOCK_PCLK_MUL2     (0)         /* set PCLK = MCLK */
/** @} */

/**
 * @name Timer configuration
 * @{
 */
#define TIMER_NUMOF         (1U)
#define TIMER_0_EN          1
#define TIMER_1_EN          0
#define TIMER_IRQ_PRIO      1

/* Timer 0 configuration */
#define TIMER_0_DEV         TIM2
#define TIMER_0_CHANNELS    4
#define TIMER_0_PRESCALER   (47U)
#define TIMER_0_MAX_VALUE   (0xffffffff)
#define TIMER_0_CLKEN()
#define TIMER_0_ISR
#define TIMER_0_IRQ_CHAN

/* Timer 1 configuration */
#define TIMER_1_DEV
#define TIMER_1_CHANNELS
#define TIMER_1_PRESCALER   (47U)
#define TIMER_1_MAX_VALUE   (0xffffffff)
#define TIMER_1_CLKEN()
#define TIMER_1_ISR
#define TIMER_1_IRQCHAN
#define TIMER_1_IRQ_PRIO
/** @} */

/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           1
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV
#define UART_0_CLKEN()
#define UART_0_IRQ
#define UART_0_ISR
/* UART 0 pin configuration */
#define UART_0_PORT
#define UART_0_PORT_CLKEN()
#define UART_0_RX_PIN
#define UART_0_TX_PIN
#define UART_0_AF

/* UART 1 device configuration */
#define UART_1_DEV
#define UART_1_CLKEN()
#define UART_1_IRQ
#define UART_1_ISR
/* UART 1 pin configuration */
#define UART_1_PORT
#define UART_1_PORT_CLKEN()
#define UART_1_RX_PIN
#define UART_1_TX_PIN
#define UART_1_AF
/** @} */

/**
 * @name GPIO configuration
 * @{
 */
#define GPIO_NUMOF          4
#define GPIO_0_EN           1
#define GPIO_1_EN           1
#define GPIO_2_EN           1
#define GPIO_3_EN           1
#define GPIO_IRQ_PRIO       1

/* GPIO channel 0 config */
#define GPIO_0_PORT         PORT1
#define GPIO_0_PIN          0
/* GPIO channel 1 config */
#define GPIO_1_PORT         GPIOA
#define GPIO_1_PIN          1
/* GPIO channel 2 config */
#define GPIO_2_PORT         GPIOF
#define GPIO_2_PIN          4
/* GPIO channel 3 config */
#define GPIO_3_PORT         GPIOF
#define GPIO_3_PIN          5
/** @} */

#endif /* __PERIPH_CONF_H */
