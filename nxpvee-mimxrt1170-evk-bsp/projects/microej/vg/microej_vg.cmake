include_guard()
message("microej/vg component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_FONT_freetype.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_FONT_PAINTER_freetype_vglite.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_FONT_stub.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_GRADIENT_impl.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_impl.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_MATRIX_impl.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_PATH_impl.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_PATH_stub.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_RAW_impl.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_vglite.c
    ${CMAKE_CURRENT_LIST_DIR}/src/microvg_helper.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_BVI_stub.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_PAINTER_impl.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vg_drawing_bvi.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vg_drawing_stub.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vg_drawing_vglite.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vg_drawing.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE    ${CMAKE_CURRENT_LIST_DIR}/inc)
