


/**
 * @name TIM fast timer configuration
 * @{
 */
#define TIM_ARCH_TIMER_MAP      {TIMER_0, TIMER_0, TIMER_0}
#define TIM_ARCH_CHANNEL_MAP    {0, 1, 2}
#define TIM_ARCH_CHANNELS       (3)
/** @} */

/**
 * @name TIM slow timer configuration
 * @{
 */
#define TIM_ARCH_STIMER_RTT     1               /**< set 1 to use RTT for STIMER */
#define TIM_ARCH_STIMER         TIMER_x         /**< timer to use, ignored when RTT==1 */
/** @} */

/**
 * @name Configure fast and slow timer clock rates and ranges
 * @{
 */
#define TIM_ARCH_FCLK           TIMER_0_FREQUENCY
#define TIM_ARCH_FMAX           TIMER_0_MAX_VALUE
#define TIM_ARCH_SCLK           RTT_FREQUENCY
#define TIM_ARCH_SMAX           RTT_MAX_VALUE
/** @} */

/**
 * @brief Define barriers
 *
 * TODO: measure and define these in relation to F_CPU and TIM_ARCH_FCLK
 * @{
 */
#define TIM_ARCH_IRQ_DELAY          (55U)
#define TIM_ARCH_SPIN_BARRIER       (20U)
/** @} */
