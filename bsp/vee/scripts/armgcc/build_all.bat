SET CMD=ninja

if not exist flexspi_nor_sdram_debug_evkb mkdir flexspi_nor_sdram_debug_evkb
cd flexspi_nor_sdram_debug_evkb
cmake --preset flexspi_nor_sdram_debug_evkb %_tail%  ..
%CMD%
cd ..\

if not exist flexspi_nor_sdram_release_evkb mkdir flexspi_nor_sdram_release_evkb
cd flexspi_nor_sdram_release_evkb
cmake --preset flexspi_nor_sdram_release_evkb %_tail%  ..
%CMD%
cd ..\

if not exist flexspi_nor_sdram_debug_evk mkdir flexspi_nor_sdram_debug_evk
cd flexspi_nor_sdram_debug_evk
cmake --preset flexspi_nor_sdram_debug_evk %_tail%  ..
%CMD%
cd ..\

if not exist flexspi_nor_sdram_release_evk mkdir flexspi_nor_sdram_release_evk
cd flexspi_nor_sdram_release_evk
cmake --preset flexspi_nor_sdram_release_evk %_tail%  ..
%CMD%

IF "%1" == "" ( pause )
