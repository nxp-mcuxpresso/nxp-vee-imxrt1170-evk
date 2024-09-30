#Description: FreeRTOS cm33 secure port; user_visible: False
include_guard(GLOBAL)
message("middleware_freertos-kernel_cm33_secure_port component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/portable/GCC/ARM_CM33/non_secure/port.c
    ${CMAKE_CURRENT_LIST_DIR}/portable/GCC/ARM_CM33/non_secure/portasm.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/portable/GCC/ARM_CM33/non_secure
    ${CMAKE_CURRENT_LIST_DIR}/portable/GCC/ARM_CM33/secure
)


include(middleware_freertos-kernel_mpu_wrappers)
