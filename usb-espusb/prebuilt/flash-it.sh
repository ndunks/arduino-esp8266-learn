#!/bin/bash
# Change to fit your system
ESPTOOL="esptool"
set -e
$ESPTOOL -b 2000000 --port /dev/ttyUSB0 write_flash -fm dio 0x00000 image.elf-0x00000.bin 0x40000 image.elf-0x40000.bin
$ESPTOOL -b 2000000 --port /dev/ttyUSB0 write_flash 65536 web/page.mpfs
