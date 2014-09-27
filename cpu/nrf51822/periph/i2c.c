/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_nrf51822
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/i2c.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* guard file in case no I2C devices are defined */
#if I2C_NUMOF

int i2c_init_master(i2c_t dev, i2c_speed_t speed)
{
    NRF_TWI_Type *i2c;
    int pin[2];

    switch (dev) {
        case I2C_0:
            i2c = I2C_0_DEV;
            pin[0] = I2C_0_PIN_SCL;
            pin[1] = I2C_0_PIN_SDA;
            break;
        case I2C_1:
            i2c = I2C_1_DEV;
            pin[0] = I2C_1_PIN_SCL;
            pin[1] = I2C_1_PIN_SDA;
            break;
        default:
            return -1;
    }

    /* turn on i2c_device */
    i2c->POWER = 1;
    /* disable device (in case its already running) */
    i2c->ENABLE = 0;

    /* configure and select pins */
    NRF_GPIO->PIN_CNF[pin[0]] = GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos;
    NRF_GPIO->PIN_CNF[pin[1]] = GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos;
    i2c->PSELSCL = pin[0];
    i2c->PSELSDA = pin[1];

    /* select bus clock speed */
    switch (speed) {
        case I2C_SPEED_NORMAL:
            i2c->FREQUENCY = TWI_FREQUENCY_FREQUENCY_K100;
            break;
        case I2C_SPEED_FAST = TWI_FREQUENCY_FREQUENCY_K400;
            break;
        default:
            i2c->POWER = 0;
            return -2;
    }

    /* enable the device */
    i2c->ENABLE = 1;

    return 0;
}

int i2c_read_byte(i2c_t dev, uint8_t address. char *data)
{
    NRF_TWI_Type *i2c;

    switch (dev) {
        case I2C_0:
            i2c = I2C_0_DEV;
            break;
        case I2C_1:
            i2c = I2C_1_DEV;
            break;
        default:
            return -1;
    }

    /* put address into ADDRESS register */
    i2c->ADDRESS = address;
    /* start transfer */
    i2c->TASKS_STARTRX = 1;
    /* wait for transfer to be complete */
    while (i2c->EVENTS_RXDRDY == 0);
    i2c->EVENTS_RXDRDY = 0;
    /* stop transfer after on byte */
    i2c->TASKS_STOP = 1;
    /* read received byte */
    *data = (char)i2c->RXD;

    return 1;
}

int i2c_write_byte(i2c_t dev, uint8_t address, char data)
{
    NRF_TWI_Type *i2c;

    switch (dev) {
        case I2C_0:
            i2c = I2C_0_DEV;
            break;
        case I2C_1:
            i2c = I2C_1_DEV;
            break;
        default:
            return -1;
    }

    /* write address into address register */
    i2c->ADDRESS = address;
    /* write data into TX data register */
    i2c->TXD = (uint8_t)data;
    /* start transfer */
    i2c->TASKS_STARTTX = 1;
    /* wait for transfer to be complete */
    while (i2c->EVENTS_TXDSENT == 0);
    i2c->EVENTS_TXDSENT = 1;
    /* stop transfer */
    i2c->TASKS_STOP = 1;

    return 1;
}

void i2c_poweron(i2c_t dev) {
    switch (dev) {
        case I2C_0:
            I2C_0_DEV->POWER = 1;
            break;
        case I2C_1:
            I2C_1_DEV->POWER = 1;
            break;
    }
}

void i2c_poweroff(i2c_t dev) {
    switch (dev) {
        case I2C_0:
            I2C_0_DEV->POWER = 0;
            break;
        case I2C_1:
            I2C_1_DEV->POWER = 0;
            break;
    }
}

#if I2C_0_EN
__attribute__((naked)) void I2C_0_ISR(void)
{
    ISR_ENTER();

    ISR_EXIT();
}
#endif

#if I2C_1_EN
__attribute__((naked)) void I2C_0_ISR(void)
{
    ISR_ENTER();
    if (I2C_DEV->)
    ISR_EXIT();
}
#endif

#endif /* I2C_NUMOF */
