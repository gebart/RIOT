
#include <stdint.h>

#include "tim.h"
#include "periph/timer.h"
#include "thread.h"
#include "mutex.h"
#include "lifo.h"
#include "msg.h"
#include "kernel_types.h"

#define ENABLE_DEBUG (1)
#include "debug.h"



typedef struct {
    uint8_t timer;
    uint8_t chan;
} tim_ch_t;

static void _tim_spin(uint32_t fticks);

static void _tim_cb_sleep(void *arg);
static void _tim_cb_msg(void *arg);
static void _tim_cb_periodic(void *arg);


static const tim_ch_t channels[] = TIM_ARCH_MAP;
static int tim_lifo[TIM_ARCH_CHANNELS + 1];


int tim_init(void)
{
    uint32_t timers = 0;

    for (int i = 0; i < TIM_ARCH_CHANNELS; i++) {
        /* see if timer was already initialized */
        if (timers & (1 << channels[i].timer)) {
            timers |= (1 << channels[i].timer);
            timer_init(channels[i].timer);
        }
    }
}

int tim_usleep(tim_t *tim, uint32_t usec)
{
    unsigned state;
    uint32_t fticks;
    uint32_t now;
    uint8_t chan;

    /* save current time for higher precision */
    timer_read(channels[0].timer, &now);
    /* compute fticks from given time */
    fticks = _tim_us_to_fticks(usec);

    /* see if we can go to sleep */
    if (fticks < TIM_ARCH_SPIN_BARRIER) {
        _tim_spin(fticks);
        return 0;
    }
    else {
        state = disableIRQ();
        chan = lifo_get(&tim_lifo);
        restoreIRQ(state);

        tim->chan = chan;
        tim->data.lock = MUTEX_INIT;
        mutex_lock(&(tim->data.lock));

        fticks = (now + fticks) & TIM_ARCH_TIMER_MAX_VALUE;
        timer_set_abs(channels[tim->chan].timer, channels[tim->chan].chan, fticks, _tim_cb_sleep, tim);
        mutex_lock(&(tim->data.lock));
    }
    return 0;
}

static void _tim_spin(uint32_t fticks)
{
    uint32_t now;

    do {
        timer_read(channels[0].timer, &now);
    } while (now != ((now + fticks) & TIM_ARCH_TIMER_MAX_VALUE));
}

static void _tim_cb_sleep(void *arg)
{
    tim_t *tim = (tim_t *)arg;
    lifo_insert(&tim_lifo, tim->chan);
    mutex_unlock(&tim->data.lock);
}

int tim_timeout(tim_t *tim, uint32_t usec, void *arg)
{
    unsigned state;
    uint32_t fticks;
    uint32_t now

    /* save current time for higher precision */
    now = timer_read(timer_inst[0]);

    /* compute timer FTICKS */
    fticks = _tim_us_to_fticks(usec);
    if (fticks < TIM_ARCH_SPIN_BARRIER) {
        return -1;
    }

    state = disableIRQ();
    chan = lifo_get(&tim_lifo);
    restoreIRQ(state);
    if (chan < 0) {
        return -1;
    }

    tim->chan = chan;
    tim->pid = pid;

    fticks = (now + fticks) & TIM_ARCH_TIMER_MAX_VALUE;
    timer_set_cb(chan[chan]->timer, chan[chan]->chan, _tim_cb_timeout, tim);
    timer_set_abs(chan[chan]->timer, chan[chan]->chan, fticks);
    return 0;
}

void _tim_cb_msg(void *arg)
{
    tim_timeout_t *tim = (tim_timeout_t *)arg;
    msg_t msg;
    unsigned state;

    msg.type = MSG_TIM_TIMEOUT;
    msg_send(&msg, tim->pid);

    state = disableIRQ();
    lifo_insert(tim_lifo, tim->chan);
    restoreIRQ(state);
}

int tim_periodic(tim_t *tim, uint32_t usec, void *arg)
{
    int chan;
    unsigned state;

    /* compute timer FTICKS */
    tim->period = _tim_us_to_fticks(usec);
    if (tim->period < TIM_ARCH_SPIN_BARRIER) {
        return -2;
    }

    state = disableIRQ();
    chan = lifo_get(&tim_lifo);
    restoreIRQ(state);
    if (chan < 0) {
        return -1;
    }

    tim->chan = (uint8_t)chan;
    tim->last = (timer_read(chan[0]) + tim->period) & TIM_ARCH_TIMER_MAX_VALUE;

    timer_set_cb(chan[chan]->timer, chan[chan]->chan);
    timer_set_abs(chan[chan]->timer, chan[chan]->chan, tim->last);
    return 0;
}

void _tim_cb_periodic(void *arg)
{
    tim_periodic_t *tim = (tim_periodic_t *)arg;
    mst_t msg;

    msg.type = MSG_TIM_PERIODIC;
    msg_send_int(&msg, tim->pid);

    tim->last = (tim->last + tim->period) & TIM_ARCH_TIMER_MAX_VALUE;
    timer_set_abs(chan[tim->chan]->timer, chan[tim->chan]->chan, tim->last);
}

int tim_clear(uint8_t chan)
{
    unsigned state;

    if (chan < TIM_ARCH_CHANNELS) {
        return timer_clear(chan[chan]->timer, chan[chan]->chan);
    }

    state = disableIRQ();
    lifo_insert(&tim_lifo, chan);
    restoreIRQ(state);
}





int _tim_set(tim_t *tim, void(*cb)(void *))
{
    int i;

    /* compensate interrupt delay nsec value */
    tim->val.nsec -= TIM_ARCH_IRQ_DELAY;

    /* if not too long of interval, we can put the thread to sleep using a hardware timer channel */
    if (t->nsec < TIM_ARCH_TIMER_BARRIER) {
        if (i = lifo_get(tim_lifo) < 0) {
            return -1;
        }
        timer_set(ch->timer, ch->channel, _tim_cb_unlock, &lock);
        mutex_lock(&lock);
        mutex_lock(&lock);
        _tim_ch_free(ch);
    }
#if RTT_NUMOF
    /* else we use some slower running timer, the RTT */
    else if (fticks < TIM_ARCH_RTT_BARRIER) {
        if (rtt_is_set()) {
            val = rtt_get_alarm();

        }

    }
#endif
    /* or we maybe have an RTC? */
#if RTC_NUMOF
    else {

    }
#endif

}

int _tim_get_ch(tim_ch_t *ch)
{
    unsigned state = disableIRQ();
    if (tim_next_ch == NULL) {
        restoreIRQ(state);
        DEBUG("TIM-ERROR: no free timer channel -> wait will fail\n");
        return -1;
    }
    ch = tim_next_ch;
    tim_next_ch = ch->next;
    restoreIRQ(state);
}

void _tim_ch_free(tim_ch_t *ch)
{
    ch->next = NULL;
    unsigned state = disableIRQ();
    if (tim_next_ch == NULL) {
        tim_next_ch = ch;
        tim_last_ch = ch;
    }
    else {
        tim_last_ch->next = ch;
        tim_last_ch = ch;
    }
    restoreIRQ(state);
}

static inline int _tim_to_ticks(tim_time_t *val)
{
    return (val->sec > 0) ? -1 : val->nsec;
}

/* convert between different time bases */
static inline uint32_t _tim_us_to_fticks(uint32_t us)
{
    return us * 1000 / TIM_ARCH_FTICKS;
}

static inline uint32_t _tim_ms_to_fticks(uint32_t ms)
{
    return (ms * 1000 * 1000);
}

static inline uint32_t _tim_us_to_ticks(uint32_t us, uint32_t &fticks)
{
    if (TIM_ARCH_FTICKS == 1000) {      /* should be compiled to constant expression?! */
        *fticks =
    }
    else if (TIM_ARCH_FTICKS < 1000) {
        *fticks = (1000 / TIM_ARCH_FTICKS) * us;
        if (*fticks > TIM_ARCH_FTICK_MAX) {
            *fticks = *fticks & TIM_ARCH_FTICK_MAX;
        }
    }
    else {

    }
}

/* all of these are run in interrupt context! */
void _tim_cb_unlock(void *arg)
{
    tim_ch_t *ch = (tim_ch_t *)arg;

    mutex_unlock(ch->data->lock);
    lifo_insert(tim_lifo, ch->index);
}
