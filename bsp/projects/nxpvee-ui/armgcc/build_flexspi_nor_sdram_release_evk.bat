SET CMD=ninja

if not exist flexspi_nor_sdram_release_evk mkdir flexspi_nor_sdram_release_evk
cd flexspi_nor_sdram_release_evk
cmake --preset flexspi_nor_sdram_release_evk %CMAKE_OPTS% %_tail% ..
%CMD% 2> build_log.txt
