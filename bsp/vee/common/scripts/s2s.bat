@echo off
set S2S_TTY=%1
set S2S_PORT=%2

netstat -an | findstr /RC:":%S2S_PORT% .*LISTENING"
if /I %errorlevel% NEQ 0 (
    CD "%~dp0/../../../../"
    CALL "gradlew.bat" execTool ^
        --name=serialToSocketTransmitter ^
        --toolProperty=serail.to.socket.comm.port="%S2S_TTY%" ^
        --toolProperty=serail.to.socket.comm.baudrate="115200" ^
        --toolProperty=serail.to.socket.server.port="%S2S_PORT%"
)
