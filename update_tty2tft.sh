#!/bin/bash

# v1.5 - Copyright (c) 2021 ojaksch, venice

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

freset="\e[0m\033[0m"
fblue="\e[1;34m"

REPOSITORY_URL="https://raw.githubusercontent.com/ojaksch/MiSTer_tty2tft/main"
#REPOSITORY_URL="https://raw.githubusercontent.com/ojaksch/MiSTer_tty2tft/main/Testing"    # Testing branch

SCRIPTNAME="/tmp/update_tty2tft_script.sh"
NODEBUG="-q -o /dev/null"

echo -e "\n +----------+";
echo -e " | ${fblue}tty2tft${freset} |---[]";
echo -e " +----------+\n";

check4error() {
  case "${1}" in
    0) ;;
    1) echo -e "${fyellow}wget: ${fred}Generic error code.${freset}" ;;
    2) echo -e "${fyellow}wget: ${fred}Parse error---for instance, when parsing command-line options, the .wgetrc or .netrc..." ;;
    3) echo -e "${fyellow}wget: ${fred}File I/O error.${freset}" ;;
    4) echo -e "${fyellow}wget: ${fred}Network failure.${freset}" ;;
    5) echo -e "${fyellow}wget: ${fred}SSL verification failure.${freset}" ;;
    6) echo -e "${fyellow}wget: ${fred}Username/password authentication failure.${freset}" ;;
    7) echo -e "${fyellow}wget: ${fred}Protocol errors.${freset}" ;;
    8) echo -e "${fyellow}wget: ${fred}Server issued an error response.${freset}" ;;
    *) echo -e "${fred}Unexpected and uncatched error.${freset}" ;;
  esac
  ! [ "${1}" = "0" ] && exit "${1}"
}

# Update the updater if neccessary
wget ${NODEBUG} --no-cache "${REPOSITORY_URL}/update_tty2tft.sh" -O /tmp/update_tty2tft.sh
check4error "${?}"
cmp -s /tmp/update_tty2tft.sh /media/fat/Scripts/update_tty2tft.sh
if [ "${?}" -gt "0" ] && [ -s /tmp/update_tty2tft.sh ]; then
    echo -e "${fyellow}Downloading Updater-Update ${fmagenta}${PICNAME}${freset}"
    mv -f /tmp/update_tty2tft.sh /media/fat/Scripts/update_tty2tft.sh
    exec /media/fat/Scripts/update_tty2tft.sh
    exit 255
else
    rm /tmp/update_tty2tft.sh
fi

# Check and update INI files if neccessary
wget ${NODEBUG} --no-cache "${REPOSITORY_URL}/tty2tft-system.ini" -O /tmp/tty2tft-system.ini
check4error "${?}"
. /tmp/tty2tft-system.ini
[[ -d "${TTY2TFT_PATH}" ]] || mkdir "${TTY2TFT_PATH}"
cmp -s /tmp/tty2tft-system.ini "${TTY2TFT_PATH}/tty2tft-system.ini"
if [ "${?}" -gt "0" ]; then
    mv /tmp/tty2tft-system.ini "${TTY2TFT_PATH}/tty2tft-system.ini"
    . "${TTY2TFT_PATH}/tty2tft-system.ini"
fi

! [ -e /media/fat/tty2tft/tty2tft-user.ini ] && touch /media/fat/tty2tft/tty2tft-user.ini

[ -f "${TTY2TFT_PATH}/tty2tft.ini" ] && mv "${TTY2TFT_PATH}/tty2tft.ini" "${TTY2TFT_PATH}/tty2tft.ini.bak"
[ -f "/media/fat/Scripts/tty2tft.ini" ] && mv "/media/fat/Scripts/tty2tft.ini" "${TTY2TFT_PATH}/tty2tft-user.ini.bak"

wget ${NODEBUG} --no-cache "${REPOSITORY_URL}/update_tty2tft_script.sh" -O "${SCRIPTNAME}"
check4error "${?}"
[ -s "${SCRIPTNAME}" ] && bash "${SCRIPTNAME}" "${1}"
[ -f "${SCRIPTNAME}" ] && rm "${SCRIPTNAME}"

exit 0
