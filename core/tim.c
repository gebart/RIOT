


#include "tim.h"
#include "arch/tim_arch.h"
#include "periph/timer.h"
#include "thread.h"
#include "mutex.h"
#include "msg.h"
#include "kernel_types.h"

#define ENABLE_DEBUG (0)
#include "debug.h"



typedef struct {
    tim_t timer;
    int channel;
} tim_ch_t;

typedef struct {
    int ch;
    tim_time_t val;
    kernel_pid_t target;
    void *arg;
} tim_t;

typedef struct {
    uint32_t nsec;
    uint32_t sec;
} tim_time_t;

static tim_t tim_base_timer;

static tim_ch_t *tim_channels = TIM_ARCH_CHANNELS;
static int tim_lifo[TIM_ARCH_CHANNELS + 1];


int tim_init(void)
{
    /* what type of timers are available on this platform? */
    /* we need at least one TIMER */
    tim_base_timer = TIMER_0;

    timer_init(tim_base_timer, 1);

    /* initialize channel pointer */
    tim_next_ch = &tim_channels;
    tim_last_ch = &(tim_channels[TIM_ARCH_NUMOF_CHANNELS]);
}


int tim_sleep(tim_time_t val)
{
    tim_t timer;
    mutex_t lock = MUTEX_INIT;
    int32_t ticks = _tim_to_ticks(val);

    if (ticks < TIM_ARCH_SPIN_BARRIER) {
        _tim_spin(ticks);
        return;
    }
    else {
        mutex_lock(&lock);
        timer.val = val;
        timer.arg = &lock;
        if (_tim_set(&timer, _tim_cb_unlock) < 0) {
            return -1;
        }
        mutex_lock(&lock);
    }
    return 0;
}


int tim_timeout(tim_t *tim, tim_time_t val, kernel_pid_t pid, void *data)
{
    int32_t ticks = _tim_to_ticks(val);

    if (ticks < TIM_ARCH_SPIN_BARRIER) {
        return -1;
    }

    tim->val = val;
    tim->target = pid;
    tim->arg = data;

    return _tim_set(tim, val, _tim_cb_msg);


    ticks = _tim_us_to_ticks(usec);
    return _tim_set(ticks, _tim_cb_msg);
}

int tim_periodic(uint32_t usec, mst_t *msg)
{
    int32_t ticks = _tim_to_ticks(val);

    if (ticks < TIM_ARCH_SPIN_BARRIER) {
        return -1;
    }

    return _tim_set(ticks, _tim_cb_periodic, (void *)msg);
}

int tim_remove(int channel)
{
    tim_ch_t *ch = tim_channels[channel];
    timer_clear(ch->timer, ch->channel);
}



int tim_core_spin()
{

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
    else if (ticks < TIM_ARCH_RTT_BARRIER) {
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

void _tim_spin(uint32_t ticks)
{
    uint32_t now = timer_read(tim_timer_base);
    while (timer_read(tim_timer_base != (now + ticks)));    /* TODO: will fail if timer_read returns 16bit value */
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

/* all of these are run in interrupt context! */
void _tim_cb_unlock(void *arg)
{
    tim_ch_t *ch = (tim_ch_t *)arg;

    mutex_unlock(ch->data->lock);
    lifo_insert(tim_lifo, ch->index);
}

void _tim_cb_msg(void *arg)
{
    tim_ch_t *ch = (tim_ch_t *)arg;
    mst_t msg;
    msg.type = TIM_MSG_TIMEOUT;
    msg.content.ptr = ch->data->msg->ptr;

    msg_send_int(&msg, ch->data->msg->target);
    lifo_insert(tim_lifo, ch->index);
}



void _tim_cb_periodic(void *arg)
{
    tim_ch_t *ch = (tim_ch_t *)arg;
    mst_t msg;

    timer_set(ch->timer, ch->channel, ch->ticks);

    msg.type = TIM_MSG_TIMEOUT;
    msg.content.ptr = ch->data->msg->ptr;
    msg_send_int(&msg, ch->data->msg->target);
}

