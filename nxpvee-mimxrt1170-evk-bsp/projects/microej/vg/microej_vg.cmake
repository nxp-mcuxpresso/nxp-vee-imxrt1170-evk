# Copyright 2022-2023 MicroEJ Corp. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be found with this software.

include_guard()
message("microej/vg component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/freetype_bitmap_helper.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_BVI_impl.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_FONT_freetype.c
    # ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_FONT_PAINTER_freetype_bitmap.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_FONT_PAINTER_freetype_vglite.c
    # ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_FONT_stub.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_GRADIENT_impl.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_impl.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_MATRIX_impl.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_PATH_impl.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_PATH_PAINTER_vglite.c
    #${CMAKE_CURRENT_LIST_DIR}/src/LLVG_PATH_stub.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_RAW_impl.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_vglite.c
    ${CMAKE_CURRENT_LIST_DIR}/src/microvg_helper.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE    ${CMAKE_CURRENT_LIST_DIR}/inc)
