#Description: FreeRTOS MPU wrappers; user_visible: False
include_guard(GLOBAL)
message("middleware_freertos-kernel_mpu_wrappers component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/portable/Common/mpu_wrappers.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
)

#OR Logic component
if(${MCUX_DEVICE} STREQUAL "MIMXRT633S_cm33")
    include(middleware_freertos-kernel_MIMXRT685S_cm33)
endif()
if(${MCUX_DEVICE} STREQUAL "MIMXRT555S_cm33")
    include(middleware_freertos-kernel_MIMXRT595S_cm33)
endif()
if(${MCUX_DEVICE} STREQUAL "LPC55S66_cm33_core0")
    include(middleware_freertos-kernel_LPC55S69_cm33_core0)
endif()
if(${MCUX_DEVICE} STREQUAL "LPC55S69_cm33_core0")
    include(middleware_freertos-kernel_LPC55S69_cm33_core0)
endif()
if(${MCUX_DEVICE} STREQUAL "MIMXRT533S_cm33")
    include(middleware_freertos-kernel_MIMXRT595S_cm33)
endif()
if(${MCUX_DEVICE} STREQUAL "MIMXRT685S_cm33")
    include(middleware_freertos-kernel_MIMXRT685S_cm33)
endif()
if(${MCUX_DEVICE} STREQUAL "MIMXRT595S_cm33")
    include(middleware_freertos-kernel_MIMXRT595S_cm33)
endif()

