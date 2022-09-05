#!/bin/bash
echo "Load EMMY-W1 bluetooth driver (SDIO-UART)..."
modprobe "sd8887-sdiouart"
modprobe "hci_uart"
modprobe "hci_uart-sdiouart"

status=$?
#We only need to check the last command's success since it WILL fail if prerequisite module didn't load properly
if [ $status -eq 0 ]; then
    echo "EMMY-W1 bluetooth driver loaded successfully."
else
    echo "Failed to load EMMY-W1 driver (exit code $?). WLAN/BT might be unavailable."
fi

exit $status