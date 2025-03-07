@ECHO off

REM 'clean.bat' implementation
REM 'clean.bat' is responsible for cleaning the project

CALL "%~dp0\set_project_env.bat"
 
CD "%MAKEFILE_DIR%\..\armgcc\"
IF %ERRORLEVEL% NEQ 0 (
	ECHO "Can not open %MAKEFILE_DIR%\..\armgcc\"
	EXIT /B %ERRORLEVEL%
)

CALL "clean.bat"
IF %ERRORLEVEL% NEQ 0 (
	ECHO "Failed to clean the environment"
	EXIT /B %ERRORLEVEL%
)

cd "%CURRENT_DIRECTORY%"