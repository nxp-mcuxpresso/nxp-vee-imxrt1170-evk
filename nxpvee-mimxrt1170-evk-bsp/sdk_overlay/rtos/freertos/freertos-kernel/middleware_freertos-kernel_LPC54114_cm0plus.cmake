#Description: FreeRTOS kernel; user_visible: True
include_guard(GLOBAL)
message("middleware_freertos-kernel component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/event_groups.c
    ${CMAKE_CURRENT_LIST_DIR}/croutine.c
    ${CMAKE_CURRENT_LIST_DIR}/list.c
    ${CMAKE_CURRENT_LIST_DIR}/portable/GCC/ARM_CM0/port.c
    ${CMAKE_CURRENT_LIST_DIR}/queue.c
    ${CMAKE_CURRENT_LIST_DIR}/stream_buffer.c
    ${CMAKE_CURRENT_LIST_DIR}/tasks.c
    ${CMAKE_CURRENT_LIST_DIR}/timers.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/portable/GCC/ARM_CM0
)


include(middleware_freertos-kernel_extension)
