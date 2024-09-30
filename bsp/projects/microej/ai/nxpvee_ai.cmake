# Copyright 2023 NXP
# SPDX-License-Identifier: BSD-3-Clause

include_guard()
message("nxpvee/ai component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/src/LLAI_impl.c
  ${CMAKE_CURRENT_LIST_DIR}/src/TfLiteSupportImpl.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/TfLiteSupportInterface.cpp
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/inc
)
