# Copyright 2023 NXP
# SPDX-License-Identifier: BSD-3-Clause

include_guard()
message("sdmmc component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/sdcard_helper.c"
    "${CMAKE_CURRENT_LIST_DIR}/sdmmc_config.c"
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/.
)
