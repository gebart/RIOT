
#include <stdint.h>

#include "tim.h"
#include "periph/timer.h"
#if TIM_ARCH_STIMER_RTT
#include "periph/rtt.h"
#endif
#include "thread.h"
#include "mutex.h"
#include "lifo.h"
#include "msg.h"
#include "irq.h"
#include "atomic.h"
#include "kernel_types.h"

#define ENABLE_DEBUG (1)
#include "debug.h"


/**
 * @brief Static array mapping logical TIM channels to timer/channel tuples
 */
static const uint8_t timer[] = TIM_ARCH_TIMER_MAP;

/**
 * @brief Static array holding the logical to real timer channel mapping
 */
static const uint8_t channel[] = TIM_ARCH_CHANNEL_MAP;

/**
 * @brief LIFO holding the currently unused timer channels
 */
static int tim_lifo[TIM_ARCH_CHANNELS + 1];

/**
 * @brief Head of pending slow timers
 */
static tim_t *stimer_pending;

/**
 * @brief Count the overflows of the slow timer for enhanced tim range
 */
static unsigned int tim_dino_ticks = 0;


/**
 * @brief Convert micro-seconds to fast timer ticks
 *
 * @param[in] us            timer value in micro-seconds
 *
 * @return                  number of fast ticks
 */
static void _tim_us_to_ticks(uint32_t us, tim_ticks_t *ticks);

static int _tim_stimer_set(tim_t *tim);
static int _tim_ftimer_set(tim_t *tim);

static void _tim_stimer_next(void);

static void _tim_cb_sleep(void *arg);
static void _tim_cb_msg(void *arg);
static void _tim_cb_periodic(void *arg);
static void _tim_cb_stimer(void *arg);
static void _tim_cb_stimer_ovf(void *arg);


int tim_init(void)
{
    uint32_t timers = 0;

    for (int i = 0; i < (TIM_ARCH_CHANNELS * 2); i += 2) {
        /* see if timer was already initialized */
        if (timers & (1 << timer[i])) {
            timers |= (1 << timer[i]);
            timer_init(timer[i]);
        }
    }
#ifdef TIM_ARCH_STIMER_RTT
    rtt_init();
    rtt_set_overflow_cb(_tim_cb_stimer_ovf, 0);
#else
    timer_init(TIM_ARCH_STIMER);
    timer_on_overflow(TIM_ARCH_STIMER, _tim_cb_stimer_ovf, 0);
#endif
    return 0;
}

int tim_usleep(uint32_t usec)
{
    tim_t tim;

    /* save current time for here higher precision */
    timer_read(timer[0], &(tim.last.fast));
    /* compute fticks from given time */
    _tim_us_to_ticks(usec, &(tim.ticks));
    /* prepare mutex */
    mutex_init(&(tim.data.lock));
    mutex_lock(&(tim.data.lock));
    /* set callback */
    tim.cb = _tim_cb_sleep;
    /* clear next field */
    tim.next = NULL;

    DEBUG("tim_usleep: setting to %i us == %i sticks and %i fticks\n",
          (unsigned)usec, (unsigned)tim.ticks.fast, (unsigned)tim.ticks.slow);

    /* figure out if the fast timer is sufficient for the amount of ticks */
    if (tim.ticks.slow == 0) {
        if (tim.ticks.fast < TIM_ARCH_SPIN_BARRIER) {
            uint32_t now;
            tim.last.fast = (tim.last.fast + tim.ticks.fast) & TIM_ARCH_FMAX;

            DEBUG("tim_usleep: interval too short to sleep, taking a spin\n");
            do {
                timer_read(timer[0], &now);
            } while (now != tim.last.fast);
            return 0;
        }
        else {
            _tim_ftimer_set(&tim);
        }
    }
    else {
        _tim_stimer_set(&tim);
    }
    mutex_lock(&(tim.data.lock));
    return 0;
}

int tim_timeout(tim_t *tim, uint32_t usec, void *arg)
{
    /* save current time right now for higher precision */
    timer_read(timer[0], &(tim->last.fast));
    /* convert given time in us to ticks */
    _tim_us_to_ticks(usec, &(tim->ticks));
    /* remember the callback argument and put the PID */
    tim->data.msg.pid = thread_getpid();
    tim->data.msg.arg = arg;
    /* set callback */
    tim->cb = _tim_cb_msg;
    /* clear next field */
    tim->next = NULL;

    DEBUG("tim_timeout: setting to %i us == %i sticks and %i fticks\n",
          (unsigned)usec, (unsigned)tim->ticks.fast, (unsigned)tim->ticks.slow);

    /* figure out if the fast timer is sufficient for the amount of ticks */
    if (tim->ticks.slow == 0) {
        if (tim->ticks.fast < TIM_ARCH_SPIN_BARRIER) {
            msg_t msg;
            /* time too short for waiting, send message directly */
            msg.type = MSG_TIM_TIMEOUT;
            msg.content.ptr = tim->data.msg.arg;
            msg_send(&msg, tim->data.msg.pid);
        }
        else {
            _tim_ftimer_set(tim);
        }
    }
    else {
        _tim_stimer_set(tim);
    }
    return 0;
}

int tim_clear(tim_t *tim)
{
    unsigned state;

//     if (tim->ticks.slow == 0) {
//         /* clear timer channel */
//         timer_clear(timer[tim->chan], channel[tim->chan]);
//         /* free logical channel */
//         state = disableIRQ();
//         lifo_insert(tim_lifo, tim->chan);
//         restoreIRQ(state);
//     }
//     else {
//         if (tim == stimer_active) {
// #ifdef TIM_ARCH_STIMER_RTT
//             rtt_clear_alarm();
// #else
//             timer_clear(timer[tim->chan], channel[tim->chan]);
// #endif
//             _tim_stimer_next();
//         }
//         else {
//             tim_t *next = stimer_pending;
//             if (stimer_pending == tim) {
//                 state = disableIRQ();
//                 stimer_pending = tim->next;
//                 restoreIRQ(state);
//             }
//             while (next) {
//                 if (next->next == tim) {
//                     state = disableIRQ();
//                     next->next = tim->next;
//                     restoreIRQ(state);
//                 }
//                 next = next->next;
//             }
//         }
//     }
    return 0;
}

static int _tim_ftimer_set(tim_t *tim)
{
    /* get a timer channel to use */
    unsigned state = disableIRQ();
    tim->chan = lifo_get(tim_lifo);
    restoreIRQ(state);
    if (tim->chan < 0) {
        /* no free channel available */
        return -1;
    }
    DEBUG("_tim_ftimer_set: chose channel %i -> TIMER_%i, chan %i\n",
            tim->chan, timer[tim->chan], channel[tim->chan]);
    /* set fast timer */
    tim->last.fast = (tim->last.fast + tim->ticks.fast) & TIM_ARCH_FMAX;
    timer_set_abs(timer[tim->chan], channel[tim->chan], tim->last.fast, tim->cb, tim);
    return 0;
}

static int _tim_stimer_set(tim_t *tim)
{
    tim->last.sticks = (tim->last.slow + tim->ticks.slow) & TIM_ARCH_SMAX;

    if (stimer_pending == NULL) {
        stimer_pending = tim;
        _tim_stimer_next();
    } else {
        if ()
    }
    return 0;
}

static void _tim_stimer_next(void)
{
    if (stimer_pending != NULL) {
        unsigned state = disableIRQ();
        stimer_active = stimer_pending;
        stimer_pending = stimer_pending->next;
        restoreIRQ(state);
#ifdef TIM_ARCH_STIMER_RTT

        rtt_set_alarm(stimer_active->last.slow, _tim_cb_stimer, stimer_active);
#else
        timer_set_abs(TIM_ARCH_STIMER, 0, stimer_active->last.slow,
                      stimer_active->irq, stimer_active);
#endif
    }
}


static void _tim_cb_sleep(void *arg)
{
    tim_t *tim = (tim_t *)arg;
    /* no need to guard the lifo call, as we expect this code to run inside an ISR */
    lifo_insert(tim_lifo, tim->chan);
    /* wake-up the sleeping thread */
    mutex_unlock(&(tim->data.lock));
}

static void _tim_cb_msg(void *arg)
{
    msg_t msg;
    tim_t *tim = (tim_t *)arg;
    /* no need to guard the lifo call, as we expect this code to run inside an ISR */
    lifo_insert(tim_lifo, tim->chan);
    /* send out message */
    msg.type = MSG_TIM_TIMEOUT;
    msg.content.ptr = tim->data.msg.arg;
    msg_send_int(&msg, tim->data.msg.pid);
}

static void _tim_cb_periodic(void *arg)
{
    msg_t msg;
    tim_t *tim = (tim_t *)arg;

    msg.type = MSG_TIM_PERIODIC;
    msg.content.ptr = tim->data.msg.arg;
    msg_send_int(&msg, tim->data.msg.pid);

    if (tim->ticks.slow == 0) {
        _tim_ftimer_set(tim);
    }
    else {
        _tim_stimer_set(tim);
    }
}

static void _tim_cb_stimer(void *arg)
{
    uint32_t now;
    tim_t *tim = (tim_t *)arg;

    /* get current value of ftimer */
    timer_read(timer[0], &now);

    /* make next timer active */
    _tim_stimer_next();
    /* set ftimer */
    tim->chan = lifo_get(tim_lifo);
    if (tim->chan >= 0) {
        tim->last.fast = (now + tim->ticks.fast) & TIM_ARCH_FMAX;
        timer_set_abs(timer[tim->chan], channel[tim->chan], tim->last.fast, tim->cb, tim);
    }

}

static void _tim_cb_stimer_ovf(void *arg)
{
    (void)arg;
    atomic_set_return(&tim_dino_ticks, tim_dino_ticks + 1);
}


/* convert between different time bases */
static void _tim_us_to_ticks(uint32_t us, tim_ticks_t *ticks)
{
    uint64_t tmp;

#if TIM_ARCH_FCLK >= (1000000U)
    tmp = us * (TIM_ARCH_FCLK / (1000000UL));
#else
    tmp = us / ((1000000UL) / TIM_ARCH_FCLK);
#endif
    ticks->fast = tmp & TIM_ARCH_FMAX;
    tmp = ((tmp & ~(TIM_ARCH_FMAX)) * TIM_ARCH_SCLK) / TIM_ARCH_FCLK;
    ticks->slow = (uint32_t)tmp;
}
