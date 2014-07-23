/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_periph
 * @brief       Low-level I2C peripheral driver
 * @{
 *
 * @file
 * @brief       Low-level I2C peripheral driver interface definition
 *
 * The current I2C interface only supports 7-bit addresses.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __I2C_H
#define __I2C_H

/* guard this file in case no I2C device is defined */
#if I2C_NUMOF

/**
 * @brief define I2C device identifiers
 */
typedef enum {
#if I2C_0_EN
    I2C_0 = 0,          /**< I2C device 0 */
#endif
#if I2C_1_EN
    I2C_1,              /**< I2C device 1 */
#endif
#if I2C_2_EN
    I2C_2,              /**< I2C device 2 */
#endif
#if I2C_3_EN
    I2C_3               /**< I2C device 3 */
#endif
} i2c_t;

/**
 * @brief definition of available I2C bus clock speeds
 */
typedef enum {
    I2C_100KHZ = 0,     /**< standard I2C clock speed */
    I2C_400KHZ,         /**< I2C fast mode */
    I2C_3P4MHZ          /**< I2C high speed mode */
} i2c_speed_t;

typedef enum {
    I2C_START = 0,
    I2C_ADRESS,
    I2C_DATA,
    I2C_REPEATED_START
} i2c_state1_t;


int i2c_init_master(i2c_t dev, i2c_speed_t bus_speed);

int i2c_init_slave(i2c_t dev, uint8_t address);

int i2c_read_byte(i2c_t dev, uint8_t address. char *data);
int i2c_read_bytes(i2c_t dev, uint8_t address, char *data, int length);

int i2c_write_byte(i2c_t dev, uint8_t address, char data);
int i2c_write_bytes(i2c_t dev, uint8_t address, char *data, int length);

void i2c_poweron(i2c_t dev);
void i2c_poweroff(i2c_t dev);


#endif /* I2C_NUMOF */

#endif /* __I2C_H */
/** @} */
