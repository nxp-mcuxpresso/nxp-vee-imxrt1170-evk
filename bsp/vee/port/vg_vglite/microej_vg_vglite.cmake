include_guard()
message("microej/vg_vglite component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_GRADIENT_impl_vglite.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLVG_impl_vglite.c
    ${CMAKE_CURRENT_LIST_DIR}/src/ui_drawing_bvi.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vg_drawing_bvi.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vg_drawing_vglite.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vg_drawing_vglite_image.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vg_path_vglite.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vg_vglite_helper.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE    ${CMAKE_CURRENT_LIST_DIR}/inc)
