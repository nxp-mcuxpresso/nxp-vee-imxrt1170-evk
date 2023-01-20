#Description: FreeRTOS heap 1; user_visible: False
include_guard(GLOBAL)
message("middleware_freertos-kernel_heap_1 component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/portable/MemMang/heap_1.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
)

#OR Logic component
if(${MCUX_DEVICE} STREQUAL "MK22F12810")
    include(middleware_freertos-kernel_MK22F51212)
endif()
if(${MCUX_DEVICE} STREQUAL "MK22F51212")
    include(middleware_freertos-kernel_MK22F51212)
endif()
if(${MCUX_DEVICE} STREQUAL "MK02F12810")
    include(middleware_freertos-kernel_MK22F51212)
endif()
if(${MCUX_DEVICE} STREQUAL "MK22F25612")
    include(middleware_freertos-kernel_MK22F51212)
endif()
if(${MCUX_DEVICE} STREQUAL "MK64F12")
    include(middleware_freertos-kernel_MK64F12)
endif()

