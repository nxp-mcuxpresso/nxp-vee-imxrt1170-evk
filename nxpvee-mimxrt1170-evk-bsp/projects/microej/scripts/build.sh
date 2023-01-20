#!/bin/bash

set -euo pipefail

# Defaults to ARMGCC
echo "TEST TYPE ${TEST_TYPE:=ARMGCC}"

if [ "${TEST_TYPE:=ARMGCC}" == "ARMGCC" ]
then
    CURRENT_DIRECTORY=$(pwd)

    # go to parent directory of script's location
    SCRIPT_DIR=$(dirname "$(realpath "$0")")
    cd "$SCRIPT_DIR/../../nxpvee-ui/sdk_makefile" || exit 1

    make RELEASE=1 || exit 2

    cp ../armgcc/flexspi_nor_sdram_release/nxpvee_ui.elf "$CURRENT_DIRECTORY"/application.out || exit 3
    cp ../armgcc/flexspi_nor_sdram_release/nxpvee_ui.hex "$CURRENT_DIRECTORY"/application.hex || exit 3
    cp ../armgcc/flexspi_nor_sdram_release/nxpvee_ui.bin "$CURRENT_DIRECTORY"/application.bin || exit 3

    cd "$CURRENT_DIRECTORY" || exit 4

elif [ "${TEST_TYPE}" == "MCU" ]
then
    CURRENT_DIRECTORY=$(pwd)

    # go to parent directory of script's location
    SCRIPT_DIR=$(dirname "$(realpath "$0")")

    ${SCRIPT_DIR}/../../common/scripts/build_mcu.sh -i "${MCUXPRESSOIDE_VAR}" -w "${MCUIDE_WORKSPACE_VAR}"
    cp ${SCRIPT_DIR}/../../nxpvee-ui/mcuide/Release/rt1170_nxpvee.axf "$CURRENT_DIRECTORY"/application.out || exit 2
    cp ${SCRIPT_DIR}/../../nxpvee-ui/mcuide/Release/rt1170_nxpvee.axf "$CURRENT_DIRECTORY"/application.axf || exit 3

    cd "$CURRENT_DIRECTORY" || exit 4

else
    exit 5
fi
