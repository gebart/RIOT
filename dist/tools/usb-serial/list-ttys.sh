#!/bin/bash

#
# Copyright (C) 2015 Eistec AB
#
# This file is subject to the terms and conditions of the GNU Lesser General
# Public License v2.1. See the file LICENSE in the top level directory for more
# details.
#

if [ ! -d /sys/bus/usb/devices ]; then
    echo "$(basename $0): /sys/bus/usb/devices not a directory (/sys is not mounted?)" >&2
    exit 1
fi

# iterate over usb-tty devices:
for dev in $(find /sys/bus/usb/devices/ -mindepth 3 -maxdepth 3 -name tty -follow -printf '%h\n' 2>/dev/null); do
    parent=$(dirname ${dev})
    serial=$(cat "${parent}/serial" 2>/dev/null)
    manuf=$(cat "${parent}/manufacturer" 2>/dev/null)
    product=$(cat "${parent}/product" 2>/dev/null)
    ttys=$(ls ${dev}/tty 2>/dev/null)
    # If at least one tty is assigned to the device we will write its info to stdout.
    if [ ! -z "${ttys}" ]; then
        echo "${parent}: ${manuf} ${product} serial: '${serial}', tty(s): $(for t in ${ttys}; do echo -n "${t}, "; done)"
    fi
done
