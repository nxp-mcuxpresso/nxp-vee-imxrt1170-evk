#!/bin/bash

# 'build.sh' implementation for GCC toolchain

# 'build.sh' is responsible for producing the executable file 
# then copying this executable file to the current directory where it has been executed to a file named 'application.out'

# Force to stop execution on error
set -euo pipefail

. $(dirname $(realpath $0))/"set_project_env.sh" || exit 1

cd $MAKEFILE_DIR || exit 2

# shell scripts may have lost their execution flag is they come from a zip
chmod -f +x $BINARY_DIR/../*.sh || true

make || exit 3

echo "Copy $BINARY_DIR/nxpvee_ui.{elf,hex,bin} into $CURRENT_DIRECTORY/application.{out,hex,bin}"

cp $BINARY_DIR/nxpvee_ui.elf $CURRENT_DIRECTORY/application.out || exit 4
cp $BINARY_DIR/nxpvee_ui.hex $CURRENT_DIRECTORY/application.hex || exit 4
cp $BINARY_DIR/nxpvee_ui.bin $CURRENT_DIRECTORY/application.bin || exit 4

cd $CURRENT_DIRECTORY || exit 5
