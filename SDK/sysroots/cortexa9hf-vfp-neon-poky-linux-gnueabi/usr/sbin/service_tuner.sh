#!/usr/bin/env bash

# reload dynamic libraries
/sbin/ldconfig

################################################################################

# Exits script with exit code and diagnostic message.
#
# param 1: Exit code.
# param @: Arguments to print before exit.
function die()
{
  local exit_code=$1
  shift
  echo "$@"
  exit "${exit_code}"
}

################################################################################

BOARD="$(ecuvarc hwversion | tr '[:upper:]' '[:lower:]')"
HASETH1="$(ecuvarc haseth1 | tr '[:upper:]' '[:lower:]')"
HASETH2="$(ecuvarc haseth2 | tr '[:upper:]' '[:lower:]')"
MODEMTYPE="$(ecuvarc modemtype | tr '[:lower:]' '[:upper:]')"

# Print service tuner info, just in case, for future debug purpose
echo "BOARD: ${BOARD}"
echo "HASETH1: ${HASETH1}"
echo "HASETH2: ${HASETH2}"
echo "MODEMTYPE: ${MODEMTYPE}"

case "${BOARD}" in
  "vcm")
    SRC_CONF="${BOARD}"
    ;;
  "tbm" | "tbm3")
    SRC_CONF="tbm3-tele"
    ;;
  *)
    die 1 "Unsupported hardware: ${BOARD}"
    ;;
esac

if [[ "$HASETH2" == "false" ]]; then
  NO_VPN_MOSQ_FILE=/etc/mosquitto/mosquitto-${SRC_CONF}-novpn.conf

  if [[ -f $NO_VPN_MOSQ_FILE ]]; then
    echo "Switch mosquitto config to NO_VPN setting, and then disable VPN Control Daemon."

    ln -sf $NO_VPN_MOSQ_FILE /etc/mosquitto/mosquitto.conf
    sync  # Force sync, just in case if device reboots suddenly (See RIOBF-1786)
    systemctl disable vpnctld
  else
    die 1 "Mosquitto config file for NO_VPN setting has not been found"
  fi
fi

if [[ "$MODEMTYPE" == "NONE" ]]; then
  if [[ "$BOARD" != "vcm" ]]; then
    systemctl disable modemd
  fi
fi

if [[ "$HASETH1" == "false" ]]; then
  if [[ "$BOARD" == "vcm" ]]; then
    systemctl stop mmt-supervise
    systemctl disable mmtd
    systemctl disable mosquitto-mmt
    systemctl disable mmt-supervise
    systemctl disable mdpd
    systemctl disable nginx
  fi
  #if there are no ethernet links, we can disable check_ethernet_link_status
  #and check eth_interface
  if [[ "$HASETH2" == "false" ]]; then
    systemctl stop check_ethernet_link_status
    systemctl stop check_eth_interface
    systemctl disable check_ethernet_link_status
    systemctl disable check_eth_interface
  fi
fi

systemctl disable service_tuner
