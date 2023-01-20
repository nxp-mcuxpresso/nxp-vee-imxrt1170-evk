RD /s /Q CMakeFiles
DEL /s /Q /F Makefile cmake_install.cmake CMakeCache.txt
IF EXIST sdram_debug RD /s /Q sdram_debug
IF EXIST flexspi_nor_sdram_debug RD /s /Q flexspi_nor_sdram_debug
IF EXIST sdram_release RD /s /Q sdram_release
IF EXIST flexspi_nor_sdram_release RD /s /Q flexspi_nor_sdram_release
