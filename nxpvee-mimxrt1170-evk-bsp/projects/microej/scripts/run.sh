#!/bin/bash

set -euo pipefail

# Defaults to ARMGCC
echo "TEST TYPE ${TEST_TYPE:=ARMGCC}"

CURRENT_DIRECTORY=$(pwd)

# go to directory of script's location
SCRIPT_DIR=$(dirname "$(realpath "$0")")
cd "${SCRIPT_DIR}/../../nxpvee-ui/sdk_makefile/"

# shell scripts may have lost their execution flag is they come from a zip
chmod -f +x $SCRIPT_DIR/../../nxpvee-ui/armgcc/*.sh || true

make RELEASE=1 flash_cmsisdap

cd "$CURRENT_DIRECTORY"
