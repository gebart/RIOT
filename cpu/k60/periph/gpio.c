/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_k60
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/gpio.h"
#include "periph_conf.h"

/* guard file in case no GPIO devices are defined */
#if GPIO_NUMOF

typedef struct {
    gpio_cb_t cb;       /**< callback to call on GPIO interrupt */
    void *arg;          /**< argument passed to the callback */
    uint32_t irqc;      /**< remember interrupt configuration between dis/enable */
} gpio_state_t;

/**
 * @brief Unified IRQ handler shared by all interrupt routines
 *
 * @param[in] dev   the device that triggered the interrupt
 */
static inline void irq_handler(gpio_t dev);

/**
 * @brief Hold one callback function pointer for each gpio device
 */
static gpio_state_t gpio_config[GPIO_NUMOF];

int gpio_init_out(gpio_t dev, gpio_pp_t pullup)
{
    PORT_Type *port = 0;
    GPIO_Type *gpio = 0;
    uint32_t pin = 0;

    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            GPIO_0_CLKEN();
            port = GPIO_0_PORT;
            gpio = GPIO_0_GPIO;
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            GPIO_1_CLKEN();
            port = GPIO_1_PORT;
            gpio = GPIO_1_GPIO;
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            GPIO_2_CLKEN();
            port = GPIO_2_PORT;
            gpio = GPIO_2_GPIO;
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            GPIO_3_CLKEN();
            port = GPIO_3_PORT;
            gpio = GPIO_3_GPIO;
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            GPIO_4_CLKEN();
            port = GPIO_4_PORT;
            gpio = GPIO_4_GPIO;
            pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            GPIO_5_CLKEN();
            port = GPIO_5_PORT;
            gpio = GPIO_5_GPIO;
            pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            GPIO_6_CLKEN();
            port = GPIO_6_PORT;
            gpio = GPIO_6_GPIO;
            pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            GPIO_7_CLKEN();
            port = GPIO_7_PORT;
            gpio = GPIO_7_GPIO;
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            GPIO_8_CLKEN();
            port = GPIO_8_PORT;
            gpio = GPIO_8_GPIO;
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            GPIO_9_CLKEN();
            port = GPIO_9_PORT;
            gpio = GPIO_9_GPIO;
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            GPIO_10_CLKEN();
            port = GPIO_10_PORT;
            gpio = GPIO_10_GPIO;
            pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            GPIO_11_CLKEN();
            port = GPIO_11_PORT;
            gpio = GPIO_11_GPIO;
            pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            GPIO_12_CLKEN();
            port = GPIO_12_PORT;
            gpio = GPIO_12_GPIO;
            pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            GPIO_13_CLKEN();
            port = GPIO_13_PORT;
            gpio = GPIO_13_GPIO;
            pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            GPIO_14_CLKEN();
            port = GPIO_14_PORT;
            gpio = GPIO_14_GPIO;
            pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            GPIO_15_CLKEN();
            port = GPIO_15_PORT;
            gpio = GPIO_15_GPIO;
            pin = GPIO_15_PIN;
            break;
#endif
        default:
            return -1;
    }
    /* Clear config */
    gpio_config[dev].cb = 0;
    gpio_config[dev].arg = 0;
    gpio_config[dev].irqc = 0;

    /* Reset all pin control settings for the pin */
    /* Switch to analog input function while fiddling with the settings, to be safe. */
    port->PCR[pin] = PORT_PCR_MUX(PIN_MUX_FUNCTION_ANALOG);

    /* The reference manual states that the pull enable bit only affects digital
     * inputs, I have not tested this WRT open drain outputs. We configure the
     * pin as an open drain output anyway if a pull resistor is specified. */
    switch (pullup) {
        case GPIO_NOPULL:
            port->PCR[pin] &= ~(PORT_PCR_PE_MASK | PORT_PCR_ODE_MASK); /* Pull disable, open drain disable */
            break;
        case GPIO_PULLUP:
            port->PCR[pin] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_ODE_MASK; /* Pull enable, pull up, open drain enable */
            break;
        case GPIO_PULLDOWN:
            port->PCR[pin] &= ~(PORT_PCR_PS_MASK); /* Pull down */
            port->PCR[pin] |= PORT_PCR_PE_MASK | PORT_PCR_ODE_MASK; /* Pull enable, open drain enable */
            break;
        default:
            return -1;
    }
    gpio->PDDR |= GPIO_PDDR_PDD(1 << pin);     /* set pin to output mode */
    gpio->PCOR |= GPIO_PCOR_PTCO(1 << pin);    /* set output to low */
    /* Select GPIO function for the pin */
    port->PCR[pin] |= PORT_PCR_MUX(PIN_MUX_FUNCTION_GPIO);

    return 0; /* all OK */
}

int gpio_init_in(gpio_t dev, gpio_pp_t pullup)
{
    PORT_Type *port = 0;
    GPIO_Type *gpio = 0;
    uint32_t pin = 0;

    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            GPIO_0_CLKEN();
            port = GPIO_0_PORT;
            gpio = GPIO_0_GPIO;
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            GPIO_1_CLKEN();
            port = GPIO_1_PORT;
            gpio = GPIO_1_GPIO;
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            GPIO_2_CLKEN();
            port = GPIO_2_PORT;
            gpio = GPIO_2_GPIO;
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            GPIO_3_CLKEN();
            port = GPIO_3_PORT;
            gpio = GPIO_3_GPIO;
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            GPIO_4_CLKEN();
            port = GPIO_4_PORT;
            gpio = GPIO_4_GPIO;
            pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            GPIO_5_CLKEN();
            port = GPIO_5_PORT;
            gpio = GPIO_5_GPIO;
            pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            GPIO_6_CLKEN();
            port = GPIO_6_PORT;
            gpio = GPIO_6_GPIO;
            pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            GPIO_7_CLKEN();
            port = GPIO_7_PORT;
            gpio = GPIO_7_GPIO;
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            GPIO_8_CLKEN();
            port = GPIO_8_PORT;
            gpio = GPIO_8_GPIO;
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            GPIO_9_CLKEN();
            port = GPIO_9_PORT;
            gpio = GPIO_9_GPIO;
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            GPIO_10_CLKEN();
            port = GPIO_10_PORT;
            gpio = GPIO_10_GPIO;
            pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            GPIO_11_CLKEN();
            port = GPIO_11_PORT;
            gpio = GPIO_11_GPIO;
            pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            GPIO_12_CLKEN();
            port = GPIO_12_PORT;
            gpio = GPIO_12_GPIO;
            pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            GPIO_13_CLKEN();
            port = GPIO_13_PORT;
            gpio = GPIO_13_GPIO;
            pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            GPIO_14_CLKEN();
            port = GPIO_14_PORT;
            gpio = GPIO_14_GPIO;
            pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            GPIO_15_CLKEN();
            port = GPIO_15_PORT;
            gpio = GPIO_15_GPIO;
            pin = GPIO_15_PIN;
            break;
#endif
        default:
            return -1;
    }

    /* Reset all pin control settings for the pin */
    /* Switch to analog input function while fiddling with the settings, to be safe. */
    port->PCR[pin] = PORT_PCR_MUX(PIN_MUX_FUNCTION_ANALOG);

    switch (pullup) {
        case GPIO_NOPULL:
            port->PCR[pin] &= ~(PORT_PCR_PE_MASK); /* Pull disable */
            break;
        case GPIO_PULLUP:
            port->PCR[pin] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; /* Pull enable, pull up */
            break;
        case GPIO_PULLDOWN:
            port->PCR[pin] &= ~(PORT_PCR_PS_MASK); /* Pull down */
            port->PCR[pin] |= PORT_PCR_PE_MASK; /* Pull enable */
            break;
        default:
            return -1;
    }

    gpio->PDDR &= ~(GPIO_PDDR_PDD(1 << pin));     /* set pin to input mode */
    /* Select GPIO function for the pin */
    port->PCR[pin] |= PORT_PCR_MUX(PIN_MUX_FUNCTION_GPIO);

    return 0; /* everything alright here */
}

int gpio_init_int(gpio_t dev, gpio_pp_t pullup, gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    int res;
    PORT_Type *port = 0;
    uint32_t pin = 0;

    res = gpio_init_in(dev, pullup);
    if (res < 0) {
        return res;
    }

    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            pin = GPIO_0_PIN;
            port = GPIO_0_PORT;
            NVIC_SetPriority(GPIO_0_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_0_IRQ);
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            pin = GPIO_1_PIN;
            port = GPIO_1_PORT;
            NVIC_SetPriority(GPIO_1_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_1_IRQ);
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            pin = GPIO_2_PIN;
            port = GPIO_2_PORT;
            NVIC_SetPriority(GPIO_2_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_2_IRQ);
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            pin = GPIO_3_PIN;
            port = GPIO_3_PORT;
            NVIC_SetPriority(GPIO_3_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_3_IRQ);
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            pin = GPIO_4_PIN;
            port = GPIO_4_PORT;
            NVIC_SetPriority(GPIO_4_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_4_IRQ);
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            pin = GPIO_5_PIN;
            port = GPIO_5_PORT;
            NVIC_SetPriority(GPIO_5_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_5_IRQ);
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            pin = GPIO_6_PIN;
            port = GPIO_6_PORT;
            NVIC_SetPriority(GPIO_6_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_6_IRQ);
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            pin = GPIO_7_PIN;
            port = GPIO_7_PORT;
            NVIC_SetPriority(GPIO_7_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_7_IRQ);
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            pin = GPIO_8_PIN;
            port = GPIO_8_PORT;
            NVIC_SetPriority(GPIO_8_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_8_IRQ);
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            pin = GPIO_9_PIN;
            port = GPIO_9_PORT;
            NVIC_SetPriority(GPIO_9_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_9_IRQ);
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            pin = GPIO_10_PIN;
            port = GPIO_10_PORT;
            NVIC_SetPriority(GPIO_10_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_10_IRQ);
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            pin = GPIO_11_PIN;
            port = GPIO_11_PORT;
            NVIC_SetPriority(GPIO_11_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_11_IRQ);
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            pin = GPIO_12_PIN;
            port = GPIO_12_PORT;
            NVIC_SetPriority(GPIO_12_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_12_IRQ);
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            pin = GPIO_13_PIN;
            port = GPIO_13_PORT;
            NVIC_SetPriority(GPIO_13_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_13_IRQ);
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            pin = GPIO_14_PIN;
            port = GPIO_14_PORT;
            NVIC_SetPriority(GPIO_14_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_14_IRQ);
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            pin = GPIO_15_PIN;
            port = GPIO_15_PORT;
            NVIC_SetPriority(GPIO_15_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_15_IRQ);
            break;
#endif
        default:
            return -1;
    }

    /* set callback */
    gpio_config[dev].cb = cb;
    gpio_config[dev].arg = arg;

    /* configure the active edges */
    switch (flank) {
        case GPIO_RISING:
            port->PCR[pin] &= ~(PORT_PCR_IRQC_MASK); /* Disable interrupt */
            port->PCR[pin] |= PORT_PCR_ISF_MASK; /* Clear interrupt flag */
            port->PCR[pin] |= PORT_PCR_IRQC(PIN_INTERRUPT_RISING); /* Enable interrupt */
            gpio_config[dev].irqc = PORT_PCR_IRQC(PIN_INTERRUPT_RISING);
            break;
        case GPIO_FALLING:
            port->PCR[pin] &= ~(PORT_PCR_IRQC_MASK); /* Disable interrupt */
            port->PCR[pin] |= PORT_PCR_ISF_MASK; /* Clear interrupt flag */
            port->PCR[pin] |= PORT_PCR_IRQC(PIN_INTERRUPT_FALLING); /* Enable interrupt */
            gpio_config[dev].irqc = PORT_PCR_IRQC(PIN_INTERRUPT_FALLING);
            break;
        case GPIO_BOTH:
            port->PCR[pin] &= ~(PORT_PCR_IRQC_MASK); /* Disable interrupt */
            port->PCR[pin] |= PORT_PCR_ISF_MASK; /* Clear interrupt flag */
            port->PCR[pin] |= PORT_PCR_IRQC(PIN_INTERRUPT_EDGE); /* Enable interrupt */
            gpio_config[dev].irqc = PORT_PCR_IRQC(PIN_INTERRUPT_EDGE);
            break;
        default:
            return -1;
    }

    return 0;
}

void gpio_irq_enable(gpio_t dev)
{
    PORT_Type *port = 0;
    uint32_t pin = 0;

    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            port = GPIO_0_PORT;
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            port = GPIO_1_PORT;
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            port = GPIO_2_PORT;
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            port = GPIO_3_PORT;
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            port = GPIO_4_PORT;
            pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            port = GPIO_5_PORT;
            pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            port = GPIO_6_PORT;
            pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            port = GPIO_7_PORT;
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            port = GPIO_8_PORT;
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            port = GPIO_9_PORT;
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            port = GPIO_10_PORT;
            pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            port = GPIO_11_PORT;
            pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            port = GPIO_12_PORT;
            pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            port = GPIO_13_PORT;
            pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            port = GPIO_14_PORT;
            pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            port = GPIO_15_PORT;
            pin = GPIO_15_PIN;
            break;
#endif
        default:
            return;
    }
    /* Restore saved state */
    port->PCR[pin] &= ~(PORT_PCR_IRQC_MASK);
    port->PCR[pin] |= PORT_PCR_IRQC_MASK & gpio_config[dev].irqc;
}

void gpio_irq_disable(gpio_t dev)
{
    PORT_Type *port = 0;
    uint32_t pin = 0;

    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            port = GPIO_0_PORT;
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            port = GPIO_1_PORT;
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            port = GPIO_2_PORT;
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            port = GPIO_3_PORT;
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            port = GPIO_4_PORT;
            pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            port = GPIO_5_PORT;
            pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            port = GPIO_6_PORT;
            pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            port = GPIO_7_PORT;
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            port = GPIO_8_PORT;
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            port = GPIO_9_PORT;
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            port = GPIO_10_PORT;
            pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            port = GPIO_11_PORT;
            pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            port = GPIO_12_PORT;
            pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            port = GPIO_13_PORT;
            pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            port = GPIO_14_PORT;
            pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            port = GPIO_15_PORT;
            pin = GPIO_15_PIN;
            break;
#endif
        default:
            return;
    }
    /* Save irqc state before disabling to allow enabling with the same trigger settings later. */
    gpio_config[dev].irqc = PORT_PCR_IRQC_MASK & port->PCR[pin];
    port->PCR[pin] &= ~(PORT_PCR_IRQC_MASK);
}

int gpio_read(gpio_t dev)
{
    GPIO_Type *gpio = 0;
    uint32_t pin = 0;

    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            gpio = GPIO_0_GPIO;
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            gpio = GPIO_1_GPIO;
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            gpio = GPIO_2_GPIO;
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            gpio = GPIO_3_GPIO;
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            gpio = GPIO_4_GPIO;
            pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            gpio = GPIO_5_GPIO;
            pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            gpio = GPIO_6_GPIO;
            pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            gpio = GPIO_7_GPIO;
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            gpio = GPIO_8_GPIO;
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            gpio = GPIO_9_GPIO;
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            gpio = GPIO_10_GPIO;
            pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            gpio = GPIO_11_GPIO;
            pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            gpio = GPIO_12_GPIO;
            pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            gpio = GPIO_13_GPIO;
            pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            gpio = GPIO_14_GPIO;
            pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            gpio = GPIO_15_GPIO;
            pin = GPIO_15_PIN;
            break;
#endif
        default:
            return -1;
    }

    if (gpio->PDDR & GPIO_PDDR_PDD(1 << pin)) { /* if configured as output */
        return gpio->PDOR & GPIO_PDOR_PDO(1 << pin); /* read output data register */
    } else {
        return gpio->PDIR & GPIO_PDIR_PDI(1 << pin); /* else read input data register */
    }
}

void gpio_set(gpio_t dev)
{
    GPIO_Type *gpio = 0;
    uint32_t pin = 0;

    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            gpio = GPIO_0_GPIO;
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            gpio = GPIO_1_GPIO;
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            gpio = GPIO_2_GPIO;
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            gpio = GPIO_3_GPIO;
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            gpio = GPIO_4_GPIO;
            pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            gpio = GPIO_5_GPIO;
            pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            gpio = GPIO_6_GPIO;
            pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            gpio = GPIO_7_GPIO;
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            gpio = GPIO_8_GPIO;
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            gpio = GPIO_9_GPIO;
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            gpio = GPIO_10_GPIO;
            pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            gpio = GPIO_11_GPIO;
            pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            gpio = GPIO_12_GPIO;
            pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            gpio = GPIO_13_GPIO;
            pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            gpio = GPIO_14_GPIO;
            pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            gpio = GPIO_15_GPIO;
            pin = GPIO_15_PIN;
            break;
#endif
        default:
            return;
    }
    gpio->PSOR = GPIO_PSOR_PTSO(1 << pin);
}

void gpio_clear(gpio_t dev)
{
    GPIO_Type *gpio = 0;
    uint32_t pin = 0;

    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            gpio = GPIO_0_GPIO;
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            gpio = GPIO_1_GPIO;
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            gpio = GPIO_2_GPIO;
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            gpio = GPIO_3_GPIO;
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            gpio = GPIO_4_GPIO;
            pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            gpio = GPIO_5_GPIO;
            pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            gpio = GPIO_6_GPIO;
            pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            gpio = GPIO_7_GPIO;
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            gpio = GPIO_8_GPIO;
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            gpio = GPIO_9_GPIO;
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            gpio = GPIO_10_GPIO;
            pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            gpio = GPIO_11_GPIO;
            pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            gpio = GPIO_12_GPIO;
            pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            gpio = GPIO_13_GPIO;
            pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            gpio = GPIO_14_GPIO;
            pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            gpio = GPIO_15_GPIO;
            pin = GPIO_15_PIN;
            break;
#endif
        default:
            return;
    }
    gpio->PCOR = GPIO_PCOR_PTCO(1 << pin);
}

void gpio_toggle(gpio_t dev)
{
    GPIO_Type *gpio = 0;
    uint32_t pin = 0;

    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            gpio = GPIO_0_GPIO;
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            gpio = GPIO_1_GPIO;
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            gpio = GPIO_2_GPIO;
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            gpio = GPIO_3_GPIO;
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            gpio = GPIO_4_GPIO;
            pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            gpio = GPIO_5_GPIO;
            pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            gpio = GPIO_6_GPIO;
            pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            gpio = GPIO_7_GPIO;
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            gpio = GPIO_8_GPIO;
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            gpio = GPIO_9_GPIO;
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            gpio = GPIO_10_GPIO;
            pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            gpio = GPIO_11_GPIO;
            pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            gpio = GPIO_12_GPIO;
            pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            gpio = GPIO_13_GPIO;
            pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            gpio = GPIO_14_GPIO;
            pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            gpio = GPIO_15_GPIO;
            pin = GPIO_15_PIN;
            break;
#endif
        default:
            return;
    }
    gpio->PTOR = GPIO_PTOR_PTTO(1 << pin);
}

void gpio_write(gpio_t dev, int value)
{
    GPIO_Type *gpio = 0;
    uint32_t pin = 0;

    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            gpio = GPIO_0_GPIO;
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            gpio = GPIO_1_GPIO;
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            gpio = GPIO_2_GPIO;
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            gpio = GPIO_3_GPIO;
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            gpio = GPIO_4_GPIO;
            pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            gpio = GPIO_5_GPIO;
            pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            gpio = GPIO_6_GPIO;
            pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            gpio = GPIO_7_GPIO;
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            gpio = GPIO_8_GPIO;
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            gpio = GPIO_9_GPIO;
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            gpio = GPIO_10_GPIO;
            pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            gpio = GPIO_11_GPIO;
            pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            gpio = GPIO_12_GPIO;
            pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            gpio = GPIO_13_GPIO;
            pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            gpio = GPIO_14_GPIO;
            pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            gpio = GPIO_15_GPIO;
            pin = GPIO_15_PIN;
            break;
#endif
        default:
            return;
    }
    gpio->PDOR = GPIO_PDOR_PDO(1 << pin);
}

static inline void irq_handler(gpio_t dev)
{
    gpio_config[dev].cb(gpio_config[dev].arg);
    if (sched_context_switch_request) {
        thread_yield();
    }
}

/* the following interrupt handlers are quite ugly, the preprocessor is used to
 * insert only the relevant checks in each isr, however, in the source all of
 * the ISRs contain all GPIO checks...
 */
/** @todo Template GPIO ISRs */

__attribute__((naked))
void _isr_porta_pin_detect(void)
{
    ISR_ENTER();
#if GPIO_0_EN && (GPIO_0_ISR == _isr_porta_pin_detect)
    if (PORTA->ISFR & PORT_ISFR_ISF(1 << GPIO_0_PIN)) {
        irq_handler(GPIO_0);
        /* clear status bit by writing a 1 to it */
        PORTA->ISFR = PORT_ISFR_ISF(1 << GPIO_0_PIN);
    }
#endif
#if GPIO_1_EN && (GPIO_1_ISR == _isr_porta_pin_detect)
    if (PORTA->ISFR & PORT_ISFR_ISF(1 << GPIO_1_PIN)) {
        irq_handler(GPIO_1);
        /* clear status bit by writing a 1 to it */
        PORTA->ISFR = PORT_ISFR_ISF(1 << GPIO_1_PIN);
    }
#endif
#if GPIO_2_EN && (GPIO_2_ISR == _isr_porta_pin_detect)
    if (PORTA->ISFR & PORT_ISFR_ISF(1 << GPIO_2_PIN)) {
        irq_handler(GPIO_2);
        /* clear status bit by writing a 1 to it */
        PORTA->ISFR = PORT_ISFR_ISF(1 << GPIO_2_PIN);
    }
#endif
#if GPIO_3_EN && (GPIO_3_ISR == _isr_porta_pin_detect)
    if (PORTA->ISFR & PORT_ISFR_ISF(1 << GPIO_3_PIN)) {
        irq_handler(GPIO_3);
        /* clear status bit by writing a 1 to it */
        PORTA->ISFR = PORT_ISFR_ISF(1 << GPIO_3_PIN);
    }
#endif
#if GPIO_4_EN && (GPIO_4_ISR == _isr_porta_pin_detect)
    if (PORTA->ISFR & PORT_ISFR_ISF(1 << GPIO_4_PIN)) {
        irq_handler(GPIO_4);
        /* clear status bit by writing a 1 to it */
        PORTA->ISFR = PORT_ISFR_ISF(1 << GPIO_4_PIN);
    }
#endif
#if GPIO_5_EN && (GPIO_5_ISR == _isr_porta_pin_detect)
    if (PORTA->ISFR & PORT_ISFR_ISF(1 << GPIO_5_PIN)) {
        irq_handler(GPIO_5);
        /* clear status bit by writing a 1 to it */
        PORTA->ISFR = PORT_ISFR_ISF(1 << GPIO_5_PIN);
    }
#endif
#if GPIO_6_EN && (GPIO_6_ISR == _isr_porta_pin_detect)
    if (PORTA->ISFR & PORT_ISFR_ISF(1 << GPIO_6_PIN)) {
        irq_handler(GPIO_6);
        /* clear status bit by writing a 1 to it */
        PORTA->ISFR = PORT_ISFR_ISF(1 << GPIO_6_PIN);
    }
#endif
#if GPIO_7_EN && (GPIO_7_ISR == _isr_porta_pin_detect)
    if (PORTA->ISFR & PORT_ISFR_ISF(1 << GPIO_7_PIN)) {
        irq_handler(GPIO_7);
        /* clear status bit by writing a 1 to it */
        PORTA->ISFR = PORT_ISFR_ISF(1 << GPIO_7_PIN);
    }
#endif
#if GPIO_8_EN && (GPIO_8_ISR == _isr_porta_pin_detect)
    if (PORTA->ISFR & PORT_ISFR_ISF(1 << GPIO_8_PIN)) {
        irq_handler(GPIO_8);
        /* clear status bit by writing a 1 to it */
        PORTA->ISFR = PORT_ISFR_ISF(1 << GPIO_8_PIN);
    }
#endif
#if GPIO_9_EN && (GPIO_9_ISR == _isr_porta_pin_detect)
    if (PORTA->ISFR & PORT_ISFR_ISF(1 << GPIO_9_PIN)) {
        irq_handler(GPIO_9);
        /* clear status bit by writing a 1 to it */
        PORTA->ISFR = PORT_ISFR_ISF(1 << GPIO_9_PIN);
    }
#endif
#if GPIO_10_EN && (GPIO_10_ISR == _isr_porta_pin_detect)
    if (PORTA->ISFR & PORT_ISFR_ISF(1 << GPIO_10_PIN)) {
        irq_handler(GPIO_10);
        /* clear status bit by writing a 1 to it */
        PORTA->ISFR = PORT_ISFR_ISF(1 << GPIO_10_PIN);
    }
#endif
#if GPIO_11_EN && (GPIO_11_ISR == _isr_porta_pin_detect)
    if (PORTA->ISFR & PORT_ISFR_ISF(1 << GPIO_11_PIN)) {
        irq_handler(GPIO_11);
        /* clear status bit by writing a 1 to it */
        PORTA->ISFR = PORT_ISFR_ISF(1 << GPIO_11_PIN);
    }
#endif
#if GPIO_12_EN && (GPIO_12_ISR == _isr_porta_pin_detect)
    if (PORTA->ISFR & PORT_ISFR_ISF(1 << GPIO_12_PIN)) {
        irq_handler(GPIO_12);
        /* clear status bit by writing a 1 to it */
        PORTA->ISFR = PORT_ISFR_ISF(1 << GPIO_12_PIN);
    }
#endif
#if GPIO_13_EN && (GPIO_13_ISR == _isr_porta_pin_detect)
    if (PORTA->ISFR & PORT_ISFR_ISF(1 << GPIO_13_PIN)) {
        irq_handler(GPIO_13);
        /* clear status bit by writing a 1 to it */
        PORTA->ISFR = PORT_ISFR_ISF(1 << GPIO_13_PIN);
    }
#endif
#if GPIO_14_EN && (GPIO_14_ISR == _isr_porta_pin_detect)
    if (PORTA->ISFR & PORT_ISFR_ISF(1 << GPIO_14_PIN)) {
        irq_handler(GPIO_14);
        /* clear status bit by writing a 1 to it */
        PORTA->ISFR = PORT_ISFR_ISF(1 << GPIO_14_PIN);
    }
#endif
#if GPIO_15_EN && (GPIO_15_ISR == _isr_porta_pin_detect)
    if (PORTA->ISFR & PORT_ISFR_ISF(1 << GPIO_15_PIN)) {
        irq_handler(GPIO_15);
        /* clear status bit by writing a 1 to it */
        PORTA->ISFR = PORT_ISFR_ISF(1 << GPIO_15_PIN);
    }
#endif
    ISR_EXIT();
}

__attribute__((naked))
void _isr_portb_pin_detect(void)
{
    ISR_ENTER();
#if GPIO_0_EN && (GPIO_0_ISR == _isr_portb_pin_detect)
    if (PORTB->ISFR & PORT_ISFR_ISF(1 << GPIO_0_PIN)) {
        irq_handler(GPIO_0);
        /* clear status bit by writing a 1 to it */
        PORTB->ISFR = PORT_ISFR_ISF(1 << GPIO_0_PIN);
    }
#endif
#if GPIO_1_EN && (GPIO_1_ISR == _isr_portb_pin_detect)
    if (PORTB->ISFR & PORT_ISFR_ISF(1 << GPIO_1_PIN)) {
        irq_handler(GPIO_1);
        /* clear status bit by writing a 1 to it */
        PORTB->ISFR = PORT_ISFR_ISF(1 << GPIO_1_PIN);
    }
#endif
#if GPIO_2_EN && (GPIO_2_ISR == _isr_portb_pin_detect)
    if (PORTB->ISFR & PORT_ISFR_ISF(1 << GPIO_2_PIN)) {
        irq_handler(GPIO_2);
        /* clear status bit by writing a 1 to it */
        PORTB->ISFR = PORT_ISFR_ISF(1 << GPIO_2_PIN);
    }
#endif
#if GPIO_3_EN && (GPIO_3_ISR == _isr_portb_pin_detect)
    if (PORTB->ISFR & PORT_ISFR_ISF(1 << GPIO_3_PIN)) {
        irq_handler(GPIO_3);
        /* clear status bit by writing a 1 to it */
        PORTB->ISFR = PORT_ISFR_ISF(1 << GPIO_3_PIN);
    }
#endif
#if GPIO_4_EN && (GPIO_4_ISR == _isr_portb_pin_detect)
    if (PORTB->ISFR & PORT_ISFR_ISF(1 << GPIO_4_PIN)) {
        irq_handler(GPIO_4);
        /* clear status bit by writing a 1 to it */
        PORTB->ISFR = PORT_ISFR_ISF(1 << GPIO_4_PIN);
    }
#endif
#if GPIO_5_EN && (GPIO_5_ISR == _isr_portb_pin_detect)
    if (PORTB->ISFR & PORT_ISFR_ISF(1 << GPIO_5_PIN)) {
        irq_handler(GPIO_5);
        /* clear status bit by writing a 1 to it */
        PORTB->ISFR = PORT_ISFR_ISF(1 << GPIO_5_PIN);
    }
#endif
#if GPIO_6_EN && (GPIO_6_ISR == _isr_portb_pin_detect)
    if (PORTB->ISFR & PORT_ISFR_ISF(1 << GPIO_6_PIN)) {
        irq_handler(GPIO_6);
        /* clear status bit by writing a 1 to it */
        PORTB->ISFR = PORT_ISFR_ISF(1 << GPIO_6_PIN);
    }
#endif
#if GPIO_7_EN && (GPIO_7_ISR == _isr_portb_pin_detect)
    if (PORTB->ISFR & PORT_ISFR_ISF(1 << GPIO_7_PIN)) {
        irq_handler(GPIO_7);
        /* clear status bit by writing a 1 to it */
        PORTB->ISFR = PORT_ISFR_ISF(1 << GPIO_7_PIN);
    }
#endif
#if GPIO_8_EN && (GPIO_8_ISR == _isr_portb_pin_detect)
    if (PORTB->ISFR & PORT_ISFR_ISF(1 << GPIO_8_PIN)) {
        irq_handler(GPIO_8);
        /* clear status bit by writing a 1 to it */
        PORTB->ISFR = PORT_ISFR_ISF(1 << GPIO_8_PIN);
    }
#endif
#if GPIO_9_EN && (GPIO_9_ISR == _isr_portb_pin_detect)
    if (PORTB->ISFR & PORT_ISFR_ISF(1 << GPIO_9_PIN)) {
        irq_handler(GPIO_9);
        /* clear status bit by writing a 1 to it */
        PORTB->ISFR = PORT_ISFR_ISF(1 << GPIO_9_PIN);
    }
#endif
#if GPIO_10_EN && (GPIO_10_ISR == _isr_portb_pin_detect)
    if (PORTB->ISFR & PORT_ISFR_ISF(1 << GPIO_10_PIN)) {
        irq_handler(GPIO_10);
        /* clear status bit by writing a 1 to it */
        PORTB->ISFR = PORT_ISFR_ISF(1 << GPIO_10_PIN);
    }
#endif
#if GPIO_11_EN && (GPIO_11_ISR == _isr_portb_pin_detect)
    if (PORTB->ISFR & PORT_ISFR_ISF(1 << GPIO_11_PIN)) {
        irq_handler(GPIO_11);
        /* clear status bit by writing a 1 to it */
        PORTB->ISFR = PORT_ISFR_ISF(1 << GPIO_11_PIN);
    }
#endif
#if GPIO_12_EN && (GPIO_12_ISR == _isr_portb_pin_detect)
    if (PORTB->ISFR & PORT_ISFR_ISF(1 << GPIO_12_PIN)) {
        irq_handler(GPIO_12);
        /* clear status bit by writing a 1 to it */
        PORTB->ISFR = PORT_ISFR_ISF(1 << GPIO_12_PIN);
    }
#endif
#if GPIO_13_EN && (GPIO_13_ISR == _isr_portb_pin_detect)
    if (PORTB->ISFR & PORT_ISFR_ISF(1 << GPIO_13_PIN)) {
        irq_handler(GPIO_13);
        /* clear status bit by writing a 1 to it */
        PORTB->ISFR = PORT_ISFR_ISF(1 << GPIO_13_PIN);
    }
#endif
#if GPIO_14_EN && (GPIO_14_ISR == _isr_portb_pin_detect)
    if (PORTB->ISFR & PORT_ISFR_ISF(1 << GPIO_14_PIN)) {
        irq_handler(GPIO_14);
        /* clear status bit by writing a 1 to it */
        PORTB->ISFR = PORT_ISFR_ISF(1 << GPIO_14_PIN);
    }
#endif
#if GPIO_15_EN && (GPIO_15_ISR == _isr_portb_pin_detect)
    if (PORTB->ISFR & PORT_ISFR_ISF(1 << GPIO_15_PIN)) {
        irq_handler(GPIO_15);
        /* clear status bit by writing a 1 to it */
        PORTB->ISFR = PORT_ISFR_ISF(1 << GPIO_15_PIN);
    }
#endif
    ISR_EXIT();
}


__attribute__((naked))
void _isr_portc_pin_detect(void)
{
    ISR_ENTER();
#if GPIO_0_EN && (GPIO_0_ISR == _isr_portc_pin_detect)
    if (PORTC->ISFR & PORT_ISFR_ISF(1 << GPIO_0_PIN)) {
        irq_handler(GPIO_0);
        /* clear status bit by writing a 1 to it */
        PORTC->ISFR = PORT_ISFR_ISF(1 << GPIO_0_PIN);
    }
#endif
#if GPIO_1_EN && (GPIO_1_ISR == _isr_portc_pin_detect)
    if (PORTC->ISFR & PORT_ISFR_ISF(1 << GPIO_1_PIN)) {
        irq_handler(GPIO_1);
        /* clear status bit by writing a 1 to it */
        PORTC->ISFR = PORT_ISFR_ISF(1 << GPIO_1_PIN);
    }
#endif
#if GPIO_2_EN && (GPIO_2_ISR == _isr_portc_pin_detect)
    if (PORTC->ISFR & PORT_ISFR_ISF(1 << GPIO_2_PIN)) {
        irq_handler(GPIO_2);
        /* clear status bit by writing a 1 to it */
        PORTC->ISFR = PORT_ISFR_ISF(1 << GPIO_2_PIN);
    }
#endif
#if GPIO_3_EN && (GPIO_3_ISR == _isr_portc_pin_detect)
    if (PORTC->ISFR & PORT_ISFR_ISF(1 << GPIO_3_PIN)) {
        irq_handler(GPIO_3);
        /* clear status bit by writing a 1 to it */
        PORTC->ISFR = PORT_ISFR_ISF(1 << GPIO_3_PIN);
    }
#endif
#if GPIO_4_EN && (GPIO_4_ISR == _isr_portc_pin_detect)
    if (PORTC->ISFR & PORT_ISFR_ISF(1 << GPIO_4_PIN)) {
        irq_handler(GPIO_4);
        /* clear status bit by writing a 1 to it */
        PORTC->ISFR = PORT_ISFR_ISF(1 << GPIO_4_PIN);
    }
#endif
#if GPIO_5_EN && (GPIO_5_ISR == _isr_portc_pin_detect)
    if (PORTC->ISFR & PORT_ISFR_ISF(1 << GPIO_5_PIN)) {
        irq_handler(GPIO_5);
        /* clear status bit by writing a 1 to it */
        PORTC->ISFR = PORT_ISFR_ISF(1 << GPIO_5_PIN);
    }
#endif
#if GPIO_6_EN && (GPIO_6_ISR == _isr_portc_pin_detect)
    if (PORTC->ISFR & PORT_ISFR_ISF(1 << GPIO_6_PIN)) {
        irq_handler(GPIO_6);
        /* clear status bit by writing a 1 to it */
        PORTC->ISFR = PORT_ISFR_ISF(1 << GPIO_6_PIN);
    }
#endif
#if GPIO_7_EN && (GPIO_7_ISR == _isr_portc_pin_detect)
    if (PORTC->ISFR & PORT_ISFR_ISF(1 << GPIO_7_PIN)) {
        irq_handler(GPIO_7);
        /* clear status bit by writing a 1 to it */
        PORTC->ISFR = PORT_ISFR_ISF(1 << GPIO_7_PIN);
    }
#endif
#if GPIO_8_EN && (GPIO_8_ISR == _isr_portc_pin_detect)
    if (PORTC->ISFR & PORT_ISFR_ISF(1 << GPIO_8_PIN)) {
        irq_handler(GPIO_8);
        /* clear status bit by writing a 1 to it */
        PORTC->ISFR = PORT_ISFR_ISF(1 << GPIO_8_PIN);
    }
#endif
#if GPIO_9_EN && (GPIO_9_ISR == _isr_portc_pin_detect)
    if (PORTC->ISFR & PORT_ISFR_ISF(1 << GPIO_9_PIN)) {
        irq_handler(GPIO_9);
        /* clear status bit by writing a 1 to it */
        PORTC->ISFR = PORT_ISFR_ISF(1 << GPIO_9_PIN);
    }
#endif
#if GPIO_10_EN && (GPIO_10_ISR == _isr_portc_pin_detect)
    if (PORTC->ISFR & PORT_ISFR_ISF(1 << GPIO_10_PIN)) {
        irq_handler(GPIO_10);
        /* clear status bit by writing a 1 to it */
        PORTC->ISFR = PORT_ISFR_ISF(1 << GPIO_10_PIN);
    }
#endif
#if GPIO_11_EN && (GPIO_11_ISR == _isr_portc_pin_detect)
    if (PORTC->ISFR & PORT_ISFR_ISF(1 << GPIO_11_PIN)) {
        irq_handler(GPIO_11);
        /* clear status bit by writing a 1 to it */
        PORTC->ISFR = PORT_ISFR_ISF(1 << GPIO_11_PIN);
    }
#endif
#if GPIO_12_EN && (GPIO_12_ISR == _isr_portc_pin_detect)
    if (PORTC->ISFR & PORT_ISFR_ISF(1 << GPIO_12_PIN)) {
        irq_handler(GPIO_12);
        /* clear status bit by writing a 1 to it */
        PORTC->ISFR = PORT_ISFR_ISF(1 << GPIO_12_PIN);
    }
#endif
#if GPIO_13_EN && (GPIO_13_ISR == _isr_portc_pin_detect)
    if (PORTC->ISFR & PORT_ISFR_ISF(1 << GPIO_13_PIN)) {
        irq_handler(GPIO_13);
        /* clear status bit by writing a 1 to it */
        PORTC->ISFR = PORT_ISFR_ISF(1 << GPIO_13_PIN);
    }
#endif
#if GPIO_14_EN && (GPIO_14_ISR == _isr_portc_pin_detect)
    if (PORTC->ISFR & PORT_ISFR_ISF(1 << GPIO_14_PIN)) {
        irq_handler(GPIO_14);
        /* clear status bit by writing a 1 to it */
        PORTC->ISFR = PORT_ISFR_ISF(1 << GPIO_14_PIN);
    }
#endif
#if GPIO_15_EN && (GPIO_15_ISR == _isr_portc_pin_detect)
    if (PORTC->ISFR & PORT_ISFR_ISF(1 << GPIO_15_PIN)) {
        irq_handler(GPIO_15);
        /* clear status bit by writing a 1 to it */
        PORTC->ISFR = PORT_ISFR_ISF(1 << GPIO_15_PIN);
    }
#endif
    ISR_EXIT();
}


__attribute__((naked))
void _isr_portd_pin_detect(void)
{
    ISR_ENTER();
#if GPIO_0_EN && (GPIO_0_ISR == _isr_portd_pin_detect)
    if (PORTD->ISFR & PORT_ISFR_ISF(1 << GPIO_0_PIN)) {
        irq_handler(GPIO_0);
        /* clear status bit by writing a 1 to it */
        PORTD->ISFR = PORT_ISFR_ISF(1 << GPIO_0_PIN);
    }
#endif
#if GPIO_1_EN && (GPIO_1_ISR == _isr_portd_pin_detect)
    if (PORTD->ISFR & PORT_ISFR_ISF(1 << GPIO_1_PIN)) {
        irq_handler(GPIO_1);
        /* clear status bit by writing a 1 to it */
        PORTD->ISFR = PORT_ISFR_ISF(1 << GPIO_1_PIN);
    }
#endif
#if GPIO_2_EN && (GPIO_2_ISR == _isr_portd_pin_detect)
    if (PORTD->ISFR & PORT_ISFR_ISF(1 << GPIO_2_PIN)) {
        irq_handler(GPIO_2);
        /* clear status bit by writing a 1 to it */
        PORTD->ISFR = PORT_ISFR_ISF(1 << GPIO_2_PIN);
    }
#endif
#if GPIO_3_EN && (GPIO_3_ISR == _isr_portd_pin_detect)
    if (PORTD->ISFR & PORT_ISFR_ISF(1 << GPIO_3_PIN)) {
        irq_handler(GPIO_3);
        /* clear status bit by writing a 1 to it */
        PORTD->ISFR = PORT_ISFR_ISF(1 << GPIO_3_PIN);
    }
#endif
#if GPIO_4_EN && (GPIO_4_ISR == _isr_portd_pin_detect)
    if (PORTD->ISFR & PORT_ISFR_ISF(1 << GPIO_4_PIN)) {
        irq_handler(GPIO_4);
        /* clear status bit by writing a 1 to it */
        PORTD->ISFR = PORT_ISFR_ISF(1 << GPIO_4_PIN);
    }
#endif
#if GPIO_5_EN && (GPIO_5_ISR == _isr_portd_pin_detect)
    if (PORTD->ISFR & PORT_ISFR_ISF(1 << GPIO_5_PIN)) {
        irq_handler(GPIO_5);
        /* clear status bit by writing a 1 to it */
        PORTD->ISFR = PORT_ISFR_ISF(1 << GPIO_5_PIN);
    }
#endif
#if GPIO_6_EN && (GPIO_6_ISR == _isr_portd_pin_detect)
    if (PORTD->ISFR & PORT_ISFR_ISF(1 << GPIO_6_PIN)) {
        irq_handler(GPIO_6);
        /* clear status bit by writing a 1 to it */
        PORTD->ISFR = PORT_ISFR_ISF(1 << GPIO_6_PIN);
    }
#endif
#if GPIO_7_EN && (GPIO_7_ISR == _isr_portd_pin_detect)
    if (PORTD->ISFR & PORT_ISFR_ISF(1 << GPIO_7_PIN)) {
        irq_handler(GPIO_7);
        /* clear status bit by writing a 1 to it */
        PORTD->ISFR = PORT_ISFR_ISF(1 << GPIO_7_PIN);
    }
#endif
#if GPIO_8_EN && (GPIO_8_ISR == _isr_portd_pin_detect)
    if (PORTD->ISFR & PORT_ISFR_ISF(1 << GPIO_8_PIN)) {
        irq_handler(GPIO_8);
        /* clear status bit by writing a 1 to it */
        PORTD->ISFR = PORT_ISFR_ISF(1 << GPIO_8_PIN);
    }
#endif
#if GPIO_9_EN && (GPIO_9_ISR == _isr_portd_pin_detect)
    if (PORTD->ISFR & PORT_ISFR_ISF(1 << GPIO_9_PIN)) {
        irq_handler(GPIO_9);
        /* clear status bit by writing a 1 to it */
        PORTD->ISFR = PORT_ISFR_ISF(1 << GPIO_9_PIN);
    }
#endif
#if GPIO_10_EN && (GPIO_10_ISR == _isr_portd_pin_detect)
    if (PORTD->ISFR & PORT_ISFR_ISF(1 << GPIO_10_PIN)) {
        irq_handler(GPIO_10);
        /* clear status bit by writing a 1 to it */
        PORTD->ISFR = PORT_ISFR_ISF(1 << GPIO_10_PIN);
    }
#endif
#if GPIO_11_EN && (GPIO_11_ISR == _isr_portd_pin_detect)
    if (PORTD->ISFR & PORT_ISFR_ISF(1 << GPIO_11_PIN)) {
        irq_handler(GPIO_11);
        /* clear status bit by writing a 1 to it */
        PORTD->ISFR = PORT_ISFR_ISF(1 << GPIO_11_PIN);
    }
#endif
#if GPIO_12_EN && (GPIO_12_ISR == _isr_portd_pin_detect)
    if (PORTD->ISFR & PORT_ISFR_ISF(1 << GPIO_12_PIN)) {
        irq_handler(GPIO_12);
        /* clear status bit by writing a 1 to it */
        PORTD->ISFR = PORT_ISFR_ISF(1 << GPIO_12_PIN);
    }
#endif
#if GPIO_13_EN && (GPIO_13_ISR == _isr_portd_pin_detect)
    if (PORTD->ISFR & PORT_ISFR_ISF(1 << GPIO_13_PIN)) {
        irq_handler(GPIO_13);
        /* clear status bit by writing a 1 to it */
        PORTD->ISFR = PORT_ISFR_ISF(1 << GPIO_13_PIN);
    }
#endif
#if GPIO_14_EN && (GPIO_14_ISR == _isr_portd_pin_detect)
    if (PORTD->ISFR & PORT_ISFR_ISF(1 << GPIO_14_PIN)) {
        irq_handler(GPIO_14);
        /* clear status bit by writing a 1 to it */
        PORTD->ISFR = PORT_ISFR_ISF(1 << GPIO_14_PIN);
    }
#endif
#if GPIO_15_EN && (GPIO_15_ISR == _isr_portd_pin_detect)
    if (PORTD->ISFR & PORT_ISFR_ISF(1 << GPIO_15_PIN)) {
        irq_handler(GPIO_15);
        /* clear status bit by writing a 1 to it */
        PORTD->ISFR = PORT_ISFR_ISF(1 << GPIO_15_PIN);
    }
#endif
    ISR_EXIT();
}


__attribute__((naked))
void _isr_porte_pin_detect(void)
{
    ISR_ENTER();
#if GPIO_0_EN && (GPIO_0_ISR == _isr_porte_pin_detect)
    if (PORTE->ISFR & PORT_ISFR_ISF(1 << GPIO_0_PIN)) {
        irq_handler(GPIO_0);
        /* clear status bit by writing a 1 to it */
        PORTE->ISFR = PORT_ISFR_ISF(1 << GPIO_0_PIN);
    }
#endif
#if GPIO_1_EN && (GPIO_1_ISR == _isr_porte_pin_detect)
    if (PORTE->ISFR & PORT_ISFR_ISF(1 << GPIO_1_PIN)) {
        irq_handler(GPIO_1);
        /* clear status bit by writing a 1 to it */
        PORTE->ISFR = PORT_ISFR_ISF(1 << GPIO_1_PIN);
    }
#endif
#if GPIO_2_EN && (GPIO_2_ISR == _isr_porte_pin_detect)
    if (PORTE->ISFR & PORT_ISFR_ISF(1 << GPIO_2_PIN)) {
        irq_handler(GPIO_2);
        /* clear status bit by writing a 1 to it */
        PORTE->ISFR = PORT_ISFR_ISF(1 << GPIO_2_PIN);
    }
#endif
#if GPIO_3_EN && (GPIO_3_ISR == _isr_porte_pin_detect)
    if (PORTE->ISFR & PORT_ISFR_ISF(1 << GPIO_3_PIN)) {
        irq_handler(GPIO_3);
        /* clear status bit by writing a 1 to it */
        PORTE->ISFR = PORT_ISFR_ISF(1 << GPIO_3_PIN);
    }
#endif
#if GPIO_4_EN && (GPIO_4_ISR == _isr_porte_pin_detect)
    if (PORTE->ISFR & PORT_ISFR_ISF(1 << GPIO_4_PIN)) {
        irq_handler(GPIO_4);
        /* clear status bit by writing a 1 to it */
        PORTE->ISFR = PORT_ISFR_ISF(1 << GPIO_4_PIN);
    }
#endif
#if GPIO_5_EN && (GPIO_5_ISR == _isr_porte_pin_detect)
    if (PORTE->ISFR & PORT_ISFR_ISF(1 << GPIO_5_PIN)) {
        irq_handler(GPIO_5);
        /* clear status bit by writing a 1 to it */
        PORTE->ISFR = PORT_ISFR_ISF(1 << GPIO_5_PIN);
    }
#endif
#if GPIO_6_EN && (GPIO_6_ISR == _isr_porte_pin_detect)
    if (PORTE->ISFR & PORT_ISFR_ISF(1 << GPIO_6_PIN)) {
        irq_handler(GPIO_6);
        /* clear status bit by writing a 1 to it */
        PORTE->ISFR = PORT_ISFR_ISF(1 << GPIO_6_PIN);
    }
#endif
#if GPIO_7_EN && (GPIO_7_ISR == _isr_porte_pin_detect)
    if (PORTE->ISFR & PORT_ISFR_ISF(1 << GPIO_7_PIN)) {
        irq_handler(GPIO_7);
        /* clear status bit by writing a 1 to it */
        PORTE->ISFR = PORT_ISFR_ISF(1 << GPIO_7_PIN);
    }
#endif
#if GPIO_8_EN && (GPIO_8_ISR == _isr_porte_pin_detect)
    if (PORTE->ISFR & PORT_ISFR_ISF(1 << GPIO_8_PIN)) {
        irq_handler(GPIO_8);
        /* clear status bit by writing a 1 to it */
        PORTE->ISFR = PORT_ISFR_ISF(1 << GPIO_8_PIN);
    }
#endif
#if GPIO_9_EN && (GPIO_9_ISR == _isr_porte_pin_detect)
    if (PORTE->ISFR & PORT_ISFR_ISF(1 << GPIO_9_PIN)) {
        irq_handler(GPIO_9);
        /* clear status bit by writing a 1 to it */
        PORTE->ISFR = PORT_ISFR_ISF(1 << GPIO_9_PIN);
    }
#endif
#if GPIO_10_EN && (GPIO_10_ISR == _isr_porte_pin_detect)
    if (PORTE->ISFR & PORT_ISFR_ISF(1 << GPIO_10_PIN)) {
        irq_handler(GPIO_10);
        /* clear status bit by writing a 1 to it */
        PORTE->ISFR = PORT_ISFR_ISF(1 << GPIO_10_PIN);
    }
#endif
#if GPIO_11_EN && (GPIO_11_ISR == _isr_porte_pin_detect)
    if (PORTE->ISFR & PORT_ISFR_ISF(1 << GPIO_11_PIN)) {
        irq_handler(GPIO_11);
        /* clear status bit by writing a 1 to it */
        PORTE->ISFR = PORT_ISFR_ISF(1 << GPIO_11_PIN);
    }
#endif
#if GPIO_12_EN && (GPIO_12_ISR == _isr_porte_pin_detect)
    if (PORTE->ISFR & PORT_ISFR_ISF(1 << GPIO_12_PIN)) {
        irq_handler(GPIO_12);
        /* clear status bit by writing a 1 to it */
        PORTE->ISFR = PORT_ISFR_ISF(1 << GPIO_12_PIN);
    }
#endif
#if GPIO_13_EN && (GPIO_13_ISR == _isr_porte_pin_detect)
    if (PORTE->ISFR & PORT_ISFR_ISF(1 << GPIO_13_PIN)) {
        irq_handler(GPIO_13);
        /* clear status bit by writing a 1 to it */
        PORTE->ISFR = PORT_ISFR_ISF(1 << GPIO_13_PIN);
    }
#endif
#if GPIO_14_EN && (GPIO_14_ISR == _isr_porte_pin_detect)
    if (PORTE->ISFR & PORT_ISFR_ISF(1 << GPIO_14_PIN)) {
        irq_handler(GPIO_14);
        /* clear status bit by writing a 1 to it */
        PORTE->ISFR = PORT_ISFR_ISF(1 << GPIO_14_PIN);
    }
#endif
#if GPIO_15_EN && (GPIO_15_ISR == _isr_porte_pin_detect)
    if (PORTE->ISFR & PORT_ISFR_ISF(1 << GPIO_15_PIN)) {
        irq_handler(GPIO_15);
        /* clear status bit by writing a 1 to it */
        PORTE->ISFR = PORT_ISFR_ISF(1 << GPIO_15_PIN);
    }
#endif
    ISR_EXIT();
}


#endif /* GPIO_NUMOF */
