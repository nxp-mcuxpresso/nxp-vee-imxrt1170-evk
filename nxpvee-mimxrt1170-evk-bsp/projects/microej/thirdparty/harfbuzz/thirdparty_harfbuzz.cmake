# Copyright 2022-2023 MicroEJ Corp. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be found with this software.

include_guard()
message("microej/thirdparty/harfbuzz component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/harfbuzz.cc
    ${CMAKE_CURRENT_LIST_DIR}/src/hb-alloc.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/inc)
