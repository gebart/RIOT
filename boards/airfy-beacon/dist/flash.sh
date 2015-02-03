#!/bin/sh

openocd -f "${RIOTBOARD}/${BOARD}/dist/openocd.cfg" \
    -c "tcl_port 0" \
    -c "telnet_port 0" \
    -c "gdb_port 0" \
    -c "init" \
    -c "targets" \
    -c "reset halt" \
    -c "program ${HEXFILE} 0 verify" \
    -c "reset run"
