


#include "cpu.h"
#include "periph_conf.h"
#include "periph/uart.h"

#if UART_0_EN || UART_1_EN || UART_2_EN

typedef struct {
    uart_cb_t cb;
} uart_conf_t;

static uart_conf_t conf[UART_NUMOF];

int uart_init(uart_t dev, uint32_t baudrate)
{
    switch (dev) {
        case UART_0:

            /* configure pins */
            PINSEL0 |= ((1 << 4) | (1 << 6));
            PINSEL0 |= ~((1 << 5) | (1 << 7));
            break;
        default:
            return -1;
    }
}

void isr_uart0(void) __attribute__((interrupt("IRQ")));
void isr_uart0(void)
{
    int iir;
    iir = U0IIR;

    switch (iir & UIIR_ID_MASK) {
        case UIIR_THRE_INT:               // Transmit Holding Register Empty
            fifo = 0;
            push_queue();
            break;

        case UIIR_CTI_INT:                // Character Timeout Indicator
        case UIIR_RDA_INT:                // Receive Data Available

#ifdef MODULE_UART0
            if (uart0_handler_pid != KERNEL_PID_UNDEF) {
                do {
                    int c = U0RBR;
                    uart0_handle_incoming(c);
                }
                while (U0LSR & ULSR_RDR);

                uart0_notify_thread();
            }

#endif
            break;

        default:
            U0LSR;
            U0RBR;
            break;
    } // switch

    VICVectAddr = 0;                    // Acknowledge Interrupt
}

#endif /* UART_0_EN || UART_1_EN || UART_2_EN */
