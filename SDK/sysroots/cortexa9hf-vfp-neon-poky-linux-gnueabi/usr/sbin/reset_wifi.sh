#!/usr/bin/env bash

CONF_FILE=/etc/reset_wifi.conf

if [ "$1" == "--init" ] ; then
	HWVAR="$(ecuvarc hwvariant)"
	BOARD="$(echo $HWVAR | cut -d_ -f1)"
	VER="$(echo $HWVAR | cut -d_ -f2)"
	if [ "$BOARD" = "N2" ] ; then
		echo "SDIO_NAME=2198000.usdhc" > $CONF_FILE
		echo "GPIO_BANK=4" >> $CONF_FILE
		echo "GPIO_PIN=5" >> $CONF_FILE
		echo "ACTIVE_LOW=1" >> $CONF_FILE
	else
		echo "SDIO_NAME=2194000.usdhc" > $CONF_FILE
		echo "GPIO_BANK=7" >> $CONF_FILE
		echo "GPIO_PIN=4" >> $CONF_FILE
		if [ $VER -lt 2 ] ; then
			echo "ACTIVE_LOW=0" >> $CONF_FILE
		else
			echo "ACTIVE_LOW=1" >> $CONF_FILE
		fi
	fi
fi

[ ! -s $CONF_FILE ] && exit 0
. $CONF_FILE

SYS_SDIO_PATH=/sys/bus/platform/drivers/sdhci-esdhc-imx
SYS_GPIO_PATH=/sys/class/gpio
GPIO_NUM=$((($GPIO_BANK - 1) * 32 + $GPIO_PIN))

GPIO_ACTIVE="low"
GPIO_INACTIVE="high"

if [ $ACTIVE_LOW -eq 0 ] ; then
	GPIO_ACTIVE="high"
	GPIO_INACTIVE="low"
fi

if [ "$1" != "--force" ] ; then
	echo -n "Checking status of Wifi: "
	if [ ! -d $SYS_SDIO_PATH/$SDIO_NAME/mmc_host/mmc?/mmc?\:0001 ] ; then
		echo "not detected on SDIO"
	elif ! ip link show mlan0 > /dev/null 2>&1 ; then
		echo "no network interface"
	else
		echo "OK"
		echo "If you want to reset anyway use '--force'"
		exit 0
	fi
fi

if [ "$(lsmod | grep sd8887)" ] ; then
	echo "This command doesn't work when driver is loaded!"
	echo "If you really want to do a reset of WiFi/BT then:"
	echo "1 No service should be using the mlan0 interface, e.g. hostapd."
	echo "2 Bring the interface down, i.e. 'ip l s dev mlan0 down' (might"
	echo "  need to be called multiple times)."
	echo "3 Unload the modules, i.e. 'modprobe -r sd8887-sdiouart'."
	echo "After that the chip can be reseted."
	exit
fi

echo -n ${SDIO_NAME} > ${SYS_SDIO_PATH}/unbind

echo ${GPIO_NUM} > ${SYS_GPIO_PATH}/export
echo ${GPIO_INACTIVE} > ${SYS_GPIO_PATH}/gpio${GPIO_NUM}/direction
usleep 20000
echo ${GPIO_ACTIVE} > ${SYS_GPIO_PATH}/gpio${GPIO_NUM}/direction
echo ${GPIO_NUM} > ${SYS_GPIO_PATH}/unexport

echo -n ${SDIO_NAME} > ${SYS_SDIO_PATH}/bind
