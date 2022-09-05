#!/bin/bash

MAX_RELOAD=20 #each reload = 15 sec, so 20 reloads = 300 sec
INTERFACE=ethext
reload_ctr=0
state="start"

trap "exit 0" SIGTERM

while :
do
   case "$state" in
   "start")
      output=$(ip link show $INTERFACE)

      if [[ $output == *"DOWN"* ]]; then
         if  [[ $reload_ctr -ge $MAX_RELOAD ]]; then
            state="report_dtc"
         else
            state="reload"
         fi
      else
         reload_ctr=0
         state="start"
         sleep 1;
         fi
      ;;
   "reload")
      ((reload_ctr++))
      ip link set down $INTERFACE
      sleep 1
      ip link set up $INTERFACE
      sleep 14 #total time on state = 15 sec
      state="start"
      ;;
   "report_dtc")
      echo "The exteth INTERFACE could no be brougth up - Reporting Internal Hardware Error DTC"
      mqttsender status_ext_eth_interface_error:"on"
      echo "Exiting service check_eth_interface"
      break
      ;;
   *)
      echo "invalid \"$state\""
      break
      ;;
   esac
done