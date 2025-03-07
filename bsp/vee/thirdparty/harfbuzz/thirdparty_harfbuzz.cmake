include_guard()
message("microej/thirdparty/harfbuzz component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/harfbuzz.cc
    ${CMAKE_CURRENT_LIST_DIR}/src/hb-alloc.c
)

# Hide warnings for Harfbuzz component
file(GLOB HarfbuzzFiles
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cc
    ${CMAKE_CURRENT_LIST_DIR}/src/*.c
    ${CMAKE_CURRENT_LIST_DIR}/inc/*.hh
    ${CMAKE_CURRENT_LIST_DIR}/inc/*.h
)
set_source_files_properties(${HarfbuzzFiles} PROPERTIES COMPILE_FLAGS "-w")

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/inc)
