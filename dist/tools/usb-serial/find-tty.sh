#!/bin/bash

#
# Copyright (C) 2015 Eistec AB
#
# This file is subject to the terms and conditions of the GNU Lesser General
# Public License v2.1. See the file LICENSE in the top level directory for more
# details.
#

# Find all USB to serial devices
for dev in /sys/bus/usb/devices/*; do
    if [ ! -f "${dev}/idVendor" ]; then
        # not a main device
        continue
    fi
    # Try to read the device serial, discard any error messages (2>/dev/null)
    serial=$(cat "${dev}/serial" 2>/dev/null)
    # Look if any subdevices have a tty directory, this means that it is assigned a port.
    unset ttys
    ttys=$( (ls "${dev}:"*/tty* -d -1 2>/dev/null | xargs -n 1 basename) 2>/dev/null)
    if [ -z "${ttys}" ]; then
        continue
    fi
    # split results into array
    read -r -a ttys <<< ${ttys}
    if [ $# -lt 1 ]; then
        # No arguments given, return first found tty
        echo "/dev/${ttys[0]}"
        exit 0
    fi
    # else: Match any of the given serials
    for s in "${@}"; do
        echo "${serial}" | egrep -e "${s}" -q
        if [ $? -eq 0 ]; then
            # return first tty
            echo "/dev/${ttys[0]}"
            exit 0
        fi
    done
done
# not found
exit 1;
