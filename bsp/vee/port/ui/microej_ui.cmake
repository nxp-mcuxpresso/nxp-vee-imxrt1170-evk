include_guard()
message("microej/ui component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/LLDW_PAINTER_impl.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLUI_DISPLAY_HEAP_impl.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLUI_DISPLAY_impl.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLUI_LED_impl.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLUI_PAINTER_impl.c
    ${CMAKE_CURRENT_LIST_DIR}/src/microui_event_decoder.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLUI_INPUT_impl.c
    ${CMAKE_CURRENT_LIST_DIR}/src/touch_manager.c
    ${CMAKE_CURRENT_LIST_DIR}/src/touch_helper.c
    ${CMAKE_CURRENT_LIST_DIR}/src/event_generator.c
    ${CMAKE_CURRENT_LIST_DIR}/src/framerate_impl_FreeRTOS.c
    ${CMAKE_CURRENT_LIST_DIR}/src/framerate.c
    ${CMAKE_CURRENT_LIST_DIR}/src/ui_drawing.c
    ${CMAKE_CURRENT_LIST_DIR}/src/ui_font_drawing.c
    ${CMAKE_CURRENT_LIST_DIR}/src/ui_image_drawing.c
    ${CMAKE_CURRENT_LIST_DIR}/src/ui_drawing_stub.c
    ${CMAKE_CURRENT_LIST_DIR}/src/ui_rect_util.c
    ${CMAKE_CURRENT_LIST_DIR}/src/ui_display_brs.c
    ${CMAKE_CURRENT_LIST_DIR}/src/ui_display_brs_legacy.c
    ${CMAKE_CURRENT_LIST_DIR}/src/ui_display_brs_single.c
    ${CMAKE_CURRENT_LIST_DIR}/src/ui_display_brs_predraw.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE    ${CMAKE_CURRENT_LIST_DIR}/inc)
