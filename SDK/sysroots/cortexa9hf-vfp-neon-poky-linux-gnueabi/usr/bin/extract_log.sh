#!/bin/bash

FAIL=1
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
LOGUPDATEDIR=/common/update-logs
LOGROOT=ecu_logs
EXTRACT_BINARY_FILE=ecu_log_binary
DONT_EXTRACT_LICENSE_FILES=ecu_licenses_none
COMMON_LICENSES=common-licenses

################################################################################

function finish {

    echo "Log extraction finished"
    echo "Removing temporary log extraction files"

    rm -rf "${TEMPLOG}"
}

trap finish EXIT TERM INT

################################################################################

function set_log_file_base_name {

    echo "Setting log file name"

    local model
    model=$(ecuvarc hwversion)
    if [ -z "$model" ]; then
        model="MODELNA"
    fi
    local serial
    serial=$(ecuvarc serialnumber)
    if [ -z "$serial" ]; then
        serial="SERIALNA"
    fi
    local vin
    vin=$(ecuvarc vehicleid)
    if [ -z "$vin" ]; then
        vin="VINNA"
    fi

    LOGFILEBASENAME=${model}${SUFFIX}_${serial}_${vin}_${TIMESTAMP}
    LOGFILE=${LOGFILEBASENAME}.log
    LOGFILETAR=${LOGFILE}.tar.gz
    LOGBINTAR=${LOGFILEBASENAME}.bin.tar.gz
    LOGUPDATETAR=${LOGFILEBASENAME}.update.tar.gz
}

################################################################################

function copy_licenses {

    echo "Copying licenses information"

    if [ ! -f "${USBPATH}/${DONT_EXTRACT_LICENSE_FILES}" ]; then
        tar -C "${TEMPLOG}" -xf "/usr/share/${COMMON_LICENSES}.tar.bz2"
        cp -RL "${TEMPLOG}/${COMMON_LICENSES}" "${USBPATH}"
        # Shellcheck issue SC2115. :? rises an issue if ${COMMON_LICENSES} is
        # empty avoiding the removal of ${TEMPLOG}.
        rm -rf "${TEMPLOG}/${COMMON_LICENSES:?}"
    else
        echo "${USBPATH}/${DONT_EXTRACT_LICENSE_FILES} found, will not extract license files!"
    fi
}

################################################################################

# TODO:
# It is not possible to guarantee this script will run before the spider in TPA
# and bugs could come up.
# For a better solution spider should be used instead.
function check_usb_stick {

    echo "Checking USB stick"

    if [ -d "${USBPATH}" ]; then
        echo "USB is mounted on ${USBPATH}, preparing to extract logs ..."
    else
        echo "USB is NOT mounted on ${USBPATH}, aborting log extraction"
        exit ${FAIL}
    fi

    if [ -f "${USBPATH}/${EXTRACT_BINARY_FILE}" ] && [ -n "${TPALOGS}" ]; then
        touch "${TPALOGS}/${EXTRACT_BINARY_FILE}"
    else
        rm -f "${TPALOGS}/${EXTRACT_BINARY_FILE}"
    fi

    if [ -f "${USBPATH}/${DONT_EXTRACT_LICENSE_FILES}" ] && [ -n "${TPALOGS}" ]; then
        touch "${TPALOGS}/${DONT_EXTRACT_LICENSE_FILES}"
    else
        rm -f "${TPALOGS}/${DONT_EXTRACT_LICENSE_FILES}"
    fi
}

################################################################################

function setup_dirs {

    echo "Creating directories for log extraction"

    mkdir -p "${TEMPLOG}"
    mkdir -p "${USBPATH}/${LOGROOT}"
}

################################################################################

function create_log_file {

    echo "Creating log file"

    systemd-cat -t version -p notice opkg list-installed
    systemd-cat -t version -p notice ecuvarc
    journalctl --sync
}

################################################################################

function generate_encryption_key {

    echo "Generating encryption key"

    openssl rand -base64 32 > "${TEMPLOG}/${LOGFILE}".key
    openssl rsautl -encrypt -inkey /usr/share/log.public.pem -pubin -in "${TEMPLOG}/${LOGFILE}".key -out "${USBPATH}"/${LOGROOT}/"${LOGFILE}".key.enc
}

################################################################################

function extract_binary_logs {

    echo "Extracting binary logs"

    tar -cvzf - -C /var/log/ journal |
        openssl enc -aes-256-cbc -salt -out ${USBPATH}/${LOGROOT}/${LOGBINTAR}.enc -pass file:/${TEMPLOG}/${LOGFILE}.key
}

################################################################################

function extract_json_logs {

    echo "Extracting json logs"

    echo "Generating json log file ${LOGFILE} ..."
    journalctl -o json --no-pager > "${TEMPLOG}/${LOGFILE}"

    echo "Writing ${LOGFILE} to ${USBPATH} ..."
    tar -cvzf - -C "${TEMPLOG}" ./"${LOGFILE}" |
        openssl enc -aes-256-cbc -salt -out "${USBPATH}/${LOGROOT}/${LOGFILETAR}".enc -pass file:"${TEMPLOG}/${LOGFILE}".key
}

################################################################################

function extract_update_logs {

    echo "Extracting update logs"

    tar -cvzf - -C ${LOGUPDATEDIR} . |
        openssl enc -aes-256-cbc -salt -out "${USBPATH}/${LOGROOT}/${LOGUPDATETAR}".enc -pass file:/"${TEMPLOG}/${LOGFILE}".key
}

################################################################################

function test_partition_args {

   if [ -z "${TEMPLOG}" ] || [ -z "${USBPATH}" ]; then
      echo "Partition variables are missing. Check partition.args file."
      echo "TEMPLOG = ${TEMPLOG}"
      echo "USBPATH = ${USBPATH}"
      exit ${FAIL}
   fi
}

################################################################################
# MAIN
################################################################################

test_partition_args
check_usb_stick
set_log_file_base_name
setup_dirs
copy_licenses
create_log_file
generate_encryption_key

if [ -f "${USBPATH}/${EXTRACT_BINARY_FILE}" ]; then
    extract_binary_logs
else
    extract_json_logs
fi

if [ -d "${LOGUPDATEDIR}" ]; then
    extract_update_logs
fi
