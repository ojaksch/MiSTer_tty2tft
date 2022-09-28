#!/bin/bash

if [ -z ${1} ]; then
  echo "Missing parameter. You have to enter the binary's filename to download."
  echo "Exiting."
  exit 255
fi

TTYDEV="/dev/ttyUSB0"
DBAUD="921600"
DSTD="--before default_reset --after hard_reset write_flash --compress --flash_mode dio --flash_freq 80m --flash_size detect"
REPOSITORY_URL1="https://raw.githubusercontent.com/ojaksch/MiSTer_tty2tft/main"
REPOSITORY_URL2="https://www.tty2tft.de/MiSTer_tty2tft-installer"

# Install pySerial (if it is missing)
if ! python -c "import serial" &> /dev/null; then
  ! [ -f /lib/python3.9/site-packages/pyserial-3.5-py3.9.egg ] && wget -q ${REPOSITORY_URL2}/pyserial-3.5-py3.9.egg -O /lib/python3.9/site-packages/pyserial-3.5-py3.9.egg
  echo "./pyserial-3.5-py3.9.egg" >> /lib/python3.9/site-packages/easy-install.pth
fi

wget -Nq ${REPOSITORY_URL2}/boot_app0.bin ${REPOSITORY_URL2}/bootloader_dio_80m.bin ${REPOSITORY_URL2}/partitions.bin ${REPOSITORY_URL2}/esptool.py ${REPOSITORY_URL2}/${1}
python esptool.py --chip esp32 --port ${TTYDEV} --baud ${DBAUD} ${DSTD} 0xe000 boot_app0.bin 0x1000 bootloader_dio_80m.bin 0x10000 ${1} 0x8000 partitions.bin
