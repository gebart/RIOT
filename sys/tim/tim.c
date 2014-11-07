
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
static inline uint64_t _tim_us_to_ticks(uint32_t us);

static void _tim_spin(uint32_t fticks);
static int _tim_sleep(tim_t *tim, uint32_t fticks, uint32_t now);
static int _tim_sleep_deep(tim_t *tim, uint64_t ticks, uint32_t now);

static void _tim_cb_sleep(void *arg);
static void _tim_cb_msg(void *arg);
static void _tim_cb_periodic(void *arg);
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
#else
    timer_init(TIM_ARCH_STIMER);
#endif

    return 0;
}

int tim_usleep(tim_t *tim, uint32_t usec)
{
    uint64_t ticks;
    uint32_t fticks;
    uint32_t now;

    /* save current time for here higher precision */
    timer_read(timer[0], &now);
    /* compute fticks from given time */
    ticks = _tim_us_to_ticks(usec);
    /* figure out if the fast timer is sufficient for the amount of ticks */
    if (ticks & ~(TIM_ARCH_FMAX)) {
        return _tim_sleep_deep(tim, ticks, now);
    }
    else {
        fticks = (uint32_t)ticks;
        if (ticks < TIM_ARCH_SPIN_BARRIER) {
            _tim_spin(fticks);
            return 0;
        }
        else {
            return _tim_sleep(tim, fticks, now);
        }
    }
}

static void _tim_spin(uint32_t fticks)
{
    uint32_t now;

    do {
        timer_read(timer[0], &now);
    } while (now != ((now + fticks) & TIM_ARCH_FMAX));
}

static int _tim_sleep(tim_t *tim, uint32_t fticks, uint32_t now)
{
    /* get a timer channel to use */
    unsigned state = disableIRQ();
    int chan = lifo_get(tim_lifo);
    restoreIRQ(state);
    if (chan < 0) {
        /* no free channel available */
        return -1;
    }
    /* set needed fields from tim data structure */
    tim->chan = chan;
    mutex_init(&(tim->data.lock));
    mutex_lock(&(tim->data.lock));
    /* set fast timer */
    fticks = (now + fticks) & TIM_ARCH_FMAX;
    timer_set_abs(timer[chan], channel[chan], fticks, _tim_cb_sleep, tim);
    /* lock (the already locked mutex) to put the thread in blocked state */
    mutex_lock(&(tim->data.lock));
}

static int _tim_sleep_deep(tim_t *tim, uint64_t ticks, uint32_t now)
{
    return -1;
}

// int tim_timeout(tim_t *tim, uint32_t usec, void *arg)
// {
//     unsigned state;
//     uint32_t fticks;
//     uint32_t now

//     /* save current time for higher precision */
//     now = timer_read(timer_inst[0]);

//     /* compute timer FTICKS */
//     fticks = _tim_us_to_fticks(usec);
//     if (fticks < TIM_ARCH_SPIN_BARRIER) {
//         return -1;
//     }

//     state = disableIRQ();
//     chan = lifo_get(&tim_lifo);
//     restoreIRQ(state);
//     if (chan < 0) {
//         return -1;
//     }

//     tim->chan = chan;
//     tim->pid = pid;

//     fticks = (now + fticks) & TIM_ARCH_TIMER_MAX_VALUE;
//     timer_set_cb(chan[chan]->timer, chan[chan]->chan, _tim_cb_timeout, tim);
//     timer_set_abs(chan[chan]->timer, chan[chan]->chan, fticks);
//     return 0;
// }

// void _tim_cb_msg(void *arg)
// {
//     tim_timeout_t *tim = (tim_timeout_t *)arg;
//     msg_t msg;
//     unsigned state;

//     msg.type = MSG_TIM_TIMEOUT;
//     msg_send(&msg, tim->pid);

//     state = disableIRQ();
//     lifo_insert(tim_lifo, tim->chan);
//     restoreIRQ(state);
// }

// int tim_periodic(tim_t *tim, uint32_t usec, void *arg)
// {
//     int chan;
//     unsigned state;

//     /* compute timer FTICKS */
//     tim->period = _tim_us_to_fticks(usec);
//     if (tim->period < TIM_ARCH_SPIN_BARRIER) {
//         return -2;
//     }

//     state = disableIRQ();
//     chan = lifo_get(&tim_lifo);
//     restoreIRQ(state);
//     if (chan < 0) {
//         return -1;
//     }

//     tim->chan = (uint8_t)chan;
//     tim->last = (timer_read(chan[0]) + tim->period) & TIM_ARCH_TIMER_MAX_VALUE;

//     timer_set_cb(chan[chan]->timer, chan[chan]->chan);
//     timer_set_abs(chan[chan]->timer, chan[chan]->chan, tim->last);
//     return 0;
// }

// void _tim_cb_periodic(void *arg)
// {
//     tim_periodic_t *tim = (tim_periodic_t *)arg;
//     mst_t msg;

//     msg.type = MSG_TIM_PERIODIC;
//     msg_send_int(&msg, tim->pid);

//     tim->last = (tim->last + tim->period) & TIM_ARCH_TIMER_MAX_VALUE;
//     timer_set_abs(chan[tim->chan]->timer, chan[tim->chan]->chan, tim->last);
// }

// int tim_clear(uint8_t chan)
// {
//     unsigned state;

//     if (chan < TIM_ARCH_CHANNELS) {
//         return timer_clear(chan[chan]->timer, chan[chan]->chan);
//     }

//     state = disableIRQ();
//     lifo_insert(&tim_lifo, chan);
//     restoreIRQ(state);
// }


static void _tim_cb_sleep(void *arg)
{
    tim_t *tim = (tim_t *)arg;
    /* no need to guard the lifo call, as we expect this code to run inside an ISR */
    lifo_insert(tim_lifo, tim->chan);
    /* wake-up the sleeping thread */
    mutex_unlock(&tim->data.lock);
}

static void _tim_cb_stimer_ovf(void *arg)
{
    (void)arg;
    atomic_set_return(&tim_dino_ticks, tim_dino_ticks + 1);
}



// int _tim_set(tim_t *tim, void(*cb)(void *))
// {
//     int i;

//     /* compensate interrupt delay nsec value */
//     tim->val.nsec -= TIM_ARCH_IRQ_DELAY;

//     /* if not too long of interval, we can put the thread to sleep using a hardware timer channel */
//     if (t->nsec < TIM_ARCH_TIMER_BARRIER) {
//         if (i = lifo_get(tim_lifo) < 0) {
//             return -1;
//         }
//         timer_set(ch->timer, ch->channel, _tim_cb_unlock, &lock);
//         mutex_lock(&lock);
//         mutex_lock(&lock);
//         _tim_ch_free(ch);
//     }
// #if RTT_NUMOF
//     /* else we use some slower running timer, the RTT */
//     else if (fticks < TIM_ARCH_RTT_BARRIER) {
//         if (rtt_is_set()) {
//             val = rtt_get_alarm();

//         }

//     }
// #endif
//     /* or we maybe have an RTC? */
// #if RTC_NUMOF
//     else {

//     }
// #endif

// }

// int _tim_get_ch(tim_ch_t *ch)
// {
//     unsigned state = disableIRQ();
//     if (tim_next_ch == NULL) {
//         restoreIRQ(state);
//         DEBUG("TIM-ERROR: no free timer channel -> wait will fail\n");
//         return -1;
//     }
//     ch = tim_next_ch;
//     tim_next_ch = ch->next;
//     restoreIRQ(state);
// }

// void _tim_ch_free(tim_ch_t *ch)
// {
//     ch->next = NULL;
//     unsigned state = disableIRQ();
//     if (tim_next_ch == NULL) {
//         tim_next_ch = ch;
//         tim_last_ch = ch;
//     }
//     else {
//         tim_last_ch->next = ch;
//         tim_last_ch = ch;
//     }
//     restoreIRQ(state);
// }


/* convert between different time bases */
static inline uint64_t _tim_us_to_ticks(uint32_t us)
{
#if TIM_ARCH_FCLK >= (1000000U)
    return us * (TIM_ARCH_FCLK / (1000000U));
#else
    return us / ((1000000U) / TIM_ARCH_FCLK);
#endif
}

// static inline uint32_t _tim_ms_to_fticks(uint32_t ms)
// {
//     return (ms * 1000 * 1000);
// }

// static inline uint32_t _tim_us_to_ticks(uint32_t us, uint32_t &fticks)
// {
//     if (TIM_ARCH_FTICKS == 1000) {       /* should be compiled to constant expression?! */
//         *fticks =
//     }
//     else if (TIM_ARCH_FTICKS < 1000) {
//         *fticks = (1000 / TIM_ARCH_FTICKS) * us;
//         if (*fticks > TIM_ARCH_FTICK_MAX) {
//             *fticks = *fticks & TIM_ARCH_FTICK_MAX;
//         }
//     }
//     else {

//     }
// }

// /* all of these are run in interrupt context! */
// void _tim_cb_unlock(void *arg)
// {
//     tim_ch_t *ch = (tim_ch_t *)arg;

//     mutex_unlock(ch->data->lock);
//     lifo_insert(tim_lifo, ch->index);
// }
