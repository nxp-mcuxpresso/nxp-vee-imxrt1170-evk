#!/bin/bash

# 'build.sh' implementation for GCC toolchain

# 'build.sh' is responsible for producing the executable file 
# then copying this executable file to the current directory where it has been executed to a file named 'application.out'

# Force to stop execution on error
set -euo pipefail

source $(dirname $(realpath $0))/"set_project_env.sh" || exit 1

cd $MAKEFILE_DIR || exit 2

if [ "${USE_PRESET}" == "1" ]
then
  cp ${MAKEFILE_DIR}/../bsp/vee/lib/microejapp.o ${MAKEFILE_DIR}
  cp ${MAKEFILE_DIR}/../bsp/vee/lib/microejruntime.a ${MAKEFILE_DIR}
fi

# shell scripts may have lost their execution flag is they come from a zip
chmod -f +x $SCRIPT_DIR/armgcc/*.sh || true

${BUILD_CMD} || exit 3

echo "Copy $BINARY_DIR/npavee.{elf,hex,bin} into $CURRENT_DIRECTORY/application.{out,hex,bin}"

cp $BINARY_DIR/npavee.elf $CURRENT_DIRECTORY/application.out || exit 4
cp $BINARY_DIR/npavee.hex $CURRENT_DIRECTORY/application.hex || exit 4
cp $BINARY_DIR/npavee.bin $CURRENT_DIRECTORY/application.bin || exit 4

cd $CURRENT_DIRECTORY || exit 5
