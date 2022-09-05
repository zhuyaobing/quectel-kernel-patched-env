#!/usr/bin/env bash

SCRIPT_NAME="${BASH_SOURCE[0]##*/}"
SCRIPT_ACTION=${1:?"action required, try 'help'"}
shift
SCRIPT_OTHER_PARAMS=$@

SYS_CLASS_PATH="/sys/class/gpio"

ECU_VERSION=${ECU_VERSION:-"$(ecuvarc -t1 hwversion)"}

_setup_variables() {
   case "$ECU_VERSION" in
      tbm | vcm)
         TTY_ACM=${TTY_ACM="/dev/ttyUbloxModemATSupport"}
         PIN_VCC_OUT=3
         PIN_PWRON_OUT=144
         PIN_RESET_OUT=145
         PIN_SIMLOCK_IN=142
         PIN_VINT_IN=9
         VCC_ON_VALUE=1
         VCC_OFF_VALUE=0
         ;;
      n2)
         TTY_ACM=${TTY_ACM="/dev/ttyUbloxModemATSupport"}
         PIN_VCC_OUT=162
         PIN_PWRON_OUT=157
         PIN_RESET_OUT=156
         PIN_SIMLOCK_IN=142
         PIN_VINT_IN=160
         PIN_SIM_SWITCH=123
         PIN_RFSWITCH_CLK=151
         VCC_ON_VALUE=0
         VCC_OFF_VALUE=1
         ;;
      *)
         echo "Unable to initalize for ECU_VERSION $ECU_VERSION"
         exit 1
   esac
}

send_at_cmd() {
   local timeout=2
   for p in "$@"; do
      if [[ "$p" == "timeout:"* ]]; then
         timeout="${p##timeout:}"
         echo "Timeout for subsequent command is set to $timeout"
         continue
      fi

      local cmd="AT$p"
      local outfile="/tmp/mdmctl_chat_output"

      echo "Send '$cmd' to $TTY_ACM"

      if [ ! -e "$TTY_ACM" ]; then
         echo "$TTY_ACM doesn't exist, unable to send cmd: $cmd"
         return 1
      fi

      stty -F "$TTY_ACM" 115200

      chat -t "$timeout" -V '' "$cmd" OK '' <"$TTY_ACM" > "$TTY_ACM" 2>"$outfile"
      cat "$outfile"
      echo ""
   done

   return 0
}

_read_gpio_value() {
   local gpio=$1
   cat "$SYS_CLASS_PATH/gpio${gpio}/value"
}

_write_gpio_value() {
   local gpio=$1 value=$2 desc=$3
   local p="$SYS_CLASS_PATH/gpio${gpio}/value"
   echo "write $value to $p ($desc)"
   echo "$value" > "$p"
}

pin_pwron() {
   _write_gpio_value "$PIN_PWRON_OUT" "$1" "pwron"
}

pin_reset() {
   _write_gpio_value "$PIN_RESET_OUT" "$1" "reset"
}

pin_vcc() {
   _write_gpio_value "$PIN_VCC_OUT" "$1" "vcc"
}

on() {
   setup_gpios
   pin_pwron 0
   pin_reset 0
   pin_vcc $VCC_ON_VALUE
   pin_pwron 1
}

off() {
   setup_gpios
   send_at_cmd "timeout:10" "+CPWROFF" && sleep 5
   pin_pwron 1
   sleep 1
   pin_vcc $VCC_OFF_VALUE
}

vint() {
    local val
    val=$(_read_gpio_value "$PIN_VINT_IN")
    echo "vint($PIN_VINT_IN): $val"
}

simlock() {
    local val

    if [ "$ECU_VERSION" == "n2" ]; then
       echo "'simlock' not supported on '$ECU_VERSION' hardware"
       return 1
    fi

    val=$(_read_gpio_value $PIN_SIMLOCK_IN)
    echo "simlock($PIN_SIMLOCK_IN): $val"
}

tty_present() {
   if [ -e "$TTY_ACM" ]; then
      echo yes
      return 0
   else
      echo no
      return 1
   fi
}

is_on() {
   tty_present
}

sim() {
    local num=${1:?"requires a SIM number, possible values: 1 or 2"}
    local value=0

    if [ "$ECU_VERSION" != "n2" ]; then
       echo "sim switching not supported on '$ECU_VERSION' hardware"
       return 1
    fi

    if [ "$num" == "1" ]; then
        value=0
    elif [ "$num" == "2" ]; then
        value=1
    else
        echo "SIM parameter error ('$num'): only 1 or 2 are allowed values"
        exit 1
    fi

    _write_gpio_value $PIN_SIM_SWITCH $value "sim switch"
    _write_gpio_value $PIN_RFSWITCH_CLK 1 "rfswitch clk"
    sleep 1
    _write_gpio_value $PIN_RFSWITCH_CLK 0 "rfswitch clk"
    atcmd "+CFUN=15"
    sleep 3
}

_export_gpio() {
   local g=$1
   echo "Export GPIO $g"
   echo "$g" > "${SYS_CLASS_PATH}/export"
}

_unexport_gpio() {
   local g=$1
   echo "unexport GPIO $g"
   echo "$g" > "${SYS_CLASS_PATH}/unexport"
}

_setup_gpio() {
   local g=$1  # string with ID:[in|out]

   local gpio="${g%%:*}"
   local direction="${g##*:}"
   local p="$SYS_CLASS_PATH/gpio${gpio}/direction"

   [ ! -e "$p" ] && return 1

   local curent_val=$(cat "$p" 2> /dev/null)
   [ "$curent_val" = "$direction" ] && return 0

   echo "Write '$direction' to $p"
   echo "$direction" > "$p" 2> /dev/null
}

export_gpios() {
   local gpios=()
   local unexport="${1:-"false"}"

   case "$ECU_VERSION" in
      tbm | vcm)
         gpios=( $PIN_VCC_OUT $PIN_PWRON_OUT $PIN_RESET_OUT $PIN_SIMLOCK_IN $PIN_VINT_IN )
         ;;
      n2)
         gpios=( $PIN_VCC_OUT $PIN_PWRON_OUT $PIN_RESET_OUT $PIN_VINT_IN $PIN_SIM_SWITCH $PIN_RFSWITCH_CLK )
         ;;
      *)
         echo "Unable to export GPIOs for ECU_VERSION $ECU_VERSION"
         exit 1
   esac

   for g in "${gpios[@]}"; do
      if [ "$unexport" == "true" ]; then
         _unexport_gpio "$g"
      else
         _export_gpio "$g"
      fi
   done
}

setup_gpios() {
   [ -e "$SYS_CLASS_PATH/gpio${PIN_PWRON_OUT}" ] || export_gpios

   local gpios=()

   case "$ECU_VERSION" in
      tbm | vcm)
         gpios=(
            "$PIN_VCC_OUT:out"
            "$PIN_PWRON_OUT:out"
            "$PIN_RESET_OUT:out"
            "$PIN_SIMLOCK_IN:in"
            "$PIN_VINT_IN:in"
         )
         ;;
      n2)
         gpios=(
            "$PIN_VCC_OUT:out"
            "$PIN_PWRON_OUT:out"
            "$PIN_RESET_OUT:out"
            "$PIN_VINT_IN:in"
            "$PIN_SIM_SWITCH:out"
            "$PIN_RFSWITCH_CLK:out"
         )
         ;;
      *)
         echo "Unable to setup gpios for ECU_VERSION $ECU_VERSION"
         exit 1
   esac

   for g in "${gpios[@]}"; do
      if ! _setup_gpio "$g"; then
         echo "Unable to setup GPIO $g"
         continue
      fi
   done
}

help() {
   echo "Available actions: "
   declare -F | sed 's/declare\ -f/\ \ /g' | grep -v '  _'
}

factory_reset(){
   if ! tty_present > /dev/null ; then
      echo "Modem is powered off, turning it on!"
      on
   fi

   local timeout=20
   if ! wait_for_tty $timeout; then
      echo "Unable to do factory reset, modem not present!"
      exit 1
   fi

   local def_uusbconf=3  # all TOBY modemds
   local model="$(send_at_cmd +GMM; echo "xyzf")"
   model="${model%%xyzf}"

   if [[ "$model" == *"SARA"* ]]; then
      echo "SARA modem"
      def_uusbconf=0
   else
      echo "TOBY modem"
      def_uusbconf=3
   fi

   send_at_cmd "+UUSBCONF=$def_uusbconf"
   send_at_cmd "+UFACTORY=0,1"
   sleep 2
   off
   sleep 2
   on
}

atcmd() {
    send_at_cmd "$@"
}

enable_uart() {
   if [ "$ECU_VERSION" != "n2" ]; then
      echo "IMXC UART not supported on '$ECU_VERSION' hardware"
      return 1
   fi

   local g=17

   if ! _export_gpio "$g"; then
      echo "Error exporting GPIO $g"
      return 1
   fi

   local g_dir="$g:out"
   if ! _setup_gpio "$g_dir"; then
      echo "Unable to setup GPIO $g_dir"
      return 1
   fi

   _write_gpio_value "$g" "1" "enable_uart"
}

wait_for_tty() {
   local total_sec=$1

   for i in $(seq "${total_sec}"); do
      if ! tty_present > /dev/null ; then
         sleep 1
      else
         echo "$TTY_ACM became available!"
         return 0
      fi
   done

   echo "TTY $TTY_ACM did not become available after $total_sec seconds"
   return 1
}

_main() {
   if ! declare -f "${SCRIPT_ACTION}" &>/dev/null; then
      echo "Provided action '$SCRIPT_ACTION' is not supported, try 'help'"
      exit 1
   fi

   if ! [[ "$ECU_VERSION" =~ [tbm|vcm|n2] ]]; then
      echo "ECU_VERSION $ECU_VERSION not supported!"
      exit 1
   fi

   echo "Execute for ECU type '$ECU_VERSION'"
   _setup_variables

   $SCRIPT_ACTION $SCRIPT_OTHER_PARAMS
}

_main

