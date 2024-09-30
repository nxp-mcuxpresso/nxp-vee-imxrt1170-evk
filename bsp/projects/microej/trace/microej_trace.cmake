include_guard()
message("microej/trace component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/LLMJVM_MONITOR_sysview.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LLTRACE_sysview.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE    ${CMAKE_CURRENT_LIST_DIR}/inc)
