include_guard()
message("microej/vg component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_BVI_impl_stub.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_FONT_impl_freetype.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_FONT_impl_stub.c
    #${CMAKE_CURRENT_LIST_DIR}/src/LLVG_FONT_PAINTER_freetype_vglite.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_MATRIX_impl.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_PAINTER_impl.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_PATH_impl_dual.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_PATH_impl_single.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_PATH_impl_stub.c
    #${CMAKE_CURRENT_LIST_DIR}/src/LLVG_RAW_impl.c
    #${CMAKE_CURRENT_LIST_DIR}/src/LLVG_vglite.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vg_drawing_stub.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vg_drawing.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vg_freetype_path.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vg_helper.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE    ${CMAKE_CURRENT_LIST_DIR}/inc)
