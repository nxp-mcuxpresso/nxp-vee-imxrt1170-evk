#!/bin/bash

cmake --preset flexspi_nor_sdram_debug_evkb "${@:2}" .
cmake --build --preset flexspi_nor_sdram_debug_evkb

cmake --preset flexspi_nor_sdram_release_evkb "${@:2}" .
cmake --build --preset flexspi_nor_sdram_release_evkb

cmake --preset flexspi_nor_sdram_debug_evk "${@:2}" .
cmake --build --preset flexspi_nor_sdram_debug_evk

cmake --preset flexspi_nor_sdram_release_evk "${@:2}" .
cmake --build --preset flexspi_nor_sdram_release_evk
