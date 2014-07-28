/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file
 * @brief       Low-level PWM peripheral driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "stdint.h"

#include "cpu.h"
#include "periph_conf.h"
#include "periph/pwm.h"

/* ignore file in case no PWM devices are defined */
#if PWM_NUMOF

int pwm_init(pwm_t dev, pwm_mode_t mode, unsigned int frequency, unsigned int resolution)
{

}

int pwm_set(pwm_t dev, int channel, unsigned int value)
{

}

void pwm_start(pwm_t dev)
{

}

void pwm_stop(pwm_t dev)
{
    if (dev == PWM_0) {

    }
}

void pwm_poweron(pwm_t dev)
{
    if (dev == PWM_0) {
        PWM_0_CLKEN();
    }
}

void pwm_poweroff(pwm_t dev);
{
    if (dev == PWM_0) {
        PWM_0_CLKDIS();
    }
}

#endif /* PWM_NUMOF */
