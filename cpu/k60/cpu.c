/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <stdint.h>
#include "cpu.h"
#include "board.h"

/**
 * @ingroup     cpu_k60
 * @{
 *
 * @file
 * @brief       Implementation of K60 CPU initialization.
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 * @}
 */

extern void *_vector_rom[];

/** @brief Current core clock frequency */
uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;
/** @brief Current system clock frequency */
uint32_t SystemSysClock = DEFAULT_SYSTEM_CLOCK;
/** @brief Current bus clock frequency */
uint32_t SystemBusClock = DEFAULT_SYSTEM_CLOCK;
/** @brief Current FlexBus clock frequency */
uint32_t SystemFlexBusClock = DEFAULT_SYSTEM_CLOCK;
/** @brief Current flash clock frequency */
uint32_t SystemFlashClock = DEFAULT_SYSTEM_CLOCK;
/** @brief Number of full PIT ticks in one microsecond. */
uint32_t PIT_ticks_per_usec = (DEFAULT_SYSTEM_CLOCK / 1000000ul);

/**
 * @brief Check the running CPU identification to find if we are running on the
 *        wrong hardware.
 */
static void check_running_cpu_revision(void);

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* Check that we are running on the CPU that this code was built for */
    check_running_cpu_revision();

    /* configure the vector table location to internal flash */
    SCB->VTOR = (uint32_t)_vector_rom;

    /* set pendSV interrupt to lowest possible priority */
    NVIC_SetPriority(PendSV_IRQn, 0xff);

}

static void check_running_cpu_revision(void)
{
    /* Check that the running CPU revision matches the compiled revision */
    if (SCB->CPUID != K60_EXPECTED_CPUID) {
        uint32_t CPUID = SCB->CPUID; /* This is only to ease debugging, type
                   * "print /x CPUID" in gdb */
        uint32_t SILICON_REVISION = (SCB->CPUID & SCB_CPUID_REVISION_Msk) + 1;
        (void)CPUID; /* prevents compiler warnings about an unused variable. */
        (void)SILICON_REVISION;

        /* Running on the wrong CPU, the clock initialization is different
         * between silicon revision 1.x and 2.x (LSB of CPUID) */
        /* If you unexpectedly end up on this line when debugging:
         * Rebuild the code using the correct value for K60_CPU_REV */
        DEBUGGER_BREAK(BREAK_WRONG_K60_CPU_REV);

        while (1);
    }
}

void SystemCoreClockUpdate(void)
{
    uint32_t MCGOUTClock;                                                        /* Variable to store output clock frequency of the MCG module */
    uint8_t Divider;

    if ((MCG->C1 & MCG_C1_CLKS_MASK) == 0x0u) {
        /* Output of FLL or PLL is selected */
        if ((MCG->C6 & MCG_C6_PLLS_MASK) == 0x0u) {
            /* FLL is selected */
            if ((MCG->C1 & MCG_C1_IREFS_MASK) == 0x0u) {
                /* External reference clock is selected */
#if K60_CPU_REV == 1
                /* rev.1 silicon */
                if ((SIM->SOPT2 & SIM_SOPT2_MCGCLKSEL_MASK) == 0x0u) {
                    MCGOUTClock =
                        CPU_XTAL_CLK_HZ;                                       /* System oscillator drives MCG clock */
                }
                else {   /* (!((SIM->SOPT2 & SIM_SOPT2_MCGCLKSEL_MASK) == 0x0u)) */
                    MCGOUTClock =
                        CPU_XTAL32k_CLK_HZ;                                    /* RTC 32 kHz oscillator drives MCG clock */
                } /* (!((SIM->SOPT2 & SIM_SOPT2_MCGCLKSEL_MASK) == 0x0u)) */

#else /* K60_CPU_REV != 1 */

                /* rev.2 silicon */
                if ((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u) {
                    MCGOUTClock =
                        CPU_XTAL_CLK_HZ;                                       /* System oscillator drives MCG clock */
                }
                else {   /* (!((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u)) */
                    MCGOUTClock =
                        CPU_XTAL32k_CLK_HZ;                                    /* RTC 32 kHz oscillator drives MCG clock */
                } /* (!((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u)) */

#endif /* K60_CPU_REV != 1 */
                Divider = (uint8_t)(1u << ((MCG->C1 & MCG_C1_FRDIV_MASK) >> MCG_C1_FRDIV_SHIFT));
                MCGOUTClock = (MCGOUTClock / Divider);  /* Calculate the divided FLL reference clock */

                if ((MCG->C2 & MCG_C2_RANGE0_MASK) != 0x0u) {
                    MCGOUTClock /=
                        32u;                                                  /* If high range is enabled, additional 32 divider is active */
                } /* ((MCG->C2 & MCG_C2_RANGE0_MASK) != 0x0u) */
            }
            else {   /* (!((MCG->C1 & MCG_C1_IREFS_MASK) == 0x0u)) */
                MCGOUTClock =
                    CPU_INT_SLOW_CLK_HZ;                                     /* The slow internal reference clock is selected */
            } /* (!((MCG->C1 & MCG_C1_IREFS_MASK) == 0x0u)) */

            /* Select correct multiplier to calculate the MCG output clock  */
            switch (MCG->C4 & (MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS_MASK)) {
                case (0x0u):
                    MCGOUTClock *= 640u;
                    break;

                case (MCG_C4_DRST_DRS(0b01)): /* 0x20u */
                    MCGOUTClock *= 1280u;
                    break;

                case (MCG_C4_DRST_DRS(0b10)): /* 0x40u */
                    MCGOUTClock *= 1920u;
                    break;

                case (MCG_C4_DRST_DRS(0b11)): /* 0x60u */
                    MCGOUTClock *= 2560u;
                    break;

                case (MCG_C4_DMX32_MASK): /* 0x80u */
                    MCGOUTClock *= 732u;
                    break;

                case (MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0b01)): /* 0xA0u */
                    MCGOUTClock *= 1464u;
                    break;

                case (MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0b10)): /* 0xC0u */
                    MCGOUTClock *= 2197u;
                    break;

                case (MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0b11)): /* 0xE0u */
                    MCGOUTClock *= 2929u;
                    break;

                default:
                    break;
            }
        }
        else {   /* (!((MCG->C6 & MCG_C6_PLLS_MASK) == 0x0u)) */
            /* PLL is selected */
            Divider = (1u + (MCG->C5 & MCG_C5_PRDIV0_MASK));
            MCGOUTClock = (uint32_t)(CPU_XTAL_CLK_HZ /
                                     Divider);                     /* Calculate the PLL reference clock */
            Divider = ((MCG->C6 & MCG_C6_VDIV0_MASK) + 24u);
            MCGOUTClock *= Divider;                       /* Calculate the MCG output clock */
        } /* (!((MCG->C6 & MCG_C6_PLLS_MASK) == 0x0u)) */
    }
    else if ((MCG->C1 & MCG_C1_CLKS_MASK) == MCG_C1_CLKS(0b01)) {   /* 0x40u */
        /* Internal reference clock is selected */
        if ((MCG->C2 & MCG_C2_IRCS_MASK) == 0x0u) {
            MCGOUTClock =
                CPU_INT_SLOW_CLK_HZ;                                       /* Slow internal reference clock selected */
        }
        else {   /* (!((MCG->C2 & MCG_C2_IRCS_MASK) == 0x0u)) */
#if K60_CPU_REV == 1
            /* rev.1 silicon */
            MCGOUTClock = CPU_INT_FAST_CLK_HZ;  /* Fast internal reference clock selected */
#else /* K60_CPU_REV != 1 */
            /* rev.2 silicon */
            MCGOUTClock = CPU_INT_FAST_CLK_HZ / (1 << ((MCG->SC & MCG_SC_FCRDIV_MASK) >>
                                                 MCG_SC_FCRDIV_SHIFT));  /* Fast internal reference clock selected */
#endif /* K60_CPU_REV != 1 */
        } /* (!((MCG->C2 & MCG_C2_IRCS_MASK) == 0x0u)) */
    }
    else if ((MCG->C1 & MCG_C1_CLKS_MASK) == MCG_C1_CLKS(0b10)) {   /* 0x80u */
        /* External reference clock is selected */
#if K60_CPU_REV == 1
        /* rev.1 silicon */
        if ((SIM->SOPT2 & SIM_SOPT2_MCGCLKSEL_MASK) == 0x0u) {
            MCGOUTClock =
                CPU_XTAL_CLK_HZ;                                           /* System oscillator drives MCG clock */
        }
        else {   /* (!((SIM->SOPT2 & SIM_SOPT2_MCGCLKSEL_MASK) == 0x0u)) */
            MCGOUTClock =
                CPU_XTAL32k_CLK_HZ;                                        /* RTC 32 kHz oscillator drives MCG clock */
        } /* (!((SIM->SOPT2 & SIM_SOPT2_MCGCLKSEL_MASK) == 0x0u)) */

#else /* K60_CPU_REV != 1 */

        /* rev.2 silicon */
        if ((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u) {
            MCGOUTClock =
                CPU_XTAL_CLK_HZ;                                           /* System oscillator drives MCG clock */
        }
        else {   /* (!((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u)) */
            MCGOUTClock =
                CPU_XTAL32k_CLK_HZ;                                        /* RTC 32 kHz oscillator drives MCG clock */
        } /* (!((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u)) */

#endif /* K60_CPU_REV != 1 */
    }
    else {   /* (!((MCG->C1 & MCG_C1_CLKS_MASK) == 0x80u)) */
        /* Reserved value */
        return;
    } /* (!((MCG->C1 & MCG_C1_CLKS_MASK) == 0x80u)) */

    SystemCoreClock = SystemSysClock = (MCGOUTClock / (1u + ((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK)
                                        >> SIM_CLKDIV1_OUTDIV1_SHIFT)));
    SystemBusClock = (MCGOUTClock / (1u + ((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK) >>
                                           SIM_CLKDIV1_OUTDIV2_SHIFT)));
    SystemFlexBusClock = (MCGOUTClock / (1u + ((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV3_MASK) >>
                                         SIM_CLKDIV1_OUTDIV3_SHIFT)));
    SystemFlashClock = (MCGOUTClock / (1u + ((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV4_MASK) >>
                                       SIM_CLKDIV1_OUTDIV4_SHIFT)));

    /* Module helper variables */
    if (SystemBusClock >= 1000000) {
        /* PIT module clock_delay_usec scale factor */
        PIT_ticks_per_usec = (SystemBusClock + 500000) / 1000000; /* Rounded to nearest integer */
    }
    else {
        /* less than 1 MHz clock frequency on the PIT module, round up. */
        PIT_ticks_per_usec = 1;
    }
}

/** @} */
