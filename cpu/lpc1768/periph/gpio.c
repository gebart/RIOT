/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lpc1768
 * @{
 *
 * @file
 * @brief       Implementation of the low-level GPIO driver for the LPC1768
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph_conf.h"
#include "periph/gpio.h"

/* guard file in case no GPIOs are defined */
#if GPIO_0_EN || GPIO_1_EN || GPIO_2_EN || GPIO_3_EN \
    GPIO_4_EN || GPIO_5_EN || GPIO_6_EN || GPIO_7_EN

/**
 * @brief data-structure holding the configuration for a single pin
 */
typedef struct {
    gpio_flank_t flank;         /**< the active flank(s) if configured as external interrupt */
    gpio_cb_t cb;               /**< callback to call on external interrupt */
    void *arg;                  /**< argument passed to the callback */
} config_t;

/**
 * @brief define ports
 */
static LPC_GPIO_TypeDef const* port[GPIO_NUMOF] = {
#if GPIO_0_EN
    GPIO_0_DEV,
#endif
#if GPIO_1_EN
    GPIO_1_DEV,
#endif
#if GPIO_2_EN
    GPIO_2_DEV,
#endif
#if GPIO_3_EN
    GPIO_3_DEV,
#endif
#if GPIO_4_EN
    GPIO_4_DEV,
#endif
#if GPIO_5_EN
    GPIO_5_DEV,
#endif
#if GPIO_6_EN
    GPIO_6_DEV,
#endif
#if GPIO_7_EN
    GPIO_7_DEV
#endif
}

/**
 * @brief define pins
 */
static const uint8_t pin[GPIO_NUMOF] = {
#if GPIO_0_EN
    GPIO_0_PIN,
#endif
#if GPIO_1_EN
    GPIO_1_PIN,
#endif
#if GPIO_2_EN
    GPIO_2_PIN,
#endif
#if GPIO_3_EN
    GPIO_3_PIN,
#endif
#if GPIO_4_EN
    GPIO_4_PIN,
#endif
#if GPIO_5_EN
    GPIO_5_PIN,
#endif
#if GPIO_6_EN
    GPIO_6_PIN,
#endif
#if GPIO_7_EN
    GPIO_7_PIN
#endif
}

/**
 * @brief save the pin configurations
 */
static config_t config[GPIO_NUMOF];


int gpio_init_out(gpio_t dev, gpio_pp_t pullup)
{
    if (dev >= GPIO_NUMOF) {
        return -1;
    }

    port[dev]->FIODIR |= (1 << pin[dev]);
    port[dev]->FIOPIN &= ~(1 << pin[dev]);
    return 0;
}

int gpio_init_in(gpio_t dev, gpio_pp_t pullup)
{
    if (dev >= GPIO_NUMOF) {
        return -1;
    }

    port[dev]->FIODIR &= ~(1 << pin[dev]);
    return 0;
}

int gpio_init_int(gpio_t dev, gpio_pp_t pullup, gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    int res = gpio_init_in(dev, pullup);
    if (res < 0) {
        return res;
    }

    /* save pin configuration */
    config[dev].cb = cb;
    config[dev].arg = arg;
    config[dev].flank = flank;

    /* enable global GPIO interrupts */

    /* enable pin specific interrupt */
    gpio_irq_enable(dev);

}

void gpio_irq_enable(gpio_t dev)
{
    if (dev >= GPIO_NUMOF) {
        return -1;
    }



    switch (config[dev].flank) {
        case GPIO_FALLING:
            if (port[dev] == LPC_GPIO0) {
                LPC_GPIOINT->IO0IntEnF |= (1 << pin[dev]);
                LPC_GPIOINT->IO0IntEnR &= ~(1 << pin[dev]);
            }
            else if (port[dev] == LPC_GPIO2) {

            }
            break;
        case GPIO_RISING:
            if (port[dev] == LPC_GPIO0) {

            }
            else if (port[dev] == LPC_GPIO2) {

            }
            break;
        case GPIO_BOTH:
            if (port[dev] == LPC_GPIO0) {

            }
            else if (port[dev] == LPC_GPIO2) {

            }
            break;
        default:
            return;
    }
}

void gpio_irq_disable(gpio_t dev)
{
    if (dev >= GPIO_NUMOF) {
        return -1;
    }

    if (port[dev] == LPC_GPIO0) {

    }
    else if (port[dev] == LPC_GPIO2) {

    }
}

int gpio_read(gpio_t dev);

void gpio_set(gpio_t dev);

void gpio_clear(gpio_t dev);

void gpio_toggle(gpio_t dev);

void gpio_write(gpio_t dev, int value);

#endif /* GPIO_x_EN */
