@ECHO off

REM 'build.bat' implementation for GCC toolchain

REM 'build.bat' is responsible for producing the executable file 
REM then copying this executable file to the current directory where it has been executed to a file named 'application.out'

CALL "%~dp0\set_project_env.bat"
IF %ERRORLEVEL% NEQ 0 (
	ECHO "Error during set_project_env.bat"
	EXIT /B %ERRORLEVEL%
)

CD %MAKEFILE_DIR%
IF %ERRORLEVEL% NEQ 0 (
	ECHO "Can not open %MAKEFILE_DIR%"
	EXIT /B %ERRORLEVEL%
)

%BUILD_CMD% %CMAKE_OPTS%
IF %ERRORLEVEL% NEQ 0 (
	ECHO "Failed to build the firmware"
	exit /B %ERRORLEVEL%
)

ECHO "Copy %BINARY_DIR%\npavee.{elf,hex,bin} into %CURRENT_DIRECTORY%\application.{out,hex,bin}"

copy /Y %BINARY_DIR%\npavee.elf %CURRENT_DIRECTORY%\application.out
copy /Y %BINARY_DIR%\npavee.hex %CURRENT_DIRECTORY%\application.hex
copy /Y %BINARY_DIR%\npavee.bin %CURRENT_DIRECTORY%\application.bin

cd %CURRENT_DIRECTORY%
