include_guard()
message("microej/ui component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/mej_math.c
    ${CMAKE_CURRENT_LIST_DIR}/src/ui_drawing_vglite.c
    ${CMAKE_CURRENT_LIST_DIR}/src/ui_drawing_vglite_path.c
    ${CMAKE_CURRENT_LIST_DIR}/src/ui_drawing_vglite_process.c
    ${CMAKE_CURRENT_LIST_DIR}/src/ui_vglite.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE    ${CMAKE_CURRENT_LIST_DIR}/inc)
