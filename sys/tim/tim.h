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

#ifdef __cplusplus
extern "C" {
#endif


#define TIM_MSG_TIMEOUT             0xf001

int tim_init(void);

int tim_sleep(time_t *sleep);
int tim_usleep(uint32_t usec);
int tim_msleep(uint32_t msec);
int tim_ssleep(uint32_t sec);

int tim_timeout()




#if TIM_TIMEX
int tim_sleep(timex_t time);
#endif


int tim_uptime(struct timeval *uptime);
int tim_now(timex_t *now);
int tim_get_localtime(struct tm *time);




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




#ifdef __cplusplus
}
#endif

#endif /* __TIM_H */
/** @} */
