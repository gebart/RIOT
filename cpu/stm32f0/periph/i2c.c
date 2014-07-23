/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f0
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "board.h"
#include "periph/i2c.h"
#include "periph_conf.h"

typedef struct {
    mutex_t lock;           /**< mutex for locking a device during a transaction */
    int state;
} _state_t;

static _state_t state[I2C_NUMOF];


int i2c_init_master(i2c_t dev, i2c_speed_t bus_speed)
{
    I2C_TypeDef *i2c;
    GPIO_TypeDef *port;
    int pins[2];
    int af;

    /* power on the I2C device */
    poweron(dev);

    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            i2c = I2C_0_DEV;
            port = I2C_0_PORT;
            pins[0] = I2C_0_PIN_SCL;
            pins[1] = I2C_0_PIN_SDA;
            af = I2C_0_PIN_AF;
            break;
#endif
#if I2C_1_EN
        case I2C_1:
            i2c = I2C_1_DEV;
            port = I2C_1_PORT;
            pins[0] = I2C_1_PIN_SCL;
            pins[1] = I2C_1_PIN_SDA;
            af = I2C_1_PIN_AF;
            break;
#endif
    }

    /* configure pins */
    for (int i = 0; i < 2; i++) {
        port->MODER &= ~(3 << (pins[i] * 2));
        port->MODER |= (2 << (pins[i] * 2));
        int hl = (pins[i] < 8) ? 0 : 1;
        port->AFR[hl] &= ~(0xf << ((pins[i] - (hl * 8)) * 4));
        port->AFR[hl] |= (af << ((pins[i] - (hl * 8)) * 4));
    }

    /* reset configuration registers */
    i2c->CR1 = 0;
    i2c->CR2 = 0;
    i2c->OAR1 = 0;
    i2c->OAR2 = 0;
    i2c->TIMEOUTR = 0;

    /* configure the clock */
    i2c->TIMINGR = 0;       /* prescaler is set to 0, so I2C is driven by F_CPU */
    switch (bus_speed) {
        case I2C_100KHZ:
            i2c->TIMINGR |= (238 << 8) | 238;
            break;
        case I2C_400KHZ:
            i2c->TIMINGR |= (60 << 8) | 60;
            break;
        case I2C_3P4MHZ:
            i2c->TIMINGR |= (14 << 8) | 14;
            break;
    }

    /* enable receive complete, transmit complete and transmission complete interrupts */
    /* i2c->CR1 |= I2C_CR1_TXIE | I2C_CR1_RXIE | I2C_CR1_TCIE;*/

    /* enable the i2c device */
    i2c->CR1 |= I2C_CR1_PECEN;
}

int i2c_init_slave(i2c_t dev, uint8_t address)
{
    I2C_TypeDef *i2c;

    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            i2c = I2C_0_DEV;
            break;
#endif
#if I2C_1_EN
        case I2C_1:
            i2c = I2C_1_DEV;
            break;
#endif
    }

    /* the the slave address */
    i2c->OAR1 = 0;
    i2c->OAR1 = ((address 0x7f) << 1);
    i2c->OAR1 |= I2C_OAR1_OA1EN;
}

int i2c_read_byte(i2c_t dev, uint8_t address. char *data)
{
    i2c_read_bytes(dev, address, data, 1);
}

int i2c_read_bytes(i2c_t dev, uint8_t address, char *data, int length)
{
    I2C_TypeDef *i2c;

    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            i2c = I2C_0_DEV;
            break;
#endif
#if I2C_1_EN
        case I2C_1:
            i2c = I2C_1_DEV;
            break;
#endif
    }
}

int i2c_write_byte(i2c_t dev, uint8_t address, char data, int end)
{
    i2c_write_bytes(dev, address, &data, 1, end);

}

int i2c_write_bytes(i2c_t dev, uint8_t address, char *data, int length, int end)
{
    I2C_TypeDef *i2c;
    int count = 0;

    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            i2c = I2C_0_DEV;
            break;
#endif
#if I2C_1_EN
        case I2C_1:
            i2c = I2C_1_DEV;
            break;
#endif
    }

    /* wait for device to be ready */
    while (i2c->ISR & I2C_ISR_BUSY);

    /* reset CR2 */
    i2c->CR2 = 0;

    /* set slave address and data length */
    i2c->CR2 |= ((address & 0x7f) << 1);

    if (lenth > 255) {
        i2c->CR2 |= I2C_CR2_RELOAD | (0xff << 16);
    }
    else {
        i2c->CR2 |= ((length & 0xff) << 16);
    }

    /* start transmission */
    i2c->CR2 | I2C_CR2_START;

    while (!(i2c->ISR & I2C_ISR_TXIS)) {
        if (i2c->ISR & I2C_ISR_NACKF) {
            return -1;
        }
    }

    while (count < length) {
        /* wait for the transmit register to be empty */
        while (!(i2c->ISR & I2C_ISR_TXE));
        /* write the next byte into the transmit register */
        i2c->TXDR = data[count++];

        /* see if a transfer was partially complete */
    }

    /* end the transaction if applicable */
    if (end) {
        i2c->CR2 |= I2C_CR2_STOP;
    }
}

int i2c_read_register(i2c_t dev, uint8_t address, uint8_t reg, char *data)
{
    i2c_read_registers(dev, address, reg, data, 1);
}

int i2c_read_registers(i2c_t dev, uint8_t address, uint8_t reg, char *data, int length)
{

}

void i2c_poweron(i2c_t dev)
{
    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            I2C_0_CLKEN();
            break;
#endif
#if I2C_1_EN
        case I2C_1:
            I2C_1_CLKEN();
            break;
#endif
    }
}

void i2c_poweroff(i2c_t dev)
{
    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            I2C_0_CLKDIS();
            break;
#endif
#if I2C_1_EN
        case I2C_1:
            I2C_1_CLKDIS();
            break;
#endif
    }
}
