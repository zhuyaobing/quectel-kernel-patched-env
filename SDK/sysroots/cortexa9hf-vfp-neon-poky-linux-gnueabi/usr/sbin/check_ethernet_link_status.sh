#!/bin/sh

PREV_ERROR=""
ERROR="off"
SYSFS_PATH="/sys/bus/mdio_bus/devices/2188000.ethernet:"
MAX_ETH_CHECKS=60

link_status="down"
eth_error_counter=0
haseth1=$(ecuvarc haseth1)
haseth2=$(ecuvarc haseth2)

trap "exit 0" SIGTERM

while :
do
   sleep 1
   for phy in "04;$haseth2" "05;$haseth1" ; do
      IFS=';' read -r phy_addr available <<< "$phy"
      unset IFS

      f="${SYSFS_PATH}${phy_addr}/link_status"

      if [[ "$available" == "true" ]] && [[ ! -e "$f" ]] ; then
         if (( "$eth_error_counter" == 0)); then
            echo "Missing PHY on MDIO address $phy_addr"
         fi
         ERROR="on"
         break
      fi

      link_status="$(cat "$f" 2> /dev/null)"
      if [[ $link_status == "down" ]] ; then
         if (( "$eth_error_counter" == 0)); then
            echo "Link on PHY at MDIO address $phy_addr is down"
         fi
         ERROR="on"
         break
      elif [[ $link_status == "up" ]] ; then
         ERROR="off"
      fi
   done

   if [[ $ERROR == "on" ]]; then
      if (( "$eth_error_counter" >= "$MAX_ETH_CHECKS" )) && [[ $PREV_ERROR != $ERROR ]]; then
         echo "Reporting Ethernet failure to CSP: $ERROR"
         mqttsender ethernet_failure:$ERROR
         PREV_ERROR=$ERROR
      else
         ((eth_error_counter++))
      fi
   else
      eth_error_counter=0
      if [[ $PREV_ERROR != $ERROR ]]; then
         echo "Eth link detected"
         mqttsender ethernet_failure:$ERROR
         PREV_ERROR=$ERROR
      fi
   fi

done