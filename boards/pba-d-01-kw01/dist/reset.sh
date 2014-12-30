#!/bin/bash

openocd -f "../../boards/pba-d-01-kw01/dist/mkw01z128.cfg" \
    -c "init" \
    -c "reset run" \
    -c "shutdown"
