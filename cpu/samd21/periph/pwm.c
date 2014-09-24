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
#include <string.h>

#include "cpu.h"
#include "periph/pwm.h"
#include "periph_conf.h"

/* ignore file in case no PWM devices are defined */
#if PWM_NUMOF

int pwm_init(pwm_t dev, pwm_mode_t mode, unsigned int frequency, unsigned int resolution)
{
    Tcc *pwm;

    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            pwm = PWM_0_DEV;
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            pwm = PWM_0_DEV;
            break;
#endif
        default:
            return -1;
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
            PWM_0_CLKEN();
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            PWM_1_CLKEN();
            break;
#endif
    }
}

void pwm_poweroff(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            PWM_0_CLKDIS();
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            PWM_1_CLKDIS();
            break;
#endif
    }
}

#endif /* PWM_NUMOF */
