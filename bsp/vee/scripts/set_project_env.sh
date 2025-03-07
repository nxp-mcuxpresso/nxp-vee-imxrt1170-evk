#!/bin/bash

# 'set_project_env.bat' implementation 

# 'set_project_env' is responsible for
# - checking the availability of required environment variables
# - setting project local variables for 'build.sh' and 'run.sh'

# Change the following variables according to your project

# Set "evk" for MIMXRT1170-EVK board or "evkb" for MIMXRT1170-EVKB board
export CHOSEN_BOARD=evkb

# Set 1 for RELEASE mode and 0 for DEBUG mode
export CHOSEN_MODE=1

# Set "flash_cmsisdap" for Linkserver probe or "flash" for J-Link probe
export CHOSEN_PROBE=flash_cmsisdap

# Don't change the following lines

if [ ! -v BOARD ]; then
    export BOARD="$CHOSEN_BOARD"
fi
echo "Board: $BOARD"

if [ ! -v RELEASE ]; then
    export RELEASE="$CHOSEN_MODE"
fi
echo "Mode (1 for RELEASE, 0 for DEBUG): $RELEASE"

# Check toolchain path
if [ ! -v ARMGCC_DIR ]; then
    export DEFAULT_ARMGCC_DIR="/opt/gcc-arm-none-eabi-13.2-Rel1"
    if [[ -d "$DEFAULT_ARMGCC_DIR" ]]; then
        export ARMGCC_DIR=$DEFAULT_ARMGCC_DIR
        echo "ARMGCC_DIR not set, default location used $ARMGCC_DIR"
    else
        echo "ARMGCC_DIR does not exist, terminating."
        exit 1
    fi
else
    echo "ARMGCC_DIR=$ARMGCC_DIR"
fi

# Check the CMake minimum version required
CMAKE_REQUIRED=3.27
cmake_version=$(cmake --version | grep -oP '\d+\.\d+\.\d+')
if printf '%s\n' "$CMAKE_REQUIRED" "$cmake_version" | sort --version-sort --check --reverse &> /dev/null
then
    printf "ERROR: CMake version ($cmake_version) is too old ($CMAKE_REQUIRED or later required), terminating.\n"
    exit 1
fi

export SCRIPT_DIR="$(dirname $(realpath $0))"
echo "Script directory: $SCRIPT_DIR"

export CURRENT_DIRECTORY=$(pwd)
echo "Current directory: $CURRENT_DIRECTORY"

# USE_PRESET=1 in case cmake commands are used instead of build and run scripts
USE_PRESET=${USE_PRESET:-0}
if [ "${USE_PRESET}" == "0" ]
then
    export BUILD_CMD=make
    if [ $RELEASE = 1 ]; then
        export BUILD_DIR="flexspi_nor_sdram_release_$BOARD/"
    else
        export BUILD_DIR="flexspi_nor_sdram_debug_$BOARD/"
    fi
    export MAKEFILE_DIR="$SCRIPT_DIR/sdk_makefile"
    export BINARY_DIR="$SCRIPT_DIR/armgcc/$BUILD_DIR"
    export FLASH_CMD="$CHOSEN_PROBE"
else
    export BUILD_CMD=ninja
    export MAKEFILE_DIR="$SCRIPT_DIR/../../../build"
    export BINARY_DIR="$SCRIPT_DIR/../../../build/bsp/vee/scripts/armgcc/"
    export FLASH_CMD=flash-cmsis
fi

echo "Makefile directory: $MAKEFILE_DIR"
echo "Binary directory: $BINARY_DIR"
echo "Flash command: $FLASH_CMD"
