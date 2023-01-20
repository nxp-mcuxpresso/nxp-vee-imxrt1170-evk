@echo off

REM Copyright 2019-2022 MicroEJ Corp. All rights reserved.
REM Use of this source code is governed by a BSD-style license that can be found with this software.

REM 'build.bat' implementation for GCC GnuToolChain

REM 'build.bat' is responsible for producing the executable file 
REM then copying this executable file to the current directory where it has been executed to a file named 'application.out'

CALL "%~dp0\set_project_env.bat"
IF %ERRORLEVEL% NEQ 0 (
	exit /B %ERRORLEVEL%
)

@echo on

SET CURRENT_DIRECTORY=%CD%

cd "%~dp0..\..\nxpvee-ui\sdk_makefile"
make RELEASE=1
IF %ERRORLEVEL% NEQ 0 (
	exit /B %ERRORLEVEL%
)

copy /Y ..\armgcc\flexspi_nor_sdram_release\nxpvee_ui.elf %CURRENT_DIRECTORY%\application.out
copy /Y ..\armgcc\flexspi_nor_sdram_release\nxpvee_ui.hex %CURRENT_DIRECTORY%\application.hex
copy /Y ..\armgcc\flexspi_nor_sdram_release\nxpvee_ui.bin %CURRENT_DIRECTORY%\application.bin

copy /Y ..\armgcc\flexspi_nor_sdram_release\nxpvee_ui.bin %~dp0\application.bin

cd "%CURRENT_DIRECTORY%"
