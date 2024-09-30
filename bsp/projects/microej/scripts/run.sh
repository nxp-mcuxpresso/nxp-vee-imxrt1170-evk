#!/bin/bash

# 'run.sh' implementation
# 'run.sh' is responsible for flashing the executable file on the target device

set -euo pipefail

. $(dirname $(realpath $0))/"set_project_env.sh" || exit 1

cd $MAKEFILE_DIR || exit 2

# shell scripts may have lost their execution flag is they come from a zip
chmod -f +x $BINARY_DIR/../*.sh || true

make $FLASH_CMD || exit 3

cd $CURRENT_DIRECTORY || exit 4
