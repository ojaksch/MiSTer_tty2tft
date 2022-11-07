#!/bin/bash

REPOSITORY_URL1="https://raw.githubusercontent.com/ojaksch/MiSTer_tty2tft/main"
REPOSITORY_URL2="https://www.tty2tft.de/MiSTer_tty2tft-installer"
DBAUD="921600"
DSTD="--before default_reset --after hard_reset write_flash --compress --flash_mode dio --flash_freq 80m --flash_size detect"
TMPDIR=$(mktemp -d)
cd ${TMPDIR}

flashesp() {
  echo "------------------------------------------------------------------------"
  case "${MCUtype}" in
    TFTESP32)
      if [ "${1}" = "stage1" ]; then	# Flash GFX Demo/Test
	wget -q ${REPOSITORY_URL2}/boot_app0.bin ${REPOSITORY_URL2}/bootloader_dio_80m.bin ${REPOSITORY_URL2}/partitions.bin ${REPOSITORY_URL2}/${2}
	${TMPDIR}/esptool.py --chip esp32 --port ${TTYDEV} --baud ${DBAUD} ${DSTD} 0xe000 ${TMPDIR}/boot_app0.bin 0x1000 ${TMPDIR}/bootloader_dio_80m.bin 0x10000 ${TMPDIR}/${2} 0x8000 ${TMPDIR}/partitions.bin
      fi
      if [ "${1}" = "stage2" ]; then	# Flash tty2tft
	if [ "${TTY2TFT_UPDATE_BY}" = "WEB" ]; then
	  echo "CMDOTAUPDATE,${TTY2TFT_DISPLAY}" > ${TTYDEV}
	else
	  wget -q --content-disposition ${REPOSITORY_URL2}/MAC.html?${MAC} ${REPOSITORY_URL2}/boot_app0.bin ${REPOSITORY_URL2}/bootloader_dio_80m.bin ${REPOSITORY_URL2}/partitions.bin ${REPOSITORY_URL2}/esp32de_${BUILDVER}_${TTY2TFT_DISPLAY}.bin?mac=${MAC}
	  if ! [ -e esp32de_${BUILDVER}_${TTY2TFT_DISPLAY}.bin ]; then
	    echo -e "${fred}Unfortunately something went wrong!${freset}"
	    echo -e "${fred}Possible causes:${freset}"
	    echo -e "${fred}- Missing files, busy server, drunken Webmaster. Call forum.${freset}"
	    echo -e "${fred}\nMAC address: ${MAC}${freset}"
	    exit 255
	  fi
	  ${TMPDIR}/esptool.py --chip esp32 --port ${TTYDEV} --baud ${DBAUD} ${DSTD} 0xe000 ${TMPDIR}/boot_app0.bin 0x1000 ${TMPDIR}/bootloader_dio_80m.bin 0x10000 ${TMPDIR}/esp32de_${BUILDVER}_${TTY2TFT_DISPLAY}.bin 0x8000 ${TMPDIR}/partitions.bin
	fi
      fi
    ;;
  esac
  echo "------------------------------------------------------------------------"
  echo -en "${fyellow}${fblink}...waiting for reboot of device...${freset}" ; sleep 4 ; echo -e "\033[2K"
  stty -F ${TTYDEV} ${BAUDRATE} ${TTYPARAM} ; sleep 1
  echo -e "\n${fgreen}Flash progress completed. Have fun!${freset}"
}

checkesp() {
  MAC=$(${TMPDIR}/esptool.py --chip esp32 --port ${TTYDEV} --baud ${DBAUD} --after no_reset flash_id | grep MAC | awk '{print $2}')
  MAC=${MAC^^}
}

# If there's an existing ini, use it
if [ -r /media/fat/tty2tft/tty2tft-system.ini ]; then
  . /media/fat/tty2tft/tty2tft-system.ini
else
  wget -q ${REPOSITORY_URL1}/tty2tft-system.ini -O ${TMPDIR}/tty2tft-system.ini
  . ${TMPDIR}/tty2tft-system.ini
fi
if [ -r /media/fat/tty2tft/tty2tft-user.ini ]; then
  . /media/fat/tty2tft/tty2tft-user.ini
else
  wget -q ${REPOSITORY_URL1}/tty2tft-user.ini -O ${TMPDIR}/tty2tft-user.ini
  . ${TMPDIR}/tty2tft-user.ini
fi

# When started with parameter "T" use testing sketch
[ "${TTY2TFT_FW_TESTING}" = "yes" ] && BUILDVER=$(wget -q ${REPOSITORY_URL2}/buildverT -O -) || BUILDVER=$(wget -q ${REPOSITORY_URL2}/buildver -O -)

# Stop an already running daemon
if [ $(pidof ${DAEMONNAME}) ] && [ -f ${INITSCRIPT} ] ; then
  ${INITSCRIPT} stop
  INITSTOPPED="yes"
  sleep 0.5
fi

# Install pySerial (if it is missing)
if ! python -c "import serial" &> /dev/null; then
  ! [ -f /lib/python3.9/site-packages/pyserial-3.5-py3.9.egg ] && wget -q ${REPOSITORY_URL2}/pyserial-3.5-py3.9.egg -O /lib/python3.9/site-packages/pyserial-3.5-py3.9.egg
  echo "./pyserial-3.5-py3.9.egg" >> /lib/python3.9/site-packages/easy-install.pth
fi

# Install esptool (if it is missing)
if ! [ -f ${TMPDIR}/esptool.py ]; then
  wget -q ${REPOSITORY_URL2}/esptool.py -O ${TMPDIR}/esptool.py
  chmod +x ${TMPDIR}/esptool.py
fi

# Check if interface tty* device is present
echo -en "${freset}Checking for device at ${TTYDEV}${freset}: "
if [[ -c ${TTYDEV} ]]; then
  stty -F ${TTYDEV} ${BAUDRATE} ${TTYPARAM}
  echo -e "${fgreen}available${freset}"

  echo -en "${freset}Trying to identify device... ${freset}"
  echo "CMDHWINF" > ${TTYDEV} ; read -t5 BLA < ${TTYDEV}
  BLA=$(echo $BLA | sed "s/ttyack;//g")
  if [[ ${?} > 1 ]] && [ -z "${BLA}" ]; then
    MCUtype="TFTESP32"
    SWver="0"
    checkesp
  else
    MCUtype="${BLA:0:8}"
    SWver="${BLA:9:6}"
    MAC="${BLA:16:17}"
    if [ "${MCUtype:0:2}" = "HW" ]; then
      echo -e "${fgreen}${MCUtype} with sketch version ${SWver}${freset}"
    fi
  fi
else
  echo -e "${fred}Could not connect to interface. Please check USB connection and run script again.${freset}"
  exit 1
fi

# Check for MCU
case "${MCUtype}" in
  Exit)	exit 0 ;;
  TFTESP32)	echo -e "${fyellow}ESP32 selected/detected.${freset}" ;;
esac

if [ "${1}" = "IDENTIFY" ]; then
  #echo -e "${fred}Trying to identify device...${freset}"
  echo -e "${fcyan}MAC address is: ${MAC}${freset}"
  exit 0
fi

if [ "${1}" = "FORCE" ]; then
  MCUtype="TFTESP32"
  MAC=$(${TMPDIR}/esptool.py --chip esp32 --port ${TTYDEV} read_mac | grep "MAC: " | tail -n1 | awk '{print $2}')
  if ! [ -z "${3}" ]; then BUILDVER="${3}"; fi
    echo -e "${fred}FORCED UPDATE${freset}"
    echo -e "${fyellow}Version of your tty2tft device is ${fblue}${SWver}${fyellow}, forced BUILDVER is ${fgreen}${BUILDVER}${fyellow}.${freset}"
    echo -e "${fyellow}MCUtype is set to ${fblue}${MCUtype}${freset}"
    if ! [ -z "${2}" ]; then
      TTY2TFT_DISPLAY="${2}"
      flashesp stage2 "${TTY2TFT_DISPLAY}"
    else
      flashesp stage2
    fi
    [ "${INITSTOPPED}" = "yes" ] && ${INITSCRIPT} start
  elif [ "${1}" = "TEST-ILI9341" ]; then
    echo -e "${fred}1st Test of your setup${freset}"
    flashesp stage1 gfx-test_ILI9341.bin
  elif [ "${1}" = "TEST-ILI9341SPI" ]; then
    echo -e "${fred}1st Test of your setup${freset}"
    flashesp stage1 gfx-test_ILI9341SPI.bin
  elif [ "${1}" = "TEST-ILI9486" ]; then
    echo -e "${fred}1st Test of your setup${freset}"
    flashesp stage1 gfx-test_ILI9486.bin
  elif [ "${1}" = "TEST-ILI9488" ]; then
    echo -e "${fred}1st Test of your setup${freset}"
    flashesp stage1 gfx-test_ILI9488.bin
  else
    if [[ "${SWver}" < "${BUILDVER}" ]]; then
      echo -e "${fyellow}Version of your tty2tft device is ${fblue}${SWver}${fyellow}, but BUILDVER is ${fgreen}${BUILDVER}${fyellow}.${freset}"
      echo -en "${fyellow}Do you want to Update? Use Cursor or Joystick for ${fgreen}YES=UP${freset} / ${fred}NO=DOWN${fyellow}. Countdown: 9${freset}"
      yesno 9
      if [ "${KEY}" = "y" ]; then
	echo "Updating tty2tft" > ${TTYDEV}
	flashesp stage2
      fi
  elif [[ "${SWver}" > "${BUILDVER}" ]]; then
    if [ "${SWver: -1}" = "T" ]; then
      echo -e "${fyellow}Your version ${fblue}${SWver}${fyellow} is newer than the available stable build ${fgreen}${BUILDVER}${fyellow}!${freset}"
      echo -en "${fyellow}Do you want to Downgrade? Use Cursor or Joystick for ${fgreen}YES=UP${freset} / ${fred}NO=DOWN${fyellow}. Countdown: 9${freset}"
      yesno 9
      if [ "${KEY}" = "y" ]; then
	echo "Downgrading tty2tft" > ${TTYDEV}
	flashesp stage2
      fi
    fi
      [ "${INITSTOPPED}" = "yes" ] && ! [ "${1}" = "UPDATER" ] && ${INITSCRIPT} start
  elif [[ "${SWver}" = "${BUILDVER}" ]]; then
    echo -e "${fyellow}Good job, your hardware is up-to-date!${freset}"
    [ "${INITSTOPPED}" = "yes" ] && ! [ "${1}" = "UPDATER" ] && ${INITSCRIPT} start
  fi
fi

rm -rf "${TMPDIR}"
exit 0
