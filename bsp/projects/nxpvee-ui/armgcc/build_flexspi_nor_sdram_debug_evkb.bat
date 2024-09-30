SET CMD=ninja

if not exist flexspi_nor_sdram_debug_evkb mkdir flexspi_nor_sdram_debug_evkb
cd flexspi_nor_sdram_debug_evkb
cmake --preset flexspi_nor_sdram_debug_evkb %CMAKE_OPTS% %_tail% ..
%CMD% 2> build_log.txt
