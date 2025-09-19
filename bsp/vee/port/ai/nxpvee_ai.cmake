# Copyright 2023 NXP
# SPDX-License-Identifier: BSD-3-Clause

include_guard()
message("nxpvee/ai component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/src/LLML_impl.c
  ${CMAKE_CURRENT_LIST_DIR}/src/LLML_microai_heap.c
  ${CMAKE_CURRENT_LIST_DIR}/src/LLML_tensorflow_lite.cpp
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/inc
)
