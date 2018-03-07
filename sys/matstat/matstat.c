/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include "matstat.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

void matstat_clear(matstat_state_t *state)
{
    *state = MATSTAT_STATE_INIT;
}

void matstat_add(matstat_state_t *state, int32_t value)
{
    if (value > state->max) {
        state->max = value;
    }
    if (value < state->min) {
        state->min = value;
    }
    state->sum += value;
    if (state->count == 0) {
        state->offset = value;
    }
    /* Subtracting this offset yields better numeric stability in the variance
     * algorithm */
    value -= state->offset;
    /* Assuming the differences will be small on average and the number of
     * samples is reasonably limited, to prevent overflow in sum_sq */
    state->sum_sq += (int64_t)value * value;
    ++state->count;
    /* Adapt the offset with every power of two number of samples, i.e. after 2 samples,
     * 4, 8, 16, 32, 64 samples, and so on. */
    if (state->count && !(state->count & (state->count - 1))) {
        /* count is a power of two (exactly one bit is set in the integer) */
        int32_t mean = matstat_mean(state);
        /* Adjust offset */
        matstat_change_offset(state, mean, mean);
    }
}

int32_t matstat_mean(const matstat_state_t *state)
{
    if (state->count == 0) {
        /* We don't have any way of returning an error */
        return 0;
    }
    int32_t mean = state->sum / state->count;
    DEBUG("mean: %" PRId64 " / %" PRIu32 " = %" PRId32 "\n",
        state->sum, state->count, mean);
    return mean;
}

uint64_t matstat_variance(const matstat_state_t *state, int32_t mean)
{
    if (state->count < 2) {
        /* We don't have any way of returning an error */
        return 0;
    }
    mean -= state->offset;
    uint64_t variance = 0;
    uint64_t tmp = state->count * mean * mean;
    /* For certain input vectors, where the variance is small (less than 1),
     * the truncation errors will accumulate and cause this equation to return a
     * negative variance */
    /* Work around: return 0 if the computed variance is < 1 */
    if (tmp < state->sum_sq) {
        variance = (state->sum_sq - tmp) / (state->count - 1);
    }
    DEBUG("Var: (%" PRIu64 " - %" PRId32 " * %" PRId32 " * %" PRId32 ") / (%" PRIu32 " - 1) = %" PRIu64 "\n",
        state->sum_sq, state->count, mean, mean, state->count, variance);
    return variance;
}

void matstat_change_offset(matstat_state_t *state, int32_t mean, int32_t new_offset)
{
    int32_t new_mean = mean - new_offset;
    mean -= state->offset;
    /* Adjust sum_sq so that the variance is the same before and after the offset change */
    int64_t adjustment = state->count * ((int64_t)new_mean * new_mean - (int64_t)mean * mean);
    /* The conditional block below is used to avoid a corner case where the
     * truncation of the result of the mean computation (integer division)
     * causes sum_sq to become negative */
    if ((adjustment < 0) && (((uint64_t)-adjustment) > state->sum_sq)) {
        /* Avoid negative sum_sq by picking a slightly different offset */
        DEBUG("adj1: %" PRId64 ", sum_sq = %" PRIu64 ", "
            "new_offset = %" PRId32 ", old_offset = %" PRId32 "\n",
            adjustment, state->sum_sq, new_offset, state->offset);
        if (new_offset > state->offset) {
            ++new_mean;
            --new_offset;
        }
        else {
            --new_mean;
            ++new_offset;
        }
        adjustment = state->count * ((int64_t)new_mean * new_mean - (int64_t)mean * mean);
        DEBUG("adj2: %" PRId64 ", sum_sq = %" PRIu64 ", "
            "new_offset = %" PRId32 ", old_offset = %" PRId32 "\n",
            adjustment, state->sum_sq, new_offset, state->offset);
    }
    state->sum_sq += adjustment;
    state->offset = new_offset;
}

void matstat_merge(matstat_state_t *dest, const matstat_state_t *src)
{
    if (src->count == 0) {
        return;
    }
    else if (dest->count == 0) {
        *dest = *src;
        return;
    }
    else if (dest->offset != src->offset) {
        /* Adjust offset to match before merging, or the variance algorithm will
         * be messed up */
        int32_t mean = matstat_mean(dest);
        matstat_change_offset(dest, mean, src->offset);
    }
    dest->count += src->count;
    dest->sum += src->sum;
    dest->sum_sq += src->sum_sq;
    if (src->max > dest->max) {
        dest->max = src->max;
    }
    if (src->min < dest->min) {
        dest->min = src->min;
    }
    /* rebalance the offset */
    int32_t mean = matstat_mean(dest);
    matstat_change_offset(dest, mean, mean);
}
