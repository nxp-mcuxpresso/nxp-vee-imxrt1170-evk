# Copyright 2023 NXP
# SPDX-License-Identifier: BSD-3-Clause

include_guard()
message("microej/ecom-wifi component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/src/WIFI_Common.c
  ${CMAKE_CURRENT_LIST_DIR}/src/LLECOM_wifi_helper.c
  ${CMAKE_CURRENT_LIST_DIR}/src/LLECOM_WIFI_impl.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE    ${CMAKE_CURRENT_LIST_DIR}/inc)
