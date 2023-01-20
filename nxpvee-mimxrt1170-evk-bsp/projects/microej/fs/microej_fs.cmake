# Copyright 2023 NXP
# SPDX-License-Identifier: BSD-3-Clause

include_guard()
message("microej/fs component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/src/LLFS_File_impl.c
  ${CMAKE_CURRENT_LIST_DIR}/src/fs_helper_fatfs.c
  ${CMAKE_CURRENT_LIST_DIR}/src/LLFS_impl.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE    ${CMAKE_CURRENT_LIST_DIR}/inc)

include(middleware_fatfs_sd)
include(middleware_sdmmc_host_usdhc_freertos)
include(utility_sdmmc)
