@ECHO off

REM 'set_project_env.bat' implementation 

REM 'set_project_env' is responsible for
REM - checking the availability of required environment variables
REM - setting project local variables for 'build.bat' and 'run.bat'

REM Change the following variables according to your project

REM Set "evk" for MIMXRT1170-EVK board or "evkb" for MIMXRT1170-EVKB board
SET CHOSEN_BOARD=evkb

REM Set 1 for RELEASE mode and 0 for DEBUG mode
SET CHOSEN_MODE=1

REM Set "flash_cmsisdap" for Linkserver probe or "flash" for J-Link probe
SET CHOSEN_PROBE=flash_cmsisdap


REM Don't change the following lines

IF "%BOARD%" == "" (
    SET BOARD=%CHOSEN_BOARD%
)
ECHO "Board: %BOARD%"

IF "%RELEASE%" == "" (
    SET RELEASE=%CHOSEN_MODE%
)
ECHO "Mode (1 for RELEASE, 0 for DEBUG): %RELEASE%"

REM Check toolchain path
IF "%ARMGCC_DIR%" == "" (
    SET ARM_FOLDER=arm-gnu-toolchain-13.2.Rel1-mingw-w64-i686-arm-none-eabi
    SET DEFAULT_ARMGCC_DIR="%USERPROFILE%\.mcuxpressotools\%ARM_FOLDER%"
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

SET SCRIPT_DIR=%~dp0
ECHO "Script directory: %SCRIPT_DIR%"

SET CURRENT_DIRECTORY=%CD%
ECHO "Current directory: %CURRENT_DIRECTORY%"

IF %RELEASE% == 1 (
    SET BUILD_DIR=flexspi_nor_sdram_release_%BOARD%
)
IF NOT %RELEASE% == 1 (
    SET BUILD_DIR=flexspi_nor_sdram_debug_%BOARD%
)

SET MAKEFILE_DIR=%SCRIPT_DIR%\sdk_makefile
ECHO "Makefile directory: %MAKEFILE_DIR%"

SET BINARY_DIR=%SCRIPT_DIR%\armgcc\%BUILD_DIR%
ECHO "Binary directory: %BINARY_DIR%"

SET BUILD_CMD=make

IF "%FLASH_CMD%" == "" (
    SET FLASH_CMD=%CHOSEN_PROBE%
)
ECHO "Flash command: %FLASH_CMD%"
