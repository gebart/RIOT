/*
 * Copyright (C) 2014-2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#ifndef DEVICEMAP_H_
#define DEVICEMAP_H_

/**
 * @ingroup     board_mulle
 * @{
 *
 * @file
 * @brief  Device I/O mappings for the Mulle platform.
 *
 * @author Joakim Gebart <joakim.gebart@eistec.se>
 *
 */

#include "devopttab.h"

/** Maximum number of file descriptors allocated to hardware devices. All fd's
 * above this number will be remapped to CFS accesses. */
#define MAX_OPEN_DEVICES 16 /* Arbitrarily chosen */

/** Number of IO devices in this platform implementation */
#define NUM_IO_DEVICES 16 /* Arbitrarily chosen */

#ifdef __cplusplus
extern "C" {
#endif

/* This table maps the standard streams to device operations table entries. */
extern const devoptab_t *devoptab_list[MAX_OPEN_DEVICES];

/* This table maps filenames to devices */
typedef struct {
    const char *name;
    const devoptab_t *devoptab;
} devoptab_name_t;

typedef struct {
    unsigned int len;
    const devoptab_name_t *data;
} devoptab_name_list_t;

extern const devoptab_name_list_t devoptab_name_list;

void devicemap_init(void);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* !defined(DEVICEMAP_H_) */
