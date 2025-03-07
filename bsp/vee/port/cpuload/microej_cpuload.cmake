include_guard()
message("microej/cpuload component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/cpuload.c
	${CMAKE_CURRENT_LIST_DIR}/src/cpuload_impl_FreeRTOS.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE    ${CMAKE_CURRENT_LIST_DIR}/inc)
