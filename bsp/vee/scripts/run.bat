@ECHO off

REM 'run.bat' implementation
REM 'run.bat' is responsible for flashing the executable file on the target device

CALL "%~dp0\set_project_env.bat"

CD %MAKEFILE_DIR%
IF %ERRORLEVEL% NEQ 0 (
	ECHO "Can not open %MAKEFILE_DIR%"
	EXIT /B %ERRORLEVEL%
)

%BUILD_CMD% %CMAKE_OPTS% %FLASH_CMD%
IF %ERRORLEVEL% NEQ 0 (
	ECHO "Failed to flash the board"
	EXIT /B %ERRORLEVEL%
)

cd "%CURRENT_DIRECTORY%"
