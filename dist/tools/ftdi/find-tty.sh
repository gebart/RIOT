#!/bin/bash

#
# Copyright (C) 2015 Eistec AB
#
# This file is subject to the terms and conditions of the GNU Lesser General
# Public License v2.1. See the file LICENSE in the top level directory for more
# details.
#

if [ $# -lt 1 ]; then
    cat <<EOF
Usage: $(basename "${0}") [serial]
 The first tty device node assigned to the USB device with the given serial number is written to stdout.
EOF
    exit 1
fi

# Find all USB to serial devices
for dev in /sys/bus/usb/devices/*; do
    if [ ! -f "${dev}/idVendor" ]; then
        # not a main device
        continue
    fi
    serial=$(cat "${dev}/serial")
    # Look if any subdevices have a tty directory, this means that it is assigned a port.
    unset ttys
    ttys=$(ls "${dev}:"*/tty* -d -1 | xargs -n 1 basename)
    if [ -z "${ttys}" ]; then
        continue
    fi
    # split results into array
    read -r -a ttys <<< ${ttys}
    for s in "${@}"; do
        if [ "${serial}" == "${s}" ]; then
            # return first tty
            echo "/dev/${ttys[0]}"
            exit 0;
        fi
    done
done
# not found
exit 1;
