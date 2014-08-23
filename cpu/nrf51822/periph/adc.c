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
 * @brief       Low-level ADC driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/adc.h"
#include "periph_conf.h"

/* guard this file in case no ADC device is defined */
#if ADC_NUMOF

static int adc_precision;

int adc_init(adc_t dev, adc_precision_t precision)
{
    if (dev != ADC_0) {
        return -2;
    }

    /* power on the ADC peripheral */
    NRF_ADC->POWER = 1;

    /* set precision, the NRF51822 only supports 8 and 10bit */
    switch (precision) {
        case ADC_RES_8BIT:
            precision = 8;
            NRF_ADC->CONFIG = ADC_CONFIG_RES_8bit;
            break;
        case ADC_RES_10BIT:
            NRF_ADC->CONFIG = ADC_CONFIG_RES_10bit;
            break;
        default:
            NRF_ADC->POWER = 0;     /* power off again */
            return -1;
    }

    /* enable the ADC device */
    NRF_ADC->ENABLE = 1;
}

int adc_sample(adc_t dev, int channel)
{
    if (dev != ADC_0) {
        return -2;
    }

    /* wait until device is ready */
    while (NRF_ADC->BUSY == 0);

    /* set channel to sample */
    NRF_ADC->CONFIG &= ~ADC_CONFIG_PSEL_Msk;
    switch (channel) {
        case 0:
            NRF_ADC->CONFIG |= (ADC_CH0_AIN << ADC_CONFIG_PSEL_Pos);
            break;
        case 1:
            NRF_ADC->CONFIG |= (ADC_CH1_AIN << ADC_CONFIG_PSEL_Pos);
            break;
        default:
            return -1;
    }

    /* start conversion */
    NRF_ADC->TASKS_START = 1;
    /* wait until finished */
    while (NRF_ADC->EVENTS_END == 0);
    NRF_ADC->EVENTS_END = 0;
    /* fetch result */
    return (int)NRF_ADC->RESULT;
}

void adc_poweron(adc_t dev)
{
    if (dev == ADC_0) {
        NRF_ADC->POWER = 1;
    }
}

void adc_poweroff(adc_t dev)
{
    if (dev == ADC_0) {
        NRF_ADC->POWER = 0;
    }
}

int adc_map(adc_t dev, int value, int min, int max);

float adc_mapf(adc_t dev, int value, float min, float max);

#endif /* ADC_NUMOF */
