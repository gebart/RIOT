/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_msba2
 * @{
 *
 * @file
 * @brief       Board specific implementations for the MSB-A2
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

void board_init(void)
{
    /* configure LED pins */
    SCS |= 1;                   /* globally enable high speed GPIO mode */
    FIO3DIR |= (LED_GREEN_PIN | LED_RED_PIN);   /* set as output */
    FIO3SET |= (LED_GREEN_PIN | LED_RED_PIN);   /* turn both off */

    /* initialize the CPU */
    cpu_init();
}
