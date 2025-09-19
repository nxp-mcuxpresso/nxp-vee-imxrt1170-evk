include_guard()
message("microej/event-queue component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/src/LLEVENT_impl.c
  ${CMAKE_CURRENT_LIST_DIR}/src/LLEVENT.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE    ${CMAKE_CURRENT_LIST_DIR}/inc)