include_guard()
message("microej/util component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/microej_async_worker.c
    ${CMAKE_CURRENT_LIST_DIR}/src/microej_pool.c
    ${CMAKE_CURRENT_LIST_DIR}/src/osal_FreeRTOS.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE    ${CMAKE_CURRENT_LIST_DIR}/inc)
