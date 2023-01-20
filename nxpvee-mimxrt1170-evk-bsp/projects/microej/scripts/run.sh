#!/bin/bash

set -euo pipefail

# Defaults to ARMGCC
echo "TEST TYPE ${TEST_TYPE:=ARMGCC}"

if [ "${TEST_TYPE:=ARMGCC}" == "ARMGCC" ]
then
    CURRENT_DIRECTORY=$(pwd)

    # go to directory of script's location
    SCRIPT_DIR=$(dirname "$(realpath "$0")")

    cd "${SCRIPT_DIR}/../../nxpvee-ui/sdk_makefile/"
    make RELEASE=1 flash_cmsisdap

    cd "$CURRENT_DIRECTORY"

elif [ "${TEST_TYPE}" == "MCU" ]
then
    CURRENT_DIRECTORY=$(pwd)
    
    # go to directory of script's location
    SCRIPT_DIR=$(dirname "$(realpath "$0")")
    
    crt_emu_cm_redlink --flash-load-exec application.axf \
        -g --debug 2  --vendor NXP -p MIMXRT1176xxxxx \
        --ConnectScript RT1170_connect_M7_wake_M4.scp --ResetScript RT1170_reset.scp  \
        -ProbeHandle=1 -CoreIndex=0 --flash-driver= \
        -x ${SCRIPT_DIR}/../../common/sdk_makefile/cmsisdap_support \
         --flash-hashing

cd "$CURRENT_DIRECTORY"

else
    exit 1
fi

