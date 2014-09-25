/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_samd21
 * @{
 *
 * @file
 * @brief       Low-level PWM driver implementation
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 *
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "periph/pwm.h"
#include "periph_conf.h"

/* ignore file in case no PWM devices are defined */
#if PWM_NUMOF

int pwm_init(pwm_t dev, pwm_mode_t mode, unsigned int frequency, unsigned int resolution)
{
    Tcc *pwm;
    PortGroup *port;
    uint8_t pin[PWM_MAX_CHANNELS];
    int channels;
    int cc;
    int tmp;

    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            pwm = PWM_0_DEV;
            port = PWM_0_PORT;
            pin[0] = PWM_0_PIN_CH0;
            pin[1] = PWM_0_PIN_CH1;
            pin[2] = PWM_0_PIN_CH2;
            pin[3] = PWM_0_PIN_CH3;
            channels = PWM_0_CHANNELS;
            cc = PWM_0_CH_BITS;
            PWM_0_CLK = 1;
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            pwm = PWM_1_DEV;
            port = PWM_1_PORT;
            pin[0] = PWM_1_PIN_CH0;
            pin[1] = PWM_1_PIN_CH1;
            channels = PWM_1_CHANNELS;
            cc = PWM_1_CH_BITS;
            PWM_1_CLK = 1;
            break;
#endif
        default:
            return -1;
    }

    /* configure pins */
    for (int i = 0; i < channels; i++) {
        tmp = pin & 0x01;
        port.DIRSET = (1 << pin[i]);
        port.PINCFG[pin[i]].bit.PMUXEN = 1;
        port.PMUX[pin[i] >> 1].reg = (0x5 << (tmp * 4));
    }

    /* reset peripheral */
    pwm->CTRLA.reg = 0;

    switch (mode) {
        case PWM_LEFT:

            break;
        case PWM_RIGHT:

            break;
        case PWM_CENTER:

            break;
    }

    /* enable the counter */
    pwm->CTRLA.bit.ENABLE = 1;

    return 0;
}

int pwm_set(pwm_t dev, int channel, unsigned int value)
{
    TIM_TypeDef *tim = NULL;

    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            break;
#endif
    }

    /* norm value to maximum possible value */
    if (value > 0xffff) {
        value = 0xffff;
    }

    switch (channel) {
        case 0:
            tim->CCR1 = value;
            break;
        case 1:
            tim->CCR2 = value;
            break;
        case 2:
            tim->CCR3 = value;
            break;
        case 3:
            tim->CCR4 = value;
            break;
        default:
            return -1;
    }

    return 0;
}

void pwm_start(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            PWM_0_DEV->CR1 |= TIM_CR1_CEN;
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            PWM_1_DEV->CR1 |= TIM_CR1_CEN;
            break;
#endif
    }
}

void pwm_stop(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            PWM_0_DEV->CR1 &= ~(TIM_CR1_CEN);
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            PWM_1_DEV->CR1 &= ~(TIM_CR1_CEN);
            break;
#endif
    }
}

void pwm_poweron(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            PWM_0_CLK = 1;
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            PWM_1_CLK = 1;
            break;
#endif
    }
}

void pwm_poweroff(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            PWM_0_CLK = 0;
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            PWM_1_CLK = 0;
            break;
#endif
    }
}

#endif /* PWM_NUMOF */
