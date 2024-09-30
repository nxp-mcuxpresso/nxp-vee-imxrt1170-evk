#!/bin/bash

cmake --preset flexspi_nor_sdram_debug_evk "${@:2}" .
cmake --build --preset flexspi_nor_sdram_debug_evk 2>&1 | tee build_log.txt
