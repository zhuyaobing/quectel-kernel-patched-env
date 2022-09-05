#!/bin/bash

FAIL=1
ECU_LOGS=ecu_logs
LICENSES=common-licenses
ECU_LOGS_TPA=ecu_logs_tpa
LICENSES_TPA=common-licenses-tpa

################################################################################

function test_partition_args {

   if [ -z "${USBPATH}" ] || [ -z "${TPALOGS}" ]; then
      echo "Partition variables are missing. Check partition.args file."
      echo "USBPATH = ${USBPATH}"
      echo "TPALOGS = ${TPALOGS}"
      exit ${FAIL}
   fi
}

################################################################################

function move_logs {

    if [ -d  "${TPALOGS}/${ECU_LOGS}" ]; then
       if [ ! -d "${USBPATH}/${ECU_LOGS_TPA}" ]; then
          mkdir "${USBPATH}/${ECU_LOGS_TPA}"
       fi

       mv "${TPALOGS}/${ECU_LOGS}/"* "${USBPATH}/${ECU_LOGS_TPA}"
       rm -r "${TPALOGS}/${ECU_LOGS:?}"
    else
       echo "${TPALOGS}/${ECU_LOGS} does not exist"
    fi

    if [ -d  "${TPALOGS}/${LICENSES}" ]; then
       if [ ! -d "${USBPATH}/${LICENSES_TPA}" ]; then
          mkdir "${USBPATH}/${LICENSES_TPA}"
       fi

       mv "${TPALOGS}/${LICENSES}/"* "${USBPATH}/${LICENSES_TPA}"
       rm -r "${TPALOGS}/${LICENSES:?}"
    else
       echo "${TPALOGS}/${LICENSES} does not exist"
    fi
}

################################################################################
# MAIN
################################################################################

test_partition_args
move_logs
