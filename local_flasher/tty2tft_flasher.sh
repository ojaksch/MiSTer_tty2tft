#!/bin/bash

DBAUD="921600"
DSTD="--before default_reset --after hard_reset write_flash --compress --flash_mode dio --flash_freq 80m --flash_size detect"
REPOSITORY_URL2="https://www.tty2tft.de/MiSTer_tty2tft-installer"

# Check for existing setup
if [ -e /media/fat/tty2tft/tty2tft-system.ini ]; then
  source /media/fat/tty2tft/tty2tft-system.ini
  [ -e /media/fat/tty2tft/tty2tft-user.ini ] && source /media/fat/tty2tft/tty2tft-user.ini
else
  echo -e "\e[1;31mNo installation of tty2tft found. Exiting.\e[0m"
  exit 1
fi

# Install pySerial (if it is missing)
if ! python -c "import serial" &> /dev/null; then
  ! [ -f /lib/python3.9/site-packages/pyserial-3.5-py3.9.egg ] && wget -q ${REPOSITORY_URL2}/pyserial-3.5-py3.9.egg -O /lib/python3.9/site-packages/pyserial-3.5-py3.9.egg
  echo "./pyserial-3.5-py3.9.egg" >> /lib/python3.9/site-packages/easy-install.pth
fi

if [ -z "${1}" ]; then
  exec 3>&1
  FLASHTYPE=$(dialog --clear --no-cancel --ascii-lines --no-tags \
    --backtitle "tty2tft" --title "[ Flash tool for ESP devices ]" \
    --menu "Use the arrow keys and enter \nor the d-pad and A button" 0 0 0 \
    STABLE "Flash latest stable" \
    FILE "Local file to flash (MiSTer_tty2tft.bin)" \
    Exit "Exit now" 2>&1 1>&3)
  exec 3>&-;
  clear
else
  FLASHFILE="${1}"
fi

case ${FLASHTYPE} in
  STABLE) 
    echo -e "\e[1;33mFetching files...\e[0m"
    BUILDVER=$(wget -q https://www.tty2tft.de/MiSTer_tty2tft-installer/buildver -O -)
    wget -Nq ${REPOSITORY_URL2}/esptool.py
    MAC=$(python ./esptool.py --chip esp32 --port ${TTYDEV} --baud ${DBAUD} --after no_reset flash_id | grep MAC | awk '{print $2}')
    wget -Nq --content-disposition ${REPOSITORY_URL2}/MAC.html?${MAC} ${REPOSITORY_URL2}/boot_app0.bin ${REPOSITORY_URL2}/bootloader_dio_80m.bin ${REPOSITORY_URL2}/partitions.bin ${REPOSITORY_URL2}/esp32de_${BUILDVER}_${TTY2TFT_DISPLAY}.bin?mac=${MAC}
    rm MAC.html*
    FLASHFILE="esp32de_${BUILDVER}_${TTY2TFT_DISPLAY}.bin"
    ;;
  FILE)
    FLASHFILE="MiSTer_tty2tft.bin"
    ;;
  Exit) exit 0 ;;
esac

if ! [ -e ${FLASHFILE} ]; then
  echo -e "\e[1;31mFile to flash \e[1;32m(${FLASHFILE}) \e[1;31mmissing!.\e[0m"
  echo -e "\e[1;31mSomething unexpected went wrong. Exiting.\e[0m"
  exit 2
else
  echo -e "\e[1;33mFlashing file ${FLASHFILE} ...\e[0m"
  python ./esptool.py --chip esp32 --port ${TTYDEV} --baud ${DBAUD} ${DSTD} 0xe000 boot_app0.bin 0x1000 bootloader_dio_80m.bin 0x10000 ${FLASHFILE} 0x8000 partitions.bin
  read -p "Press Enter key to exit."
fi
