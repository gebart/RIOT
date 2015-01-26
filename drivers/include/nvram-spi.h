/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     nvram
 * @{
 *
 * @file
 *
 * @brief       Device interface for various SPI connected NVRAM.
 *
 * Tested on:
 * - Cypress/Ramtron FM25L04B.
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 */

#ifndef DRIVERS_NVRAM_SPI_H_
#define DRIVERS_NVRAM_SPI_H_

#include <stdint.h>
#include "nvram.h"
#include "periph/spi.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Bus parameters for SPI NVRAM.
 */
typedef struct nvram_spi_params {
    /** @brief RIOT SPI device */
    spi_t spi;
    /** @brief Chip select pin */
    gpio_t cs;
    /** @brief Number of address bytes following each read/write command. */
    uint8_t address_count;
} nvram_spi_params_t;

/**
 * @brief Initialize an nvram_t structure with SPI settings.
 *
 * @param[out] dev          Pointer to NVRAM device descriptor
 * @param[out] spi_params   Pointer to SPI settings
 * @param[in]  size         Device capacity
 *
 * @return          0 on success
 * @return          <0 on errors
 */
int nvram_spi_init(nvram_t *dev, nvram_spi_params_t *spi_params, size_t size);

/**
 * @brief Copy data from system memory to NVRAM.
 *
 * @param[in]  dev   Pointer to NVRAM device descriptor
 * @param[in]  dst   Starting address in the NVRAM device address space
 * @param[in]  src   Pointer to the first byte in the system memory address space
 * @param[in]  len   Number of bytes to copy
 *
 * @return          Number of bytes written on success
 * @return          <0 on errors
 */
int nvram_spi_write(nvram_t *dev, uint32_t dst, uint8_t *src, size_t len);

/**
 * @brief Copy data from system memory to NVRAM.
 *
 * This is a special form of the WRITE command used by some Ramtron/Cypress
 * 4Kbit FRAM devices which puts the 9th address bit inside the command byte to
 * be able to use one byte for addressing instead of two.
 *
 * @param[in]  dev   Pointer to NVRAM device descriptor
 * @param[in]  dst   Starting address in the NVRAM device address space
 * @param[in]  src   Pointer to the first byte in the system memory address space
 * @param[in]  len   Number of bytes to copy
 *
 * @return          Number of bytes written on success
 * @return          <0 on errors
 */
int nvram_spi_write_9bit_addr(nvram_t *dev, uint32_t dst, uint8_t *src, size_t len);

/**
 * @brief Copy data from NVRAM to system memory.
 *
 * @param[in]  dev   Pointer to NVRAM device descriptor
 * @param[in]  dst   Pointer to the first byte in the system memory address space
 * @param[in]  src   Starting address in the NVRAM device address space
 * @param[in]  len   Number of bytes to copy
 *
 * @return          Number of bytes written on success
 * @return          <0 on errors
 */
int nvram_spi_read(nvram_t *dev, uint8_t *dst, uint32_t src, size_t len);

/**
 * @brief Copy data from NVRAM to system memory.
 *
 * This is a special form of the READ command used by some Ramtron/Cypress 4Kbit
 * FRAM devices which puts the 9th address bit inside the command byte to be
 * able to use one byte for addressing instead of two.
 *
 * @param[in]  dev   Pointer to NVRAM device descriptor
 * @param[in]  dst   Pointer to the first byte in the system memory address space
 * @param[in]  src   Starting address in the NVRAM device address space
 * @param[in]  len   Number of bytes to copy
 *
 * @return          Number of bytes written on success
 * @return          <0 on errors
 */
int nvram_spi_read_9bit_addr(nvram_t *dev, uint8_t *dst, uint32_t src, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_NVRAM_SPI_H_ */
/** @} */
