#!/bin/bash

# Based on iot-lab_M3 debug.sh script.
# Author: Jonas Remmert j.remmert@phytec.de

elffile=`echo $1 | sed 's/.hex/.elf/'`

echo The file: $elffile will be flashed to the board: $BOARD .
echo Press y to proceed, v to proceed with verify or any other key to abort.
read x

retval=$(arm-none-eabi-readelf -x .fcfield $elffile  | awk '/0x00000400/ {print $2 $3 $4 $5}')
unlocked_fcfield="fffffffffffffffffffffffffeffffff"

if [ "$retval" != "$unlocked_fcfield" ]; then
echo "Hexdump, .fcfield of $elffile"
retval=$(arm-none-eabi-readelf -x .fcfield $elffile)
echo $retval
echo "Fcfield not fitting to MKW22Dxxx, danger of bricking the device during flash...[abort]"
exit 0
fi
echo "Flash configuration tested... [OK]"


# flash without verify
if [ $x = "y" ] || [ $x = "v" ];# -o $x -eq v ];
then
echo "Flashing device, do not disconnect or reset the target until this script exits!!!"
openocd -f "../../boards/pba-d-01-kw2x/dist/mkw22d512.cfg" \
	-c "init" \
	-c "reset halt" \
        -c "flash write_image erase $elffile" \
	-c "reset run" \
	-c "exit"
fi

# verify
if [ "$x" = "v" ];
then
echo "Flashing device in 1s, do not disconnect or reset the target until this script exits!!!"
openocd -f "../../boards/pba-d-01-kw2x/dist/mkw22d512.cfg" \
        -c "init" \
        -c "reset halt" \
        -c "verify_image $elffile" \
        -c "reset run" \
        -c "exit"
fi
echo done...
