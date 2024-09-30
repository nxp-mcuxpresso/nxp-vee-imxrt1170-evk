SET CMD=ninja

if not exist flexspi_nor_sdram_release_evkb mkdir flexspi_nor_sdram_release_evkb
cd flexspi_nor_sdram_release_evkb
cmake --preset flexspi_nor_sdram_release_evkb %CMAKE_OPTS% %_tail% ..
%CMD% 2> build_log.txt
