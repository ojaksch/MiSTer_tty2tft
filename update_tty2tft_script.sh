#!/bin/bash

# v1.8 - Copyright (c) 2021 ojaksch, venice

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# You can download the latest version of this script from:
# https://github.com/ojaksch/MiSTer_tty2tft



# Changelog:


! [ -e /media/fat/tty2tft/tty2tft-user.ini ] && touch /media/fat/tty2tft/tty2tft-user.ini
. /media/fat/tty2tft/tty2tft-system.ini
. /media/fat/tty2tft/tty2tft-user.ini


# Check for and create tty2tft script folder
[[ -d ${TTY2TFT_PATH} ]] && cd ${TTY2TFT_PATH} || mkdir ${TTY2TFT_PATH}


# Check and remount root writable if neccessary
if [ $(/bin/mount | head -n1 | grep -c "(ro,") = 1 ]; then
  /bin/mount -o remount,rw /
  MOUNTRO="true"
fi


# Check for and create tty2tft script folder
[[ -d ${TTY2TFT_PATH} ]] || mkdir ${TTY2TFT_PATH}

if [ ! -e ${USERSTARTUP} ] && [ -e /etc/init.d/S99user ]; then
  if [ -e ${USERSTARTUPTPL} ]; then
    echo "Copying ${USERSTARTUPTPL} to ${USERSTARTUP}"
    cp ${USERSTARTUPTPL} ${USERSTARTUP}
  else
    echo "Building ${USERSTARTUP}"
    echo -e "#!/bin/sh\n" > ${USERSTARTUP}
    echo -e 'echo "***" $1 "***"' >> ${USERSTARTUP}
  fi
fi
if [ $(grep -c "tty2tft" ${USERSTARTUP}) = "0" ]; then
  echo -e "Add tty2tft to ${USERSTARTUP}\n"
  echo -e "\n# Startup tty2tft" >> ${USERSTARTUP}
  echo -e "[[ -e ${INITSCRIPT} ]] && ${INITSCRIPT} \$1" >> ${USERSTARTUP}
fi


# init script
wget ${NODEBUG} "${REPOSITORY_URL}/S60tty2tft" -O /tmp/S60tty2tft
if  ! [ -f ${INITSCRIPT} ]; then
  if  [ -f ${INITDISABLED} ]; then
    echo -e "${fyellow}Found disabled init script, skipping Install${freset}"
  else
    echo -e "${fyellow}Installing init script ${fmagenta}S60tty2tft${freset}"
    mv -f /tmp/S60tty2tft ${INITSCRIPT}
    chmod +x ${INITSCRIPT}
  fi
elif ! cmp -s /tmp/S60tty2tft ${INITSCRIPT}; then
  if [ "${SCRIPT_UPDATE}" = "yes" ]; then
    echo -e "${fyellow}Updating init script ${fmagenta}S60tty2tft${freset}"
    mv -f /tmp/S60tty2tft ${INITSCRIPT}
    chmod +x ${INITSCRIPT}
  else
    echo -e "${fblink}Skipping${fyellow} available init script update because of the ${fcyan}SCRIPT_UPDATE${fyellow} INI-Option${freset}"
  fi
fi
[[ -f /tmp/S60tty2tft ]] && rm /tmp/S60tty2tft


# daemon
wget ${NODEBUG} "${REPOSITORY_URL}/${DAEMONNAME}" -O /tmp/${DAEMONNAME}
if  ! [ -f ${DAEMONSCRIPT} ]; then
  echo -e "${fyellow}Installing daemon script ${fmagenta}tty2tft${freset}"
  mv -f /tmp/${DAEMONNAME} ${DAEMONSCRIPT}
  chmod +x ${DAEMONSCRIPT}
elif ! cmp -s /tmp/${DAEMONNAME} ${DAEMONSCRIPT}; then
  if [ "${SCRIPT_UPDATE}" = "yes" ]; then
    echo -e "${fyellow}Updating daemon script ${fmagenta}tty2tft${freset}"
    mv -f /tmp/${DAEMONNAME} ${DAEMONSCRIPT}
    chmod +x ${DAEMONSCRIPT}
  else
    echo -e "${fblink}Skipping${fyellow} available daemon script update because of the ${fcyan}SCRIPT_UPDATE${fyellow} INI-Option${freset}"
  fi
fi
[[ -f /tmp/${DAEMONNAME} ]] && rm /tmp/${DAEMONNAME}


# UDEV rule
wget ${NODEBUG} "${REPOSITORY_URL}/80-ttyusb.rules.example" -O /tmp/80-ttyusb.rules.example
if ! cmp -s /tmp/80-ttyusb.rules.example ${TTY2TFT_PATH}/80-ttyusb.rules.example; then
  mv /tmp/80-ttyusb.rules.example ${TTY2TFT_PATH}/80-ttyusb.rules.example
fi
if [ -e ${TTY2TFT_PATH}/80-ttyusb.rules ] && ! [ -e /etc/udev/rules.d/80-ttyusb.rules ]; then
  cp -a ${TTY2TFT_PATH}/80-ttyusb.rules /etc/udev/rules.d/80-ttyusb.rules
fi

# MBC - https://github.com/pocomane/MiSTer_Batch_Control
[ ! -e /media/fat/Scripts/mbc ] && wget ${NODEBUG} ${REPOSITORY_URL}/mbc -O /media/fat/Scripts/mbc && chmod +x /media/fat/Scripts/mbc

# Download the installer to check esp firmware
cd /tmp
[ "${TTY2TFT_UPDATE}" = "yes" ] && bash <(wget -qO- ${REPOSITORY_URL}/installer.sh) UPDATER


# Check and remount root non-writable if neccessary
[ "${MOUNTRO}" = "true" ] && /bin/mount -o remount,ro /

if [ $(pidof ${DAEMONNAME}) ]; then
  echo -e "${fgreen}Restarting init script\n${freset}"
  ${INITSCRIPT} restart
elif [ -c "${TTYDEV}" ]; then
  echo -e "${fgreen}Starting init script\n${freset}"
  ${INITSCRIPT} start
fi

[ -z "${SSH_TTY}" ] && echo -e "${fgreen}Press any key to continue\n${freset}"
