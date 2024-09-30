@ECHO off

REM 'set_project_env.bat' implementation 

REM 'set_project_env' is responsible for
REM - checking the availability of required environment variables
REM - setting project local variables for 'build.bat' and 'run.bat'

REM Change the following variables according to your project

REM Set "evk" for MIMXRT1170-EVK board or "evkb" for MIMXRT1170-EVKB board
IF "%BOARD%" == "" (
    SET BOARD=evkb
)
ECHO "Chosen board: %BOARD%"

REM Set 1 for RELEASE mode and 0 for DEBUG mode
IF "%RELEASE%" == "" (
    SET RELEASE=1
)
ECHO "Chosen mode (1 for RELEASE, 0 for DEBUG): %RELEASE%"

REM Activate/Deactivate compilation optons. Set "-DENABLE_XX=1" to enable and "-DENABLE_XX=0" to disable.
REM Following compilation options are configurable:
REM   - ENABLE_AI: Activate/Deactivate the AI support. (Deactivate by default)
IF "%CMAKE_OPTS%" == "" (
    SET CMAKE_OPTS=CMAKE_OPTS="-DENABLE_AI=0"
)
ECHO "Chosen compilation options: %CMAKE_OPTS%"

REM Set "flash_cmsisdap" for Linkserver probe or "flash" for J-Link probe
IF "%FLASH_CMD%" == "" (
    SET FLASH_CMD=flash_cmsisdap
)
ECHO "Chosen flash command: %FLASH_CMD%"

REM Check toolchain path
SET DEFAULT_ARMGCC_DIR=%USERPROFILE%\.mcuxpressotools\arm-gnu-toolchain-13.2.Rel1-mingw-w64-i686-arm-none-eabi
IF "%ARMGCC_DIR%" == "" (
    IF EXIST "%DEFAULT_ARMGCC_DIR%" (
        SET ARMGCC_DIR=%DEFAULT_ARMGCC_DIR%
        ECHO "ARMGCC_DIR not set, default location used: %DEFAULT_ARMGCC_DIR%"
    ) ELSE (
        ECHO "ERROR: ARMGCC_DIR not set"
        EXIT /B 1
    )
) ELSE (
    ECHO "ARMGCC_DIR=%ARMGCC_DIR%"
)

REM Check the CMake minimum version required
SET CMAKE_MAJOR_REQUIRED=3
SET CMAKE_MINOR_REQUIRED=27
FOR /f "tokens=3" %%A in ('cmake --version') DO (
    FOR /f "tokens=1,2 delims=." %%B in ("%%A") DO (
        IF %%B LSS %CMAKE_MAJOR_REQUIRED% (
            ECHO "ERROR: CMake version (%%A) is too old (%CMAKE_MAJOR_REQUIRED%.%CMAKE_MINOR_REQUIRED% or later required), terminating.\n"
            EXIT /B 1
        ) ELSE IF %%B EQU %CMAKE_MAJOR_REQUIRED% (
            IF %%C LSS %CMAKE_MINOR_REQUIRED% (
                ECHO "ERROR: CMake version (%%A) is too old (%CMAKE_MAJOR_REQUIRED%.%CMAKE_MINOR_REQUIRED% or later required), terminating.\n"
                EXIT /B 1
            )
        )
    )
)


REM Don't change the following variables

IF %RELEASE% == 1 (
    SET BUILD_DIR="flexspi_nor_sdram_release_%BOARD%\"
)
IF NOT %RELEASE% == 1 (
    SET BUILD_DIR="flexspi_nor_sdram_debug_%BOARD%\"
)

SET SCRIPT_DIR="%~dp0"
ECHO "Script directory: %SCRIPT_DIR%"

SET MAKEFILE_DIR="%~dp0\..\..\nxpvee-ui\sdk_makefile"
ECHO "Makefile directory: %MAKEFILE_DIR%"

SET BINARY_DIR="%MAKEFILE_DIR%\..\armgcc\%BUILD_DIR%"
ECHO "Build directory: %BINARY_DIR%"

SET CURRENT_DIRECTORY=%CD%
ECHO "Current directory: %CURRENT_DIRECTORY%"
