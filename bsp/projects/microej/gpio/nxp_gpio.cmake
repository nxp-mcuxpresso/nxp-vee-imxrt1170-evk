include_guard()
message("nxp/gpio component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/LLGPIO_impl.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE    ${CMAKE_CURRENT_LIST_DIR}/inc)
