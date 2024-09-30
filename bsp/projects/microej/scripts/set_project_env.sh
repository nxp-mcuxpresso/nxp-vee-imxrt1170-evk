#!/bin/bash

# 'set_project_env.bat' implementation 

# 'set_project_env' is responsible for
# - checking the availability of required environment variables
# - setting project local variables for 'build.sh' and 'run.sh'

# Change the following variables according to your project

# Set "evk" for MIMXRT1170-EVK board or "evkb" for MIMXRT1170-EVKB board
if [ ! -v BOARD ]; then
    export BOARD=evkb
fi
echo "Chosen board: $BOARD"

# Set 1 for RELEASE mode and 0 for DEBUG mode
if [ ! -v RELEASE ]; then
    export RELEASE=1
fi
echo "Chosen mode (1 for RELEASE, 0 for DEBUG): $RELEASE"

# Activate/Deactivate compilation optons. Set "-DENABLE_XX=1" to enable and "-DENABLE_XX=0" to disable.
# Following compilation options are configurable:
#   - ENABLE_AI: Activate/Deactivate the AI support. (Deactivate by default)
if [ ! -v CMAKE_OPTS ]; then
    export CMAKE_OPTS="-DENABLE_AI=0"
fi
echo "Chosen compilation options: $CMAKE_OPTS"

# Set "flash_cmsisdap" for Linkserver probe or "flash" for J-Link probe
if [ ! -v FLASH_CMD ]; then
    export FLASH_CMD=flash_cmsisdap
fi
echo "Chosen flash command: $FLASH_CMD"

# Check toolchain path
if [ ! -v ARMGCC_DIR ]; then
    export DEFAULT_ARMGCC_DIR="~/.mcuxpressotools/arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi"
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

# Don't change the following variables

if [ $RELEASE = 1 ]; then
    export BUILD_DIR="flexspi_nor_sdram_release_$BOARD/"
else
    export BUILD_DIR="flexspi_nor_sdram_debug_$BOARD/"
fi

export SCRIPT_DIR="$(dirname $(realpath $0))"
echo "Script directory: $SCRIPT_DIR"

export MAKEFILE_DIR="$SCRIPT_DIR/../../nxpvee-ui/sdk_makefile"
echo "Makefile directory: $MAKEFILE_DIR"

export BINARY_DIR="$MAKEFILE_DIR/../armgcc/$BUILD_DIR"
echo "Build directory: $BINARY_DIR"

export CURRENT_DIRECTORY=$(pwd)
echo "Current directory: $CURRENT_DIRECTORY"
