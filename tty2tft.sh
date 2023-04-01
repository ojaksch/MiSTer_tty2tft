#!/bin/bash

# By venice & ojaksch
#
#
#

. /media/fat/tty2tft/tty2tft-system.ini
. /media/fat/tty2tft/tty2tft-user.ini
cd /tmp

# Debug function
dbug() {
  if [ "${debug}" = "true" ]; then
    echo "${1}"
    if [ ! -e ${debugfile} ]; then						# log file not (!) exists (-e) create it
      echo "---------- tty2tft Debuglog ----------" > ${debugfile}
    fi 
    echo "${1}" >> ${debugfile}							# output debug text
  fi
}

# Rotate Display function
sendrotation() {
  if [ "${ROTATE}" = "yes" ]; then
    dbug "Sending: CMDROT,3"
    echo "CMDROT,3" > ${TTYDEV}							# Send Rotation if set to "yes"
    sleep ${WAITSECS}
  fi
}

sendtime() {
  timeoffset=$(date +%:::z)
  localtime=$(date '-d now '${timeoffset}' hour' +%s)
  echo "CMDSETTIME,${localtime}" > ${TTYDEV}
}

setvideoplay() {
  dbug "Sending: CMDVIDEO,${VIDEOPLAY}"
  echo "CMDVIDEOPLAY,${VIDEOPLAY}" > ${TTYDEV}					# Set playing of videos or not
}

setscreensaver() {
  if [ "${SCREENSAVER}" = "yes" ]; then
    dbug "Sending: CMDSAVER,${SCREENSAVER_START},${SCREENSAVER_IVAL}"
    echo "CMDSAVER,${SCREENSAVER_START},${SCREENSAVER_IVAL}" > ${TTYDEV}	# Set screensaver
    sleep 0.02
    echo "CMDSAVEROPTS,${SCREENSAVER_AMPM},${SCREENSAVER_CLOCK},${SCREENSAVER_TEXT},${SCREENSAVER_PICT},${SCREENSAVER_MOVE},${SCREENSAVER_TYPE},${SCREENSAVER_DOTCOL}" > ${TTYDEV}		# Set screensaver options
  else
    dbug "Sending: CMDSAVER,0,0"
    echo "CMDSAVER,0,0" > ${TTYDEV}						# Disable screensaver
  fi
}

checkbackchannel() {
  read -t 0.5 BACKCHANNEL < ${TTYDEV}
  BACKCHANNEL=$(echo ${BACKCHANNEL} | sed 's/ttyack;//g')
  # buttons are: RESET, USER, OSD
  # OSD: mbc raw_seq M
  # exit to menu: mbc raw_seq MRUUO
  # reset core/USER: mbc raw_seq MUUO
  case "${BACKCHANNEL}" in
    "touchpressed;button1;")
      ${TOUCHBUTTON01}
      ;;
    "touchpressed;button2;")
      ${TOUCHBUTTON02}
      ;;
    "touchpressed;button3;")
      ${TOUCHBUTTON03}
      ;;
    "touchpressed;button4;")
      ${TOUCHBUTTON04}
      ;;
    "touchpressed;button5;")
      ${TOUCHBUTTON05}
      ;;
    "touchpressed;button6;")
      ${TOUCHBUTTON06}
      ;;
    "touchpressed;button7;")
      ${TOUCHBUTTON07}
      ;;
    "touchpressed;button8;")
      sysinfotop="$(top -n 1 | sed -n '2p' | awk '{print $2}')"
      sysinfofree="$(free -m | sed -n '2p' | awk '{print $4}')MB"
      sysinfodf1="$(df -h /media/fat | sed -n '2p' | awk '{print $4}')B"
      ! [ -z "$(grep -m1 "cifs" /etc/mtab)" ] && sysinfodf2="$(df -h -t cifs | sed -n '2p' | awk '{print $4}')B free" || sysinfodf2="n/a"
      ! [ -z "$(grep -m1 "nfs4" /etc/mtab)" ] && sysinfodf3="$(df -h -t nfs4 | sed -n '2p' | awk '{print $4}')B free" || sysinfodf3="n/a"
      sysinfotemp="n/a"
      sysinfoeth0="$(ip -4 -o addr s eth0 | awk '!/^[0-9]*: ?lo|link\/ether/ {gsub("/", " "); print $4}')"
      sysinfowlan0="$(ip -4 -o addr s wlan0 | awk '!/^[0-9]*: ?lo|link\/ether/ {gsub("/", " "); print $4}')"
      sysinfocore="$(cat /tmp/CORENAME)"
      let sysinfocpuclk=$(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq)/1000
      [ -z "${sysinfoeth0}" ] && sysinfoeth0="n/a"
      [ -z "${sysinfowlan0}" ] && sysinfowlan0="n/a"
      #echo "CMDSYSINFO,${sysinfotop},${sysinfofree},${sysinfodf},${sysinfotemp},${sysinfoeth0},${sysinfowlan0},${sysinfocore}" > ${TTYDEV}
      echo "CMDCLS" > ${TTYDEV}
      writetext "3,2016,0,10,20,MiSTer Sys Info"
      [ "${SCREENSAVER_AMPM}" = "yes" ] && writetext "2,65535,0,10,40,Time: $(date +%r)      Date: $(date +%F)"
      [ "${SCREENSAVER_AMPM}" = "no" ] && writetext "2,65535,0,10,40,Time: $(date +%R)      Date: $(date +%F)"
      writetext "2,65535,0,10,55,CPU: ${sysinfotop} busy"
      writetext "2,65535,0,10,70,RAM: ${sysinfofree} free"
      writetext "2,65535,0,10,85,SD: ${sysinfodf1} free"
      writetext "2,65535,0,10,100,CIFS: ${sysinfodf2}"
      writetext "2,65535,0,10,115,NFS: ${sysinfodf3}"
      writetext "2,65535,0,10,130,TEMP: ${sysinfotemp}"
      writetext "2,65535,0,10,145,IP eth0: ${sysinfoeth0}"
      writetext "2,65535,0,10,160,IP wlan0: ${sysinfowlan0}"
      writetext "2,65535,0,10,175,CORE: ${sysinfocore}"
      [ -e /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor ] && writetext "2,65535,0,10,190,CPU clock: ${sysinfocpuclk} MHz"
      ;;
    "touchpressed;button9;")
      ${TOUCHBUTTON09}
      ;;
    "touchpressed;button10;")
      ${TOUCHBUTTON10}
      ;;
    "touchpressed;button11;")
      ${TOUCHBUTTON11}
      ;;
    "touchpressed;button12;")
      ${TOUCHBUTTON12}
      ;;
    "touchpressed;button13;")
      ${TOUCHBUTTON13}
      ;;
    "touchpressed;button14;")
      ${TOUCHBUTTON14}
      ;;
    "touchpressed;button15;")
      ${TOUCHBUTTON15}
      ;;
    "touchpressed;button16;")
      ${TOUCHBUTTON16}
      ;;
    "touchpressed;button17;")
      ${TOUCHBUTTON17}
      ;;
    "touchpressed;button18;")
      ${TOUCHBUTTON18}
      ;;
    "touchpressed;button19;")
      ${TOUCHBUTTON19}
      ;;
    "touchpressed;button20;")
      ${TOUCHBUTTON20}
      ;;
    "touchpressed;button21;")
      ${TOUCHBUTTON21}
      ;;
    "touchpressed;button22;")
      ${TOUCHBUTTON22}
      ;;
    "touchpressed;button23;")
      ${TOUCHBUTTON23}
      ;;
    "touchpressed;button24;")
      ${TOUCHBUTTON24}
      ;;
    "touchpressed;button25;")
      ${TOUCHBUTTON25}
      ;;
    "touchpressed;button26;")
      ${TOUCHBUTTON26}
      ;;
    "touchpressed;button27;")
      ${TOUCHBUTTON27}
      ;;
  esac
}

writetext() {
  echo "CMDTXT,${1}" > ${TTYDEV}
  sleep ${WAITSECS}
}

# ** Main **
# Check for Command Line Parameter
if [ "${#}" -ge 1 ] && ! [ "${1}" = "tty2x" ]; then				# Command Line Parameter given, override Parameter
  echo -e "\nUsing Command Line Parameter"
  TTYDEV=${1}									# Set TTYDEV with Parameter 1
  if [ -n "${2}" ]; then							# Parameter 2 Baudrate
    BAUDRATE=${2}								# Set Baudrate
  fi										# end if Parameter 3
  echo "Using Interface: ${TTYDEV} with ${BAUDRATE} Baud"			# Device Output
fi										# end if command line Parameter 

# Let's go
if [ -c "${TTYDEV}" ]; then							# check for tty device
  dbug "${TTYDEV} detected, setting Parameter: ${BAUDRATE} ${TTYPARAM}."
  stty -F ${TTYDEV} ${BAUDRATE} ${TTYPARAM}					# set tty parameter
  sleep ${WAITSECS}
  echo "QWERTZ" > ${TTYDEV}							# First Transmission to clear serial send buffer
  dbug "Send QWERTZ as first transmission"
  sleep ${WAITSECS}
  sendrotation									# Set Display Rotation
  setvideoplay									# Set playing of videos or not
  sendtime									# Set time and date
  setscreensaver								# Set screensaver, if enabled
  while true; do								# main loop
    if [ -r ${corenamefile} ]; then						# proceed if file exists and is readable (-r)
      newcore=$(cat ${corenamefile})						# get CORENAME
      dbug "Read CORENAME: -${newcore}-"
      if [ "${newcore}" != "${oldcore}" ]; then					# proceed only if Core has changed
	dbug "Send -${newcore}- to ${TTYDEV}."
	. /media/fat/tty2tft/tty2tft-user.ini					# ReRead INI for changes
	senddata "CMDCOR,${newcore}"						# The "Magic"
	oldcore="${newcore}"							# update oldcore variable
	sendtime								# Update time and date
	setvideoplay								# Set playing of videos or not
	setscreensaver								# Reenable screensaver, if emabled
	sendrotation								# Set Display Rotation
      fi									# end if core check
      if [ "${PLAYSOUND}" = "yes" ] && [ -e "${SOUNDPATH}/${newcore}."[mM][pP]3 ]; then
	sleep ${PLAYSOUND_DELAY}
	(&>/dev/null mpg123 -q --no-control "${SOUNDPATH}/${newcore}."[mM][pP]3 &)	# Play soundfile silently in background
      fi
      [ "${1}" = "tty2x" ] && exit 9
      if [ "${debug}" = "false" ]; then
	while [ "${newcore}" = "${oldcore}" ]; do
	  inotifywait -qq -e modify -t 1 "${corenamefile}"
	  ! [ $(cat ${corenamefile}) = "${oldcore}" ] && break
	  checkbackchannel
	done
      fi
      if [ "${debug}" = "true" ]; then
	while [ "${newcore}" = "${oldcore}" ]; do
	  inotifywait -e modify -t 1 "${corenamefile}"
	  checkbackchannel
	done
      fi
    else									# CORENAME file not found
     dbug "File ${corenamefile} not found!"
    fi										# end if /tmp/CORENAME check
  done										# end while
else										# no tty detected
  echo "No ${TTYDEV} Device detected, abort."
  dbug "No ${TTYDEV} Device detected, abort."
fi										# end if tty check
# ** End Main **
