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
  if ! [ "${ROTATE}" = "no" ]; then
    dbug "Sending: CMDROT,${ROTATE}"
    echo "CMDROT,${ROTATE}" > ${TTYDEV}						# Send Rotation if set to "yes"
    sleep ${WAITSECS}
    echo "CMDSORG" > ${TTYDEV}							# Show Start Screen rotated
    sleep 4
  fi
}

# USB Send-Picture-Data function
senddata() {
  . /media/fat/tty2tft/tty2tft-user.ini						# ReRead INI for changes
  echo "CMDCOR,${1}" > ${TTYDEV}						# Instruct the device to load the appropriate picture from SD card
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
  else
    dbug "Sending: CMDSAVER,0,0"
    echo "CMDSAVER,0,0" > ${TTYDEV}						# Disable screensaver
  fi
}

# ** Main **
# Check for Command Line Parameter
if [ "${#}" -ge 1 ]; then							# Command Line Parameter given, override Parameter
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
	senddata "${newcore}"							# The "Magic"
	oldcore="${newcore}"							# update oldcore variable
	sendtime								# Update time and date
	setscreensaver								# Reenable screensaver, if emabled
      fi									# end if core check
      [ "${debug}" = "false" ] && inotifywait -qq -e modify "${corenamefile}"	# wait here for next change of corename, -qq for quietness
      [ "${debug}" = "true" ] && inotifywait -e modify "${corenamefile}"	# but not -qq when debugging
    else									# CORENAME file not found
     dbug "File ${corenamefile} not found!"
    fi										# end if /tmp/CORENAME check
  done										# end while
else										# no tty detected
  echo "No ${TTYDEV} Device detected, abort."
  dbug "No ${TTYDEV} Device detected, abort."
fi										# end if tty check
# ** End Main **
