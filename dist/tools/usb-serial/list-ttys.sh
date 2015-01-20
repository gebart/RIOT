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

# Find all USB to serial devices
for dev in /sys/bus/usb/devices/[0-9]*; do
    if [ ! -f "${dev}/idVendor" ]; then
        # not a main device
        continue
    fi
    # Try to read the device info, discard any error messages (2>/dev/null)
    serial=$(cat "${dev}/serial" 2>/dev/null)
    manuf=$(cat "${dev}/manufacturer" 2>/dev/null)
    product=$(cat "${dev}/product" 2>/dev/null)
    # Look if any subdevices have a tty directory, this means that it is
    # assigned a port.
    # Note: limit the depth to avoid duplicate tty devices below USB hubs.
    # Each 'tty' directory contains a subdirectory named after the tty device
    # e.g. /sys/bus/usb/devices/1-1/1-1:1.0/ttyUSB0/tty/ttyUSB0
    ttys=$( (find "${dev}"/ -maxdepth 3 -type d -name 'tty' -exec ls -1 {} \; | grep tty ) 2>/dev/null )
    # If at least one tty is assigned to the device we will write its info to stdout.
    if [ ! -z "${ttys}" ]; then
        echo "${dev}: ${manuf} ${product} serial: '${serial}', tty(s): $(for t in ${ttys}; do echo -n "${t}, "; done)"
    fi
done
