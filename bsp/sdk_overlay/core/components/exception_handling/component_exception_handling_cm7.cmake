#Description: Component exception_handling_cm7; user_visible: True
include_guard(GLOBAL)
message("component_exception_handling_cm7 component is included.")

if(CONFIG_USE_driver_common AND CONFIG_USE_utility_debug_console)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/cm7/fsl_component_exception_handling.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/.
)

else()

message(SEND_ERROR "component_exception_handling_cm7 dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()
