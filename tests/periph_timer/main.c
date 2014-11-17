/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the peripheral timer driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "periph_conf.h"
#include "periph/timer.h"
#include "thread.h"

typedef struct {
    uint8_t timer;
    uint8_t channel;
    uint32_t max;
    uint32_t last;
    uint32_t interval;
} test_t;

void cb(void *arg)
{
    test_t *data = (test_t *)arg;
    uint32_t now;

    timer_read(data->timer, &now);
    printf("Callback TIMER_%i - CH_%i: was set to %u, now is %u\n",
           data->timer, data->channel, (unsigned)data->last, (unsigned)now);
    data->last = (data->last + data->interval) & data->max;
    timer_set_abs(data->timer, data->channel, data->last, cb, arg);
}


int main(void)
{
    uint64_t tmp;

#if TIMER_0_EN
    test_t t0[TIMER_0_CHANNELS];
#endif
#if TIMER_1_EN
    test_t t1[TIMER_1_CHANNELS];
#endif
#if TIMER_2_EN
    test_t t2[TIMER_2_CHANNELS];
#endif
#if TIMER_3_EN
    test_t t3[TIMER_3_CHANNELS];
#endif

    puts("\nRIOT low-level timer driver test...\n");

    puts("This test will initialize and enable all configured timers. Afterwards each channel");
    puts("of each timer is continuously set.\n");

    /*
     * Initialize all available timers
     */
#if TIMER_0_EN
    puts("Initializing TIMER_0:");
    printf("    Frequency: %8iHz\n", (int)TIMER_0_FREQUENCY);
    printf("    Max Value: 0x%08x\n", TIMER_0_MAX_VALUE);
    printf("    Channels:  %10i\n", TIMER_0_CHANNELS);
    puts("");
    if (timer_init(TIMER_0) == 0) {
        puts("TIMER_0 initialization...     [OK]\n\n");
    } else {
        puts("TIMER_0 initialization...     [FAIL]\n");
        return -1;
    }
#endif

#if TIMER_1_EN
    puts("Initializing TIMER_1:");
    printf("    Frequency: %8iHz\n", (int)TIMER_1_FREQUENCY);
    printf("    Max Value: 0x%08x\n", TIMER_1_MAX_VALUE);
    printf("    Channels:  %10i\n", TIMER_1_CHANNELS);
    puts("");
    if (timer_init(TIMER_1) == 0) {
        puts("TIMER_1 initialization...     [OK]\n\n");
    } else {
        puts("TIMER_1 initialization...     [FAIL]\n");
        return -1;
    }
#endif

#if TIMER_2_EN
    puts("Initializing TIMER_2:");
    printf("    Frequency: %8iHz\n", (int)TIMER_2_FREQUENCY);
    printf("    Max Value: 0x%08x\n", TIMER_2_MAX_VALUE);
    printf("    Channels:  %10i\n", TIMER_2_CHANNELS);
    puts("");
    if (timer_init(TIMER_2) == 0) {
        puts("TIMER_2 initialization...     [OK]\n\n");
    } else {
        puts("TIMER_2 initialization...     [FAIL]\n");
        return -1;
    }
#endif

#if TIMER_3_EN
    puts("Initializing TIMER_3:");
    printf("    Frequency: %8iHz\n", (int)TIMER_3_FREQUENCY);
    printf("    Max Value: 0x%08x\n", TIMER_3_MAX_VALUE);
    printf("    Channels:  %10i\n", TIMER_3_CHANNELS);
    puts("");
    if (timer_init(TIMER_3) == 0) {
        puts("TIMER_3 initialization...     [OK]\n\n");
    } else {
        puts("TIMER_3 initialization...     [FAIL]\n");
        return -1;
    }
#endif

    /*
     * Do the actual testing
     */
#if TIMER_0_EN
     for (int i = 0; i < TIMER_0_CHANNELS; i++) {
        t0[i].timer = 0;
        t0[i].channel = i;
        t0[i].max = TIMER_0_MAX_VALUE;
        t0[i].interval = (TIMER_0_MAX_VALUE >> (i + 1));
        t0[i].last = t0[i].interval;
        tmp = t0[i].interval;
        tmp = (tmp * 1000000) / TIMER_0_FREQUENCY;
        printf("TIMER_0: CH_%i: will trigger every %u ticks (%uus)\n", i,
               (unsigned)t0[i].interval, (unsigned)tmp);
        timer_set_abs(TIMER_0, i, t0[i].last, cb, (void*)(&(t0[i])));
     }
     puts("");
#endif

    /* prevent thread from exiting to keep stack clean */
     thread_sleep();
    return 0;
}
