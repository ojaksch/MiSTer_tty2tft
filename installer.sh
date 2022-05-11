#!/bin/bash

REPOSITORY_URL1="https://raw.githubusercontent.com/ojaksch/MiSTer_tty2tft/main"
REPOSITORY_URL2="https://www.tty2tft.de/MiSTer_tty2tft-installer"
DBAUD="921600"
DSTD="--before default_reset --after hard_reset write_flash --compress --flash_size detect"
TMPDIR=$(mktemp -d)
cd ${TMPDIR}

flashesp() {
    echo "------------------------------------------------------------------------"
    case "${MCUtype}" in
	HWESP32DE)
	    wget -q ${REPOSITORY_URL2}/boot_app0.bin ${REPOSITORY_URL2}/bootloader_dio_80m.bin ${REPOSITORY_URL2}/partitions.bin ${REPOSITORY_URL2}/${MAC}/esp32de_${BUILDVER}.bin
	    if ! [ -e esp32de_${BUILDVER}.bin ]; then
		echo -e "${fred}Unfortunately something went wrong!${freset}"
		echo -e "${fred}Possible causes:${freset}"
		echo -e "${fred}- Missing files, busy server, drunken Webmaster. Call forum.${freset}"
		echo -e "${fred}- You aren't a registered user. Call forum and register.${freset}"
		echo -e "${fred}\nMAC address: ${MAC}${freset}"
		exit 255
	    fi
	    ${TMPDIR}/esptool.py --chip esp32 --port ${TTYDEV} --baud ${DBAUD} ${DSTD} 0xe000 ${TMPDIR}/boot_app0.bin 0x1000 ${TMPDIR}/bootloader_dio_80m.bin 0x10000 ${TMPDIR}/esp32de_${BUILDVER}.bin 0x8000 ${TMPDIR}/partitions.bin
	    ;;
    esac
    echo "------------------------------------------------------------------------"
    echo -en "${fyellow}${fblink}...waiting for reboot of device...${freset}" ; sleep 4 ; echo -e "\033[2K"
    stty -F ${TTYDEV} ${BAUDRATE} ${TTYPARAM} ; sleep 1
    echo -e "\n${fgreen}Flash progress completed. Have fun!${freset}"
}

checkesp() {
    MAC=$(${TMPDIR}/esptool.py --chip esp32 --port ${TTYDEV} --baud ${DBAUD} flash_id | grep MAC | awk '{print $2}')
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
[ "${TTY2OLED_FW_TESTING}" = "yes" ] && BUILDVER=$(wget -q ${REPOSITORY_URL2}/buildverT -O -) || BUILDVER=$(wget -q ${REPOSITORY_URL2}/buildver -O -)

# Clear the display by setting this as CORENAME, which keeps the display while updating
echo "cls" > /tmp/CORENAME

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

# Check if interface ttyUSB0 is present
echo -en "${freset}Checking for device at ${TTYDEV}${freset}: "
if [[ -c ${TTYDEV} ]]; then
    stty -F ${TTYDEV} ${BAUDRATE} ${TTYPARAM}
    echo -e "${fgreen}available${freset}"

    echo -en "${freset}Trying to identify device... ${freset}"
    echo "CMDHWINF" > ${TTYDEV} ; read -t5 BLA < ${TTYDEV}
    if [[ ${?} > 1 ]] && [ -z "${BLA}" ]; then
	MCUtype="HWESP32DE"
	SWver="0"
	checkesp
    else
	MCUtype="${BLA:0:9}"
	SWver="${BLA:10:6}"
	MAC="${BLA:17:17}"
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
    HWESP32DE)	echo -e "${fyellow}ESP32 selected/detected.${freset}" ;;
esac

if [ "${1}" = "IDENTIFY" ]; then
    echo -e "${fred}Trying to identify device...${freset}"
    echo "MAC address is: ${MAC}"
    exit 0
fi

if [ "${1}" = "FORCE" ]; then
    echo -e "${fred}FORCED UPDATE${freset}"
    echo -e "${fyellow}Version of your tty2oled device is ${fblue}${SWver}${fyellow}, forced BUILDVER is ${fgreen}${BUILDVER}${fyellow}.${freset}"
    echo -e "${fyellow}MCUtype is set to ${fblue}${MCUtype}${freset}"
    flashesp
    [ "${INITSTOPPED}" = "yes" ] && ${INITSCRIPT} start
else
    if [[ "${SWver}" < "${BUILDVER}" ]]; then
	echo -e "${fyellow}Version of your tty2tft device is ${fblue}${SWver}${fyellow}, but BUILDVER is ${fgreen}${BUILDVER}${fyellow}.${freset}"
	echo -en "${fyellow}Do you want to Update? Use Cursor or Joystick for ${fgreen}YES=UP${freset} / ${fred}NO=DOWN${fyellow}. Countdown: 9${freset}"
	yesno 9
	if [ "${KEY}" = "y" ]; then
	    echo "Updating tty2tft" > /dev/ttyUSB0
	    flashesp
	fi
    elif [[ "${SWver}" > "${BUILDVER}" ]]; then
	if [ "${SWver: -1}" = "T" ]; then
	    echo -e "${fyellow}Your version ${fblue}${SWver}${fyellow} is newer than the available stable build ${fgreen}${BUILDVER}${fyellow}!${freset}"
	    echo -en "${fyellow}Do you want to Downgrade? Use Cursor or Joystick for ${fgreen}YES=UP${freset} / ${fred}NO=DOWN${fyellow}. Countdown: 9${freset}"
	    yesno 9
	    if [ "${KEY}" = "y" ]; then
		echo "Downgrading tty2tft" > /dev/ttyUSB0
		flashesp
	    fi
	fi
	[ "${INITSTOPPED}" = "yes" ] && ! [ "${1}" = "UPDATER" ] && ${INITSCRIPT} start
    elif [[ "${SWver}" = "${BUILDVER}" ]]; then
	echo -e "${fyellow}Good boy, your hardware is up-to-date!${freset}"
	[ "${INITSTOPPED}" = "yes" ] && ! [ "${1}" = "UPDATER" ] && ${INITSCRIPT} start
    fi
fi
echo "MENU" > /tmp/CORENAME

rm -rf ${TMPDIR}
exit 0
