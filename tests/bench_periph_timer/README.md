# Benchmark test for periph_timer

This test is intended to collect statistics about the runtime delays in the
periph_timer implementation. This tool is mainly intended to detect problems in
the low level driver implementation which can be difficult to detect from
higher level systems such as xtimer.

## Test outline

The basic idea of the test is to generate a wide variety of calls to the
periph/timer API, in order to detect problems with the software implementation.
The test consists of a single thread of execution which will set random
timeouts on a periph_timer device. A reference timer is used to measure the
time it takes until the callback is called. The difference between the expected
time and the measured time is computed, and the mean and variance of the
recorded values are calculated. The results are printed as a table on stdout
every 30 seconds. All of the test scenarios used in this application are
based on experience from real world bugs encountered during the development of
RIOT and other systems.

### Avoiding phase lock

The CPU time used for generation and setting of the next timer target in
software is approximately constant across iterations. When the application flow
is driven by the timer under test, via a mutex which is unlocked from the timer
callback, the application will be "phase locked" to the timer under test. This
peculiarity may hide certain implementation race conditions which only occur at
specific phases of the timer ticks. In the test application, this problem is
avoided by inserting random CPU delay loops at key locations:

 - After timer_stop
 - Before timer_set/timer_set_absolute
 - Before timer_start

### API functions tested

Both timer_set and timer_set_absolute calls are mixed in a random order, to
ensure that both functions are working correctly.

### Rescheduling

Some bugs may occur only when the application attempts to replace an already
set timer by calling timer_set again while the timer is running. This is dubbed
"resched" in this application. The application will issue rescheduling timer
calls in the normal operation of the test to catch this class of problems.

### Setting stopped timers

Another class of bugs is the kind where a timer is not behaving correctly if it
was stopped before setting a timer target. The timer should set the new target,
but not begin counting towards the target until timer_start is called. This is
covered by this application under the "stopped" category of test inputs.

### Estimating benchmark CPU overhead

An estimation of the overhead resulting from the CPU processing delays inside
the benchmarking code is performed during benchmark initialization. The
estimation algorithm attempts to perform the exact same actions that the real
benchmark will perform, but without setting any timers. The measured delay is
assumed to originate in the benchmark code and will be subtracted when
computing the difference between expected and actual values. A warning is
printed when the variance of the estimated CPU overhead is too high, this can
be a sign that some other process is running on the CPU and disrupting the
estimation, or a sign of serious problems inside the timer_read function.

## Results

When the test has run for a certain amount of time, the current results will be
presented as a table on stdout. To assist with finding the source of any
discrepancies, the results are split according to three parameters:

 - Function used: timer_set, timer_set_absolute
 - Rescheduled: yes/no
 - Stopped: yes/no

### Interpreting timer_set_xxx statistics

The first part of the results compares the reference time elapsed against the
expected time. A negative value means that the timer alarm is triggered too
soon, compared to the expected time. A positive value means that the timer
alarm is triggered late, compared to the expected time.

#### Expected variance

The variance of the results should generally be as small as possible, except
for the case where the reference timer is running at a higher frequency than
the timer under test. There will always be a truncation error when setting a
timer target because of the discrete timer counts that can be set. The
benchmark will compute limits for the expected variance and show a message if
the variance is less than or greater than the expected variance.

##### Variance too low

A variance lower than expected indicates that there is a phase correlation
between the reference timer and the timer under test. This can only be
explained by a software bug or a hardware failure. The primary suspect will be
the reference timer.

##### Variance too high

A variance greater than expected indicates that there is some extra randomness
in the timeout length, which can be caused by other processes interfering with
the test, a software bug in the timer under test, or hardware failure.

##### Mathematical explanation

Let `X0` be the exact time that a timer_set call occurs, `X0` is a real number.
Let `T` be the desired timeout, `T` is a positive integer because the timer can
only count integer number of ticks. Let `x0 = floor(X0)` be the count that the
timer is currently on when the timer is set, `x0` is an integer. If `X0` is
uniformly random, then the truncation error `X0' = (X0 - x0)` will be a sample
from a continuous uniform distribution in the interval `[0,1)` ticks. The timer
target, `x1`, in the timer under test can be expressed as `x1 = x0 + T`. Let
`k` be a real number that satisfies `Y = kx`, where `x` is an interval in the
unit used by the timer under test, and `Y` is an interval in the unit used by
the reference timer, `Y` is a real number. The time measured by the reference
timer can be expressed as
`Y = k(x1 - X0) = k(x0 + T - X0) = k(X0 - X0' + T - X0) = k(T - X0')`.
`Y` will be a sample from a uniform random distribution in the interval
`[kT, k(T + 1)]`. The variance of `Y` is defined as
`Var(Y) = (k(T + 1) - kT)^2 / 12`.

### Interpreting timer_read statistics

A separate table is displayed for statistics on timer_read. This table compares
the expected timer under test time after a timer has triggered, against the
actual reported value from `timer_read(TIM_TEST_DEV)`. A positive value means
that the TUT time has passed the timer target time. A negative value means
that, according to the timer under test, the alarm target time has not yet been
reached, even though the timer callback has been executed.

### A note on BOARD=native

When running on native, the application will be running as a normal process in
a multi process system which means that most results will have a much greater
variance than expected and the resulting differences will also be much larger
than on a bare metal system. Be careful when drawing conclusions on results
from native.

## Configuration

The timer under test and the reference timer can be chosen at compile time by
defining TIM_TEST_DEV and TIM_REF_DEV, respectively. The frequencies for
these timers can be configured through TIM_DEV_FREQ and TIM_REF_FREQ. For
example, to compare timer device 2 running at 32768 Hz against a reference timer
on timer device 0 (default) running at 1 MHz (default) on Mulle:

    CFLAGS='-DTIM_TEST_DEV=TIMER_DEV\(2\) -DTIM_TEST_FREQ=32768' BOARD=mulle make flash

### Default configuration

reference timer: TIMER_DEV(0)
timer under test: TIMER_DEV(1) if TIMER_NUMOF > 1, TIMER_DEV(0) otherwise.
timer frequency: 1 MHz

Define USE_REFERENCE=0 to compare a timer against itself. Be aware that this
may hide a class of errors where the timer is losing ticks in certain
situations, which a separate reference timer would be able to detect.

## Expected result

It is difficult to set a general expected result which applies to all
platforms. However, the mean and variance of the differences should be small,
in relation to the tick duration of the timer under test. What is "small" also
depends the CPU core frequency, because of CPU processing time spent in the
driver code and in the test code. For example, when testing a 1 MHz timer and
comparing with another 1 MHz timer, the mean difference should likely be in
single digits on a Cortex-M CPU. Testing a 32768 Hz timer and referencing a 1
MHz timer on the other hand should have a mean difference in double digits, the
variance will also be greater because of the quantization errors and rounding
in the tick conversion routine. If the mean or variance of the difference is
exceptionally large, the row will be marked with "SIC!" to draw attention to
the fact.

### Rule of thumb

Below are some rules of thumb that can be useful when looking at the results.
These are only guidelines for when debugging timer drivers, the real world
requirements vary from application to application. The CPU core clock speed will
also affect the measured mean difference because of the execution time of the
code between the timer interrupt being triggered and when the benchmark
application has read out both timer values. The variance should not be affected
by the CPU speed because the processing time between the ISR and the timer_read
should be constant across all iterations.

1 MHz timer tested with 1 MHz reference:

abs(mean) < 20
variance < 10

32768 Hz timer tested with 1 MHz reference:

abs(mean) < 100
variance < 100

32768 Hz timer tested with 32768 Hz reference:

abs(mean) < 4
variance < 10

1 MHz timer tested with 32768 Hz reference:

abs(mean) < 2
variance < 10

### Example output

Below is a short sample of a test of a 32768 Hz timer with a 1 MHz reference:

    ------------- BEGIN STATISTICS --------------
    Target error (actual trigger time - expected trigger time)
    positive: timer is late, negative: timer is early
    === timer_set running ===
       interval    count       sum       sum_sq    min   max  mean  variance
       1 -    2:     502     28976        38942     43    74    57     77
       3 -    4:     423     24826        31372     43    74    58     74
       5 -    8:     887     52075        65411     43    74    58     73
       9 -   16:    1809    105768       136714     43    74    58     75
      17 -   32:    3671    214870       281126     42    74    58     76
          TOTAL     7292    426515       554067     42    74    58     75
...

The statistics above show that the timer implementation introduces a small
delay on all timers. Note however that it is not clear whether this delay is in
timer_set, timer_read, or within the testing code, but the combined effect of
all of those error sources make the timer overshoot its target by on average 58
microseconds. There is also the expected difference of approximately 31 µs
between the minimum and the maximum values, this is entirely expected because
one 32768 Hz tick equals 1/32768 s = 30.51 µs.

Below is a short sample of a test where there is something wrong with the timer
implementation, again a 32768 Hz timer tested with a 1 MHz reference:

    === timer_set_absolute running ===
       interval    count       sum       sum_sq    min   max  mean  variance
       1 -    2:     706    393603     59023835     44  1075   557  83717  <=== SIC!
       3 -    4:     644    358001     56925795     43  1074   555  88495  <=== SIC!
       5 -    8:    1402    783661    121240925     45  1074   558  86534  <=== SIC!
       9 -   16:    2762   1515993    243789723     44  1073   548  88288  <=== SIC!
      17 -   32:    5476   2981671    490016153     43  1075   544  89500  <=== SIC!
          TOTAL    10990   6032929    971284075     43  1075   548  88386  <=== SIC!

We can see that the variance, the maximum error, and the mean are very large.
This particular timer implementation needs some work on its timer_set_absolute
implementation.

## Configuration details

Configuration macros used by the application are described below

### Settings related to timer hardware

#### TIM_TEST_DEV

Timer under test. Default: `TIMER_DEV(1)`

#### TIM_REF_DEV

Reference timer used for measuring the callback time error. Default: `TIMER_DEV(0)`

#### TIM_TEST_FREQ

Frequency of the timer under test, used when calling timer_init during the
application startup. Default: `1000000`, 1 MHz

#### TIM_REF_FREQ

Frequency of the reference timer, used when calling timer_init during the
application startup. Default: `1000000`, 1 MHz

#### TIM_TEST_CHAN

Timer channel used on the timer under test. Default: `0`

#### USE_REFERENCE

Shorthand for setting the reference timer to the same as the timer under test.
Default: `0`

### Settings related to result processing


#### DETAILED_STATS

Keep statistics per timer target offset value if set to 1, otherwise keep
statistics only per scenario. Default: `1`

#### LOG2_STATS

Only used when `DETAILED_STATS == 1`. Statistics are grouped according to
2-logarithms of the timer offset value, e.g. 1-2, 3-4, 5-8, 9-16 etc. This
reduces memory consumption and creates a shorter result table for easier
overview. Default: `1`

#### TEST_PRINT_INTERVAL_TICKS

The result table will be printed to standard output when this many reference
timer ticks have passed since the last printout. Default: `((TIM_REF_FREQ) * 30)`

### Settings related to timer input generation

#### TEST_MIN

Minimum timer offset tested for `timer_set_absolute`, in timer under test
ticks. Default: `1` for timers < 1 MHz, `16` otherwise, to prevent setting a
timer target in the past

#### TEST_MIN_REL

Minimum timer offset tested for `timer_set`, in timer under test ticks.
Default: `0`

#### TEST_MAX

Maximum timer offset tested, in timer under test ticks. Default: `128`

#### SPIN_MAX_TARGET

The CPU busy wait loop will be calibrated during application start up so that
the maximum random spin length is approximately equal to this many timer under
test ticks. Default: `16`

#### TEST_UNEXPECTED_STDDEV

Mark the output row if the computed variance implies a standard deviation which
is greater than this value. This value will be automatically recomputed to
compensate for the variance resulting from the truncation in the tick
conversion if the reference timer is running at a higher frequency than the
timer under test. Default: `4`

#### TEST_UNEXPECTED_MEAN

Mark the output row if the computed mean absolute difference is greater than this
number of timer under test ticks. This value will be automatically recomputed
to compensate for the error resulting from the truncation in the tick
conversion if the reference timer is running at a higher frequency than the
timer under test. Default: `2`
