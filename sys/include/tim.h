/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    core_tim Core timing functionality
 * @ingroup     core
 * @brief       Global timer API
 *
 * @{
 *
 * @file
 * @brief       Global timer API for unified time abstraction
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __TIM_H
#define __TIM_H

#include <stdint.h>

#include "tim_arch.h"
#include "kernel_types.h"
#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief the number of micro-seconds that are needed to switch between two tasks
 *        and back on a specific platform
 */
#ifndef TIM_ARCH_SPIN_BARRIER
#warning "TIM_ARCH_SPIN_BARRIER not defined for your platform, using default value: 20"
#define TIM_ARCH_SPIN_BARRIER               (20U)
#endif

#ifndef TIM_ARCH_IRQ_DELAY
#warning "TIM_ARCH_IRQ_DELAY not defined for your platform, using default value: 500"
#define TIM_ARCH_IRQ_DELAY                  (500U)
#endif

// #ifndef TIM_ARCH_TIMER_BARRIER
// #warning "TIM_ARCH_TIMER_BARRIER not defined for your platform, using default value 65000"
// #define TIM_ARCH_TIMER_BARRIER              (65000U)
// #endif
/**
 * @brief the number of overall available hardware timer channels
 *
 * This number is a sum of the number of hardware timers times the number of
 * channels on each hardware timer.
 */
#ifndef TIM_ARCH_CHANNELS
#error "TIM_ARCH_CHANNELS not defined! You have to initialize the CHANNELS for your platform"
#endif



// #ifndef TIM_ARCH_FTICKS
// #error "TIM_ARCH_FTICKS not defined for your platform!"
// #endif

// #ifndef TIM_ARCH_FTICK_MAX
// #error "TIM_ARCH_FTICK_MAX not defined for your platform!"
// #endif

// #ifndef TIM_ARCH_TIMER_MAX_VALUE
// #error FOO
// #endif


#define MSG_TIM_ONESHOT         (0x8001)
#define MSG_TIM_TIMEOUT         (0x8002)

typedef void(*tim_cb_t)(void *);

typedef struct {
    int chan;
    uint32_t fticks;
    uint32_t sticks;
    tim_cb_t cb;
    union {
        mutex_t lock;
        struct {
            kernel_pid_t pid;
            void *arg;
        } msg;
    } data;
    uint32_t last_f;
    uint32_t last_t;
} tim_t;

#define TIM_MSG_TIMEOUT             0xf001

int tim_init(void);

int tim_sleep(time_t *sleep);
int tim_usleep(tim_t *tim, uint32_t usec);
int tim_msleep(uint32_t msec);
int tim_ssleep(uint32_t sec);

int tim_timeout(tim_t *tim, uint32_t usec, void *arg);

int tim_periodic(tim_t *tim, uint32_t usec, void *arg);


// #if TIM_TIMEX
// int tim_sleep(timex_t time);
// #endif
// int tim_uptime(struct timeval *uptime);
// int tim_now(timex_t *now);
// int tim_get_localtime(struct tm *time);


/*

    vtimer_init
vtimer_now
vtimer_gettimeofday
vtimer_get_localtime
vtimer_init
    vtimer_usleep
    vtimer_sleep
vtimer_set_msg
vtimer_set_wakeup
    vtimer_remove
vtimer_msg_receive_timeout

hwtimer_init
hwtimer_now
hwtimer_set
hwtimer_set_abs
    hwtimer_remove
hwtimer_wait
hwtimer_active
hwtimer_init_copm
httimer_spin

timer_init
timer_set
timer_set_abs
    timer_clear
timer_read
timer_start
timer_stop
timer_irq_en
timer_irq_dis
timer_reset

rtt_init
rtt_get_counter
rtt_set_alarm
rtt_get_alarm
rtt_clear_alarm

rtc_init
rtc_set_time
rtc_get_time
rtc_set_alarm
rtc_get_alarm
rtc_clear_alarm

*/


#ifdef __cplusplus
}
#endif

#endif /* __TIM_H */
/** @} */
