/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_k20
 * @{
 *
 * @file
 * @brief       GPIO driver implementation
 *
 * @author      Finn Wilke <finn.wilke@fu-berlin.de>
 *
 * @}
 */


#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/gpio.h"
#include "periph_conf.h"


int k20_gpio_direction(GPIO_Type *gpio, uint8_t pinid, bool output)
{
    if (pinid >= 32) {
        return -1;
    }

    if (gpio) {
        if (output) {
            gpio->PDDR |= (1 << pinid);
        } else {
            gpio->PDDR &= ~(1 << pinid);
        }

        return 0;
    }

    return -1;
}

static int _gpio_pin_activate(GPIO_Type *gpio, uint8_t pinid, bool high_power)
{
    if (pinid >= 32) {
        return -1;
    }

    PORT_Type *port;

    /* Enable the clock gate for the corresponding port first */
    if (gpio == PTA) {
        PORTA_CLKEN();
        port = PORTA;
    }
    else if (gpio == PTB) {
        PORTB_CLKEN();
        port = PORTB;
    }
    else if (gpio == PTC) {
        PORTC_CLKEN();
        port = PORTC;
    }
    else if (gpio == PTD) {
        PORTD_CLKEN();
        port = PORTD;
    }
    else if (gpio == PTE) {
        PORTE_CLKEN();
        port = PORTE;
    }
    else {
        return -1;
    }

    port->PCR[pinid] |= ((uint32_t)high_power << PORT_PCR_DSE_SHIFT)
                     |  (001 << PORT_PCR_MUX_SHIFT);

    return 0;
}

static int _gpio_enable(GPIO_Type *gpio, uint8_t pinid, bool output, bool high_power)
{
    if(_gpio_pin_activate(gpio, pinid, high_power) != 0) {
        return -1;
    }

    return k20_gpio_direction(gpio, pinid, output);
}

int k20_gpio_enable_output(GPIO_Type *gpio, uint8_t pinid, bool high_power)
{
    return _gpio_enable(gpio, pinid, true, high_power);
}
int k20_gpio_enable_input(GPIO_Type *gpio, uint8_t pinid)
{
    return _gpio_enable(gpio, pinid, false, false);
}
