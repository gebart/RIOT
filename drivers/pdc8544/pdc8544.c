/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_pdc8544
 * @{
 * @file
 * @brief       Implementation of the SPI driver for the PDC8544 graphics display
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <stdint.h>
#include <stdio.h>

#include "periph/spi.h"
#include "periph/gpio.h"
#include "pdc8544.h"
#include "pdc8544-internal.h"
#include "riot.h"


static void _write(pdc8544_t *dev, uint8_t is_data, char data);

int pdc8544_init(pdc8544_t *dev, spi_t spi, gpio_t cs, gpio_t reset, gpio_t mode)
{
    /* save pin mapping */
    dev->spi = spi;
    dev->cs = cs;
    dev->reset = reset;
    dev->mode = mode;

    /* initialze pins */
    gpio_init_out(cs, GPIO_NOPULL);
    gpio_init_out(reset, GPIO_NOPULL);
    gpio_init_out(mode, GPIO_NOPULL);
    /* clear CS line */
    gpio_set(cs);
    /* initialize SPI */
    spi_init_master(spi, SPI_CONF_FIRST_RISING, SPI_SPEED_1MHZ);
    /* reset display */
    gpio_clear(reset);
    gpio_set(reset);

    /* write initialization sequence to display */
    _write(dev, COM_CMD, CMD_EXTENDED);
    _write(dev, COM_CMD, PDC8544_CONTRAST_MEDIUM);
    _write(dev, COM_CMD, CMD_EXT_TEMP0);
    _write(dev, COM_CMD, CMD_EXT_BIAS3);
    /* enable display */
    _write(dev, COM_CMD, CMD_ENABLE_H);
    _write(dev, COM_CMD, CMD_MODE_NORMAL);

    return 0;
}

int pdc8544_print_riot_logo(pdc8544_t *dev)
{
    pdc8544_write_img(dev, riot_logo);
    return 0;
}

int pdc8544_set_contrast(pdc8544_t *dev, pdc8544_contrast_t contrast)
{
    _write(dev, COM_CMD, contrast);
    return 0;
}

int pdc8544_write_img(pdc8544_t *dev, const char img[])
{
    uint8_t x = 0;
    uint8_t y = 0;
    for (int i = 0; i < 504; i++) {
        pcd8544_write_c(dev, x++, y++, img[i]);
        x = (x == RES_X) ? 0 : x;
        y = (y == RES_Y) ? 0 : y;
    }
    return 0;
}


int pcd8544_write_c(pdc8544_t *dev, uint8_t x, uint8_t y, const char data)
{
    if (x >= RES_X || y >= RES_Y) {
        return -1;
    }

    /* set address */
    _write(dev, COM_CMD, CMD_SET_X | x);
    _write(dev, COM_CMD, CMD_SET_Y | y);
    /* write data */
    _write(dev, COM_DTA, data);
    return 0;
}

int pdc8544_set_normal(pdc8544_t *dev)
{
    _write(dev, COM_CMD, CMD_MODE_NORMAL);
    return 0;
}

int pdc8544_allon(pdc8544_t *dev)
{
    _write(dev, COM_CMD, CMD_MODE_ALLON);
    return 0;
}

int pdc8544_alloff(pdc8544_t *dev)
{
    _write(dev, COM_CMD, CMD_MODE_BLANK);
    return 0;
}

int pdc8544_invert(pdc8544_t *dev)
{
    _write(dev, COM_CMD, CMD_MODE_INVERSE);
    return 0;
}

int pdc8544_poweron(pdc8544_t *dev)
{
    _write(dev, COM_CMD, CMD_ENABLE_H);
    return 0;
}

int pdc8544_poweroff(pdc8544_t *dev)
{
    _write(dev, COM_CMD, CMD_DISABLE);
    return 0;
}

static void _write(pdc8544_t *dev, uint8_t is_data, char data)
{
    /* set command or data mode */
    gpio_write(dev->mode, is_data);

    /* write byte to LCD */
    gpio_clear(dev->cs);
    spi_transfer_byte(dev->spi, data, 0);
    gpio_set(dev->cs);
}

