#!/bin/bash

# 'run.sh' implementation
# 'run.sh' is responsible for flashing the executable file on the target device

set -euo pipefail

source $(dirname $(realpath $0))/"set_project_env.sh" || exit 1

cd $MAKEFILE_DIR || exit 2

# shell scripts may have lost their execution flag is they come from a zip
chmod -f +x $SCRIPT_DIR/armgcc/*.sh || true

${BUILD_CMD} $FLASH_CMD || exit 3

cd $CURRENT_DIRECTORY || exit 4
