#!/usr/bin/env bash

print_help() {
    echo "Configures the WiFi AP"
    echo ""
    echo "Usage:"
    echo "  $(basename "$0") <options>"
    echo ""
    echo "  -s, --ssid       Set SSID of AP (default [BOARD]-[SERIAL_NUMBER])"
    echo "  -p, --passwd     Sett password of AP (default [SERIAL_NUMBER])"
    echo "  -S, --start      Start the AP after configure, done automatically"
    echo "                   if service already enabled"
}

while [[ $# -gt 0 ]] ; do
    key="$1"

    case $key in
    -s|--ssid)
        SSID="$2"
        shift # past argument
        shift # past value
        ;;
    -p|--passwd)
        PASSWD="$2"
        shift # past argument
        shift # past value
        ;;
    -S|--start)
        START="YES"
        shift # past argument
        ;;
    -h|--help)
        print_help
        exit 0
        ;;
    *)    # unknown option
        echo "Unknown option: $key"
        print_help
        exit 1
        ;;
    esac
done

HOSTAPD_CONF="/etc/hostapd.conf"
HOSTAPD_SERVICE="hostapd.service"

BOARD="$(ecuvarc -t 3 hwversion 2> /dev/null || echo unknown)"
SERIAL_NUMBER="$(ecuvarc -t 3 serialnumber 2> /dev/null || echo 000000-0000)"

SSID=${SSID:-$BOARD-$SERIAL_NUMBER}
PASSWD=${PASSWD:-$SERIAL_NUMBER}

echo "Configuring WiFi AP SSID ($SSID) and WPA2 password ($PASSWD)"
sed -i \
    -e 's/^\(ssid=\).*$/\1'"$SSID"'/' \
    -e 's/^#*\(wpa=\).*$/\12/' \
    -e 's/^#*\(wpa_passphrase=\).*$/\1'"$PASSWD"'/' \
    $HOSTAPD_CONF

# Only enable by default on N2 development
if [ "$BOARD" = "n2" ] && grep -q "console=" /proc/cmdline ; then
    echo "Enabling WiFi AP"
    systemctl enable $HOSTAPD_SERVICE
fi

ENABLED=$(systemctl is-enabled $HOSTAPD_SERVICE)
if [ "$START" = "YES" -o "$ENABLED" = "enabled" ] ; then
    echo "(Re)staring WiFi AP"
    systemctl restart $HOSTAPD_SERVICE
fi
