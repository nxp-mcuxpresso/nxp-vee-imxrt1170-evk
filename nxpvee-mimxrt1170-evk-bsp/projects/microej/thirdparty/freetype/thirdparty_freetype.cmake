# Copyright 2022 MicroEJ Corp. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be found with this software.

include_guard()
message("microej/vg component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/wrappers/ft_base_wrapper.c
    ${CMAKE_CURRENT_LIST_DIR}/src/wrappers/ft_otf_wrapper.c
    ${CMAKE_CURRENT_LIST_DIR}/src/wrappers/ft_ttf_wrapper.c
    ${CMAKE_CURRENT_LIST_DIR}/src/wrappers/ft_vector_wrapper.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/inc)
