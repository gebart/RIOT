/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_k60
 * @{
 *
 * @file
 *
 * @brief Interrupt vector for K60 MCU.
 *
 * @author Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @note It is not necessary to modify this file to define custom interrupt
 * service routines. All symbols are defined weak, it is only necessary to
 * define a function with the same name in another file to override the default
 * interrupt handlers.
 */

/**
 * @name Interrupt vector definition
 * @{
 */

#define ISR_VECTOR_SECTION __attribute__ ((used,section(".vector_table")))
void reset_handler(void) __attribute__((naked));

/* Default handler for interrupts, infinite loop */
static void unhandled_interrupt(void) __attribute__((unused));

#define UNHANDLED_ALIAS __attribute__((weak, alias("unhandled_interrupt")));

/* __attribute__((naked)) in order to not add any function prologue to the
 * default hardfault handler written in asm */
/* __attribute__((unused)) in order to avoid (incorrect) compiler warnings about
 * the functions being unused when only referenced from the weak alias. */
static void dHardFault_handler(void) __attribute__((naked, unused));
static void dMemManage_handler(void) __attribute__((unused));
static void dUsageFault_handler(void) __attribute__((unused));
static void dBusFault_handler(void) __attribute__((unused));
static void dNMI_handler(void) __attribute__((unused));

/* ARM Cortex defined interrupt vectors */
void reset_handler(void) __attribute__((naked));
void isr_nmi(void) __attribute__((weak, alias("dNMI_handler")));
void isr_hard_fault(void) __attribute__((weak, alias("dHardFault_handler")));
void isr_mem_manage(void) __attribute__((weak, alias("dMemManage_handler")));
void isr_bus_fault(void) __attribute__((weak, alias("dBusFault_handler")));
void isr_usage_fault(void) __attribute__((weak, alias("dUsageFault_handler")));
void isr_reserved(void) UNHANDLED_ALIAS;
/* void isr_reserved(void) UNHANDLED_ALIAS; */
/* void isr_reserved(void) UNHANDLED_ALIAS; */
/* void isr_reserved(void) UNHANDLED_ALIAS; */
void isr_svc(void) UNHANDLED_ALIAS;
void isr_debug_mon(void) UNHANDLED_ALIAS;
/* void _isr_reserved(void) UNHANDLED_ALIAS; */
void isr_pendsv(void) UNHANDLED_ALIAS;
void isr_systick(void) UNHANDLED_ALIAS;

/* device-specific (freescale) defined interrupt vectors */
void isr_dma0_complete(void) UNHANDLED_ALIAS;
void isr_dma1_complete(void) UNHANDLED_ALIAS;
void isr_dma2_complete(void) UNHANDLED_ALIAS;
void isr_dma3_complete(void) UNHANDLED_ALIAS;
void isr_dma4_complete(void) UNHANDLED_ALIAS;
void isr_dma5_complete(void) UNHANDLED_ALIAS;
void isr_dma6_complete(void) UNHANDLED_ALIAS;
void isr_dma7_complete(void) UNHANDLED_ALIAS;
void isr_dma8_complete(void) UNHANDLED_ALIAS;
void isr_dma9_complete(void) UNHANDLED_ALIAS;
void isr_dma10_complete(void) UNHANDLED_ALIAS;
void isr_dma11_complete(void) UNHANDLED_ALIAS;
void isr_dma12_complete(void) UNHANDLED_ALIAS;
void isr_dma13_complete(void) UNHANDLED_ALIAS;
void isr_dma14_complete(void) UNHANDLED_ALIAS;
void isr_dma15_complete(void) UNHANDLED_ALIAS;
void isr_dma_error(void) UNHANDLED_ALIAS;
void isr_mcm(void) UNHANDLED_ALIAS;
void isr_flash_command_complete(void) UNHANDLED_ALIAS;
void isr_flash_read_collision(void) UNHANDLED_ALIAS;
void isr_low_voltage(void) UNHANDLED_ALIAS;
void isr_llwu(void) UNHANDLED_ALIAS;
void isr_watchdog(void) UNHANDLED_ALIAS;
void isr_random_number_generator(void) UNHANDLED_ALIAS;
void isr_i2c0(void) UNHANDLED_ALIAS;
void isr_i2c1(void) UNHANDLED_ALIAS;
void isr_spi0(void) UNHANDLED_ALIAS;
void isr_spi1(void) UNHANDLED_ALIAS;
void isr_spi2(void) UNHANDLED_ALIAS;
void isr_can0_ored_msg_buffer(void) UNHANDLED_ALIAS;
void isr_can0_bus_off(void) UNHANDLED_ALIAS;
void isr_can0_error(void) UNHANDLED_ALIAS;
void isr_can0_tx_warn(void) UNHANDLED_ALIAS;
void isr_can0_rx_warn(void) UNHANDLED_ALIAS;
void isr_can0_wake_up(void) UNHANDLED_ALIAS;
void isr_i2s0_tx(void) UNHANDLED_ALIAS;
void isr_i2s0_rx(void) UNHANDLED_ALIAS;
void isr_can1_ored_msg_buffer(void) UNHANDLED_ALIAS;
void isr_can1_bus_off(void) UNHANDLED_ALIAS;
void isr_can1_error(void) UNHANDLED_ALIAS;
void isr_can1_tx_warn(void) UNHANDLED_ALIAS;
void isr_can1_rx_warn(void) UNHANDLED_ALIAS;
void isr_can1_wake_up(void) UNHANDLED_ALIAS;
/* void isr_reserved(void) UNHANDLED_ALIAS; */
void isr_uart0_lon(void) UNHANDLED_ALIAS;
void isr_uart0_status(void) UNHANDLED_ALIAS;
void isr_uart0_error(void) UNHANDLED_ALIAS;
void isr_uart1_status(void) UNHANDLED_ALIAS;
void isr_uart1_error(void) UNHANDLED_ALIAS;
void isr_uart2_status(void) UNHANDLED_ALIAS;
void isr_uart2_error(void) UNHANDLED_ALIAS;
void isr_uart3_status(void) UNHANDLED_ALIAS;
void isr_uart3_error(void) UNHANDLED_ALIAS;
void isr_uart4_status(void) UNHANDLED_ALIAS;
void isr_uart4_error(void) UNHANDLED_ALIAS;
/* void isr_reserved(void) UNHANDLED_ALIAS; */
/* void isr_reserved(void) UNHANDLED_ALIAS; */
void isr_adc0(void) UNHANDLED_ALIAS;
void isr_adc1(void) UNHANDLED_ALIAS;
void isr_cmp0(void) UNHANDLED_ALIAS;
void isr_cmp1(void) UNHANDLED_ALIAS;
void isr_cmp2(void) UNHANDLED_ALIAS;
void isr_ftm0(void) UNHANDLED_ALIAS;
void isr_ftm1(void) UNHANDLED_ALIAS;
void isr_ftm2(void) UNHANDLED_ALIAS;
void isr_cmt(void) UNHANDLED_ALIAS;
void isr_rtc_alarm(void) UNHANDLED_ALIAS;
void isr_rtc_seconds(void) UNHANDLED_ALIAS;
void isr_pit0(void) UNHANDLED_ALIAS;
void isr_pit1(void) UNHANDLED_ALIAS;
void isr_pit2(void) UNHANDLED_ALIAS;
void isr_pit3(void) UNHANDLED_ALIAS;
void isr_pdb(void) UNHANDLED_ALIAS;
void isr_usb_otg(void) UNHANDLED_ALIAS;
void isr_usb_charger_detect(void) UNHANDLED_ALIAS;
void isr_enet_1588_timer(void) UNHANDLED_ALIAS;
void isr_enet_tx(void) UNHANDLED_ALIAS;
void isr_enet_rx(void) UNHANDLED_ALIAS;
void isr_enet_error_misc(void) UNHANDLED_ALIAS;
/* void isr_reserved(void) UNHANDLED_ALIAS; */
void isr_sdhc(void) UNHANDLED_ALIAS;
void isr_dac0(void) UNHANDLED_ALIAS;
/* void isr_reserved(void) UNHANDLED_ALIAS; */
void isr_tsi(void) UNHANDLED_ALIAS;
void isr_mcg(void) UNHANDLED_ALIAS;
void isr_lptmr0(void) UNHANDLED_ALIAS;
/* void isr_reserved(void) UNHANDLED_ALIAS; */
void isr_porta_pin_detect(void) UNHANDLED_ALIAS;
void isr_portb_pin_detect(void) UNHANDLED_ALIAS;
void isr_portc_pin_detect(void) UNHANDLED_ALIAS;
void isr_portd_pin_detect(void) UNHANDLED_ALIAS;
void isr_porte_pin_detect(void) UNHANDLED_ALIAS;
/* void isr_reserved(void) UNHANDLED_ALIAS; */
/* void isr_reserved(void) UNHANDLED_ALIAS; */
void isr_software(void) UNHANDLED_ALIAS;

extern void* _estack[];

typedef void (*ISR_func)(void);

/**
 * @brief Interrupt vector definition
 */
const ISR_func isr_vector[111] ISR_VECTOR_SECTION =
{
  /* ARM Cortex defined interrupt vectors */
  (ISR_func)_estack,
  reset_handler,
  isr_nmi,
  isr_hard_fault,
  isr_mem_manage,
  isr_bus_fault,
  isr_usage_fault,
  isr_reserved,
  isr_reserved,
  isr_reserved,
  isr_reserved,
  isr_svc,
  isr_debug_mon,
  isr_reserved,
  isr_pendsv,
  isr_systick,

  /* Device-specific (Freescale defined) interrupt vectors */
  isr_dma0_complete,
  isr_dma1_complete,
  isr_dma2_complete,
  isr_dma3_complete,
  isr_dma4_complete,
  isr_dma5_complete,
  isr_dma6_complete,
  isr_dma7_complete,
  isr_dma8_complete,
  isr_dma9_complete,
  isr_dma10_complete,
  isr_dma11_complete,
  isr_dma12_complete,
  isr_dma13_complete,
  isr_dma14_complete,
  isr_dma15_complete,
  isr_dma_error,
  isr_mcm,
  isr_flash_command_complete,
  isr_flash_read_collision,
  isr_low_voltage,
  isr_llwu,
  isr_watchdog,
  isr_random_number_generator,
  isr_i2c0,
  isr_i2c1,
  isr_spi0,
  isr_spi1,
  isr_spi2,
  isr_can0_ored_msg_buffer,
  isr_can0_bus_off,
  isr_can0_error,
  isr_can0_tx_warn,
  isr_can0_rx_warn,
  isr_can0_wake_up,
  isr_i2s0_tx,
  isr_i2s0_rx,
  isr_can1_ored_msg_buffer,
  isr_can1_bus_off,
  isr_can1_error,
  isr_can1_tx_warn,
  isr_can1_rx_warn,
  isr_can1_wake_up,
  isr_reserved,
  isr_uart0_lon,
  isr_uart0_status,
  isr_uart0_error,
  isr_uart1_status,
  isr_uart1_error,
  isr_uart2_status,
  isr_uart2_error,
  isr_uart3_status,
  isr_uart3_error,
  isr_uart4_status,
  isr_uart4_error,
  isr_reserved,
  isr_reserved,
  isr_adc0,
  isr_adc1,
  isr_cmp0,
  isr_cmp1,
  isr_cmp2,
  isr_ftm0,
  isr_ftm1,
  isr_ftm2,
  isr_cmt,
  isr_rtc_alarm,
  isr_rtc_seconds,
  isr_pit0,
  isr_pit1,
  isr_pit2,
  isr_pit3,
  isr_pdb,
  isr_usb_otg,
  isr_usb_charger_detect,
  isr_enet_1588_timer,
  isr_enet_tx,
  isr_enet_rx,
  isr_enet_error_misc,
  isr_reserved,
  isr_sdhc,
  isr_dac0,
  isr_reserved,
  isr_tsi,
  isr_mcg,
  isr_lptmr0,
  isr_reserved,
  isr_porta_pin_detect,
  isr_portb_pin_detect,
  isr_portc_pin_detect,
  isr_portd_pin_detect,
  isr_porte_pin_detect,
  isr_reserved,
  isr_reserved,
  isr_software
};

/**
 * @brief Default handler for Non-Maskable Interrupt
 */
void
dNMI_handler(void)
{
  while(1);
}

/**
 * @brief Default handler for unhandled interrupts.
 */
static void
unhandled_interrupt(void)
{
  while(1);
}
/**
 * Default handler of Hard Faults
 *
 * This function is only an assembly language wrapper for the function
 * hard_fault_handler_c, defined in fault-handlers.c
 */
static void
dHardFault_handler(void)
{
  __asm volatile
  (
    "tst lr, #4\n"
    "ite eq\n"
    "mrseq r0, msp\n"
    "mrsne r0, psp\n"
    "b hard_fault_handler_c\n"
  );
  while(1);
}
/**
 * Default handler of Usage Fault
 */
static void
dUsageFault_handler(void)
{
  while(1);
}
/**
 * Default handler of MemManage Fault
 */
static void
dMemManage_handler(void)
{
  while(1);
}
/**
 * Default handler of Bus Fault
 */
static void
dBusFault_handler(void)
{
  while(1);
}
/** @} */

/** @} */
