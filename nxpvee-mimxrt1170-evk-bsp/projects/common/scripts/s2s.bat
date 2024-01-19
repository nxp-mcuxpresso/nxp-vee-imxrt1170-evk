@echo off
set S2S_TTY=%1
set S2S_PORT=%2

netstat -an | findstr /RC:":%S2S_PORT% .*LISTENING"
if /I %errorlevel% NEQ 0 (
    cd MIMXRT1170-evk_platform-CM7hardfp_GCC48-2.1.0/source
    start "s2s.bat" cmd /k java ^
        -Djava.library.path="resources/os/Windows64" ^
        -cp "tools/*" ^
        com.is2t.serialsockettransmitter.SerialToSocketTransmitterBatch ^
        -port "%S2S_TTY%" ^
        -baudrate 115200 -databits 8 -parity none -stopbits 1 ^
        -hostPort "%S2S_PORT%"
)
