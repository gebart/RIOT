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
 * @brief       Low-level UART driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/adc.h"
#include "periph_conf.h"

/* guard file in case no ADC device is defined */
#if ADC_NUMOF

/* save the maximum value configured for the ADC */
int adc_max_value;

int adc_init(adc_t dev, adc_precision_t precision)
{
    /* the NRF51822 only supports one ADC... */
    if (dev != ADC_0) {
        return -2;
    }

    /* power on ADC */
    NRF_ADC->POWER = 1;
    /* configure ADC, set precision, internal reference to VBG */
    switch (precision) {
        case ADC_RES_8BIT:
            adc_max_value = 255;
            NRF_ADC->CONFIG = ADC_CONFIG_RES_8bit;
            break;
        case ADC_RES_10BIT:
            adc_max_value = 1023;
            NRF_ADC->CONFIG = ADC_CONFIG_RES_10bit;
            break;
        default:
            NRF_ADC->POWER = 0;
            return -1;
    }
    /* enable the ADC */
    NRF_ADC->ENABLE = 1;
    return 0;
}

int adc_sample(adc_t dev, int channel)
{
    if (dev != ADC_0) {
        return -2;
    }

    /* set channel */
    NRF_ADC->CONFIG &= ~(ADC_CONFIG_PSEL_Msk);
    switch (channel) {
        case 0:
            NRF_ADC->CONFIG |= ADC_0_CH0 << ADC_CONFIG_PSEL_Pos;
            break;
        case 1:
            NRF_ADC->CONFIG |= ADC_0_CH1 << ADC_CONFIG_PSEL_Pos;
            break;
        case 2:
            NRF_ADC->CONFIG |= ADC_0_CH2 << ADC_CONFIG_PSEL_Pos;
            break;
        case 3:
            NRF_ADC->CONFIG |= ADC_0_CH3 << ADC_CONFIG_PSEL_Pos;
            break;
        default:
            return -1;
    }

    /* start conversion */
    NRF_ADC->TASKS_START = 1;

    /* wait for conversion to be complete */
    while (NRF_ADC->EVENTS_END == 0);
    NRF_ADC->EVENTS_END = 1;

    /* return result */
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

int adc_map(adc_t dev, int value, int min, int max)
{
    return (int)adc_mapf(dev, value, (float)min, (float)max);
}

float adc_mapf(adc_t dev, int value, float min, float max)
{
    return ((max - min) / ((float)adc_max_value)) * value;
}

#endif /* ADC_NUMOF */
