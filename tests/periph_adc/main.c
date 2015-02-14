/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test case for the low-level ADC driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "cpu.h"
#include "board.h"
#include "vtimer.h"
#include "periph/adc.h"

#if ADC_NUMOF < 1
#error "Please enable at least 1 ADC device to run this test"
#endif

#define RES             ADC_RES_16BIT
#define DELAY           (100 * 1000U)

static int values[ADC_NUMOF][ADC_MAX_CHANNELS];

float kinetis_adc_to_core_temp(int value);

int main(void)
{
    puts("\nRIOT ADC peripheral driver test\n");
    puts("This test simply converts each available ADC channel about every 10ms\n\n");

    for (int i = 0; i < ADC_NUMOF; i++) {
        /* initialize result vector */
        for (int j = 0; j < ADC_MAX_CHANNELS; j++) {
            values[i][j] = -1;
        }
        /* initialize ADC device */
        printf("Initializing ADC_%i @ %i bit resolution", i, (6 + (2* RES)));
        if (adc_init(i, RES) == 0) {
            puts("    ...[ok]");
        }
        else {
            puts("    ...[failed]");
            return 1;
        }
    }

    puts("\n");
    /* Buffer 1.00 V band gap from PMC to ADC input */
    BITBAND_REG8(PMC->REGSC, PMC_REGSC_BGBE_SHIFT) = 1;
    /* Enable VREF low noise parameters */
    BITBAND_REG8(VREF->TRM, VREF_TRM_CHOPEN_SHIFT) = 1;
    VREF->SC = VREF_SC_VREFEN_MASK | VREF_SC_ICOMPEN_MASK | VREF_SC_REGEN_MASK | VREF_SC_MODE_LV(2);
    /* Select VREF as analog reference */
    ADC0->SC2 &= ~(ADC_SC2_REFSEL_MASK);
    ADC0->SC2 |= ADC_SC2_REFSEL(1);
    #undef ADC_NUMOF
    #define ADC_NUMOF 1
    while (1) {
        /* convert each channel for this ADC device */
        for (int i = 0; i < ADC_NUMOF; i++) {
            for (int j = 0; j < ADC_MAX_CHANNELS; j++) {
                values[i][j] = adc_sample(i, j);
            }
        }

        /* print the results */
        printf("Values: ");
        for (int i = 0; i < ADC_NUMOF; i++) {
            for (int j = 0; j < ADC_MAX_CHANNELS; j++) {
                if (values[i][j] >= 0) {
                    printf("ADC_%i-CH%i: %4i  ", i, j, values[i][j]);
                }
            }
        }
        /* Compute voltage using band gap voltage */
        float bandgap = values[0][1];
        float Vrefh = 1.00f * ((float) values[0][2]) / bandgap;
        float Vrefl = 1.00f * ((float) values[0][3]) / bandgap;
        float Vtemp = 1.00f * ((float) values[0][0]) / bandgap;
        float temp = kinetis_adc_to_core_temp(Vtemp * 1000);
        printf("VH: %1d.%06u ", (int)Vrefh, (unsigned int) (Vrefh * 1000000) % 1000000);
        printf("VL: %1d.%06u ", (int)Vrefl, (unsigned int) (Vrefl * 1000000) % 1000000);
        printf("CT: %2d.%06u ", (int)temp, (unsigned int) (temp * 1000000) % 1000000);
        printf("\n");

        /* sleep a little while */
        vtimer_usleep(DELAY);
    }

    return 0;
}
