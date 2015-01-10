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

echo "These are the currently connected FTDI devices:"

# Find all FT2232H devices
for dev in /sys/bus/usb/devices/*; do
    if [ ! -f "${dev}/idVendor" ]; then
        # not a main device
        continue
    fi
    # filter out any devices not identified as 0403:6010
    if grep -v '0403' "${dev}/idVendor" -q; then
        continue
    fi
    if grep -v '6010' "${dev}/idProduct" -q; then
        continue
    fi
    manuf=$(cat "${dev}/manufacturer")
    product=$(cat "${dev}/product")
    serial=$(cat "${dev}/serial")
    # Look if any subdevices have a tty directory, this means that it is assigned a port.
    ttys=$(ls "${dev}:"*/tty* -d -1 | xargs -n 1 basename)
    echo "${dev}: ${manuf} ${product} serial: '${serial}', tty(s): $(for t in ${ttys}; do echo -n "${t}, "; done)"
done
