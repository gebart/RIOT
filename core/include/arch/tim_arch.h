

#ifndef __TIM_ARCH_H
#define __TIM_ARCH_H

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

#ifndef TIM_ARCH_TIMER_BARRIER
#warning "TIM_ARCH_TIMER_BARRIER not defined for your platform, using default value 65000"
#define TIM_ARCH_TIMER_BARRIER              (65000U)
#endif

#ifndef TIM_ARCH_CHANNELS
#error "TIM_ARCH_CHANNELS not defined! You have to initialize the CHANNELS for your platform"
#endif

/**
 * @brief the number of overall available hardware timer channels
 *
 * This number is a sum of the number of hardware timers times the number of
 * channels on each hardware timer.
 */
#ifndef TIM_ARCH_NUMOF_CHANNELS
#warning "TIM_ARCH_NUMOF_CHANNELS not defined for your platform, using default value: 8"
#define TIM_ARCH_NUMOF_CHANNELS             (8)
#endif

#ifndef TIM_ARCH_IRQ_DELAY
#warning "TIM_ARCH_IRQ_DELAY not defined for your platform, using default value: 500"
#define TIM_ARCH_IRQ_DELAY                  (500U)
#endif

#ifndef TIM_ARCH_FTICKS
#error "TIM_ARCH_FTICKS not defined for your platform!"
#endif

#ifndef TIM_ARCH_FTICK_MAX
#error "TIM_ARCH_FTICK_MAX not defined for your platform!"
#endif

#ifdef __cplusplus
}
#endif

#endif /* __TIM_ARCH_H */
/** @} */
