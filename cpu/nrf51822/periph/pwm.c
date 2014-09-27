/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_nrf51822
 * @{
 *
 * @file
 * @brief       Low-level PWM driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/pwm.h"

/* only compile this driver if there is at least one PWM device defined */
#if PWM_NUMOF

int pwm_init(pwm_t dev, pwm_mode_t mode, unsigned int frequency, unsigned int resolution)
{
    switch (dev) {
        case PWM_0:
            PWM_0_DEV->MODE = TIMER_MODE_MODE_Timer;
            PWM_0_DEV->BITMODE = TIMER_BITMODE_BITMODE_16Bit;
            PWM_0_DEV->CC[3] = resolution;
            /* TODO: prescaler calculation */
            clk = frequency * resolution;
            pre = F_CPU / clk;
            PWM_0_PRESCALER = pre;

            /* enable the PWM timer */
            PWM_0_DEV->TASKS_START = 1;
            break;
        default:
            return -2;
    }

    /* calculate prescale factor for the timer */

    /* configure output pins */
    NRF_GPIOTE->CONFIG[PWM_0_GPIOTE_CH0] = (GPIOTE_CONFIG_MODE_Task <<  GPIOTE_CONFIG_MODE_Pos) |
                                           (PWM_0_PIN_CH0 << GPIOTE_CONFIG_PSEL_Pos) |
                                           (GPIOTE_CONFIG_POLARITY_HiToLo << GPIOTE_CONFIG_POLARITY_Pos) |
                                           (GPIOTE_CONGIG_OUTINIT_Low << GPIOTE_CONFIG_OUTINIT_Pos);


    return 0;
}

int pwm_set(pwm_t dev, int channel, unsigned int value)
{
    NRF_TIMER_Type *pwm;

    switch (dev) {
        case PWM_0:
            pwm = PWM_0_DEV;
            break;
        default:
            return -2;
    }

    switch (channel) {
        case 0:
            pwm->CC[0] = value;
            break;
        case 1:
            pwm->CC[1] = value;
            break;
        case 2:
            pwm->CC[2] = value;
            break;
        default:
            return -1;
    }
    return 0;
}

void pwm_start(pwm_t dev)
{
    switch (dev) {
        case PWM_0:
            PWM_0_DEV->TASKS_START = 1;
            break;
        default:
            return;
    }
}

void pwm_stop(pwm_t dev)
{
    switch (dev) {
        case PWM_0:
            PWM_0_DEV->TASKS_STOP = 1;
            break;
        default:
            return;
    }
}

void pwm_poweron(pwm_t dev)
{
    switch(dev) {
        case PWM_0:
            PWM_0_DEV->POWER = 1;
            break;
        default:
            return;
    }
}

void pwm_poweroff(pwm_t dev)
{
    switch (dev) {
        case PWM_0:
            PWM_0_DEV->POWER = 0;
            break;
        default:
            return;
    }
}

#endif /* PWM_NUMOF */
