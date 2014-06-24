/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    driver_pdc8544 PDC8544 LCD driver
 * @ingroup     drivers
 * @brief       Driver for PDC8544 LCD displays
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the PDC8544 LCD driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __PDC8544_H
#define __PDC8544_H

#include <stdint.h>

#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
 extern "C" {
#endif


typedef struct {
    spi_t spi;          /**< SPI bus the display is connected to */
    gpio_t cs;          /**< chip-select pin, low active */
    gpio_t reset;       /**< reset pin, low active */
    gpio_t mode;        /**< data mode pin: cmd mode -> low, data mode -> high */
} pdc8544_t;

typedef enum {
    PDC8544_CONTRAST_LOW    = 0xb0,
    PDC8544_CONTRAST_MEDIUM = 0xb1,
    PDC8544_CONTRAST_HIGH   = 0xbf
} pdc8544_contrast_t;

int pdc8544_init(pdc8544_t *dev, spi_t spi, gpio_t cs,
                 gpio_t reset, gpio_t mode);

int pdc8544_print_riot_logo(pdc8544_t *dev);

int pdc8544_set_contrast(pdc8544_t *dev, pdc8544_contrast_t contrast);

/**
 * @brief
 *
 * @note expects given image to be 504 bytes
 *
 * @param img [description]
 * @return [description]
 */
int pdc8544_write_img(pdc8544_t *dev, const char img[]);

int pcd8544_write_c(pdc8544_t *dev, uint8_t x, uint8_t y, const char data);

int pdc8544_set_normal(pdc8544_t *dev);

int pdc8544_allon(pdc8544_t *dev);

int pdc8544_alloff(pdc8544_t *dev);

int pdc8544_invert(pdc8544_t *dev);
int pdc8544_poweron(pdc8544_t *dev);

int pdc8544_poweroff(pdc8544_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* __PDC8544_H */
/** @} */
