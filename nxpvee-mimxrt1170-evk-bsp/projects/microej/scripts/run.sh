#!/bin/bash

set -euo pipefail

# Defaults to ARMGCC
echo "TEST TYPE ${TEST_TYPE:=ARMGCC}"

CURRENT_DIRECTORY=$(pwd)

# go to directory of script's location
SCRIPT_DIR=$(dirname "$(realpath "$0")")

cd "${SCRIPT_DIR}/../../nxpvee-ui/sdk_makefile/"
make RELEASE=1 flash_cmsisdap

cd "$CURRENT_DIRECTORY"
