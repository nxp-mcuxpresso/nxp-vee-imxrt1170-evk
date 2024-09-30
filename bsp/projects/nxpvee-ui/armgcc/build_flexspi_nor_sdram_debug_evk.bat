SET CMD=ninja

if not exist flexspi_nor_sdram_debug_evk mkdir flexspi_nor_sdram_debug_evk
cd flexspi_nor_sdram_debug_evk
cmake --preset flexspi_nor_sdram_debug_evk %CMAKE_OPTS% %_tail% ..
%CMD% 2> build_log.txt
