# CROSS COMPILER SETTING
SET(CMAKE_SYSTEM_NAME Generic)
CMAKE_MINIMUM_REQUIRED (VERSION 3.27.0)

# ENABLE ASM
ENABLE_LANGUAGE(ASM)

SET(CMAKE_STATIC_LIBRARY_PREFIX)
SET(CMAKE_STATIC_LIBRARY_SUFFIX)

SET(CMAKE_EXECUTABLE_LIBRARY_PREFIX)
SET(CMAKE_EXECUTABLE_LIBRARY_SUFFIX)

# CURRENT DIRECTORY
SET(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Git)

project(npavee)

file (STRINGS "${ProjDirPath}/../../../nvee_version.txt" VEE_VERSION)


set(MCUX_SDK_PROJECT_NAME npavee.elf)
cmake_path(GET MCUX_SDK_PROJECT_NAME STEM MCUX_SDK_PROJECT_NAME_NO_EXT)

if (NOT DEFINED MicroEjRootDirPath)
    SET(MicroEjRootDirPath ${ProjDirPath}/../../../vee/)
endif()

if (NOT DEFINED SdkOverlayRootDirPath)
    SET(SdkOverlayRootDirPath ${ProjDirPath}/../../../sdk_overlay/)
endif()

if (NOT DEFINED VeeSdkRootDirPath)
    SET(VeeSdkRootDirPath ${ProjDirPath}/../../../mcux-sdk/core/)
endif()

if (NOT DEFINED SdkMiddlewareDirPath)
    SET(SdkMiddlewareDirPath ${ProjDirPath}/../../../mcux-sdk/middleware/)
endif()

if (NOT DEFINED RtosRootDirPath)
    SET(RtosRootDirPath ${ProjDirPath}/../../../mcux-sdk/rtos/)
endif()

include(${ProjDirPath}/flags.cmake)

include(${ProjDirPath}/config.cmake)

include(${SdkMiddlewareDirPath}/coreHTTP/httpFilePaths.cmake)

execute_process(COMMAND
  "${GIT_EXECUTABLE}" describe --always --abbrev=8
  WORKING_DIRECTORY "${ProjDirPath}"
  OUTPUT_VARIABLE GIT_SHA_1
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

configure_file("${MicroEjRootDirPath}/src/main/tree_version.c.in" "${CMAKE_CURRENT_BINARY_DIR}/tree_version.c" @ONLY)


if (MIMXRT1170_EVKB)
    set(JLINKSCRIPT_SRC "${MicroEjRootDirPath}/src/bsp/evkbmimxrt1170_connect_cm4_cm7side.jlinkscript")
else()
    set(JLINKSCRIPT_SRC "${MicroEjRootDirPath}/src/bsp/evkmimxrt1170_connect_cm4_cm7side.jlinkscript")
endif(MIMXRT1170_EVKB)

if (ENABLE_WIFI)
    SET(WifiExtraFiles ${MicroEjRootDirPath}/src/bsp/wifi_bt_config.c ${MicroEjRootDirPath}/src/bsp/wifi_bt_config.h ${MicroEjRootDirPath}/src/bsp/wifi_config.h)
else()
    SET(WifiExtraFiles "")
endif(ENABLE_WIFI)

if (ENABLE_AI)
    set(ADDITIONAL_SRC "${MicroEjRootDirPath}/src/main/model_cifarnet_ops.cpp")
else()
    set(ADDITIONAL_SRC "")
endif(ENABLE_AI)


add_executable(${MCUX_SDK_PROJECT_NAME}
    "${MicroEjRootDirPath}/src/main/npavee.c"
    "${MicroEjRootDirPath}/src/main/npavee.h"
    "${MicroEjRootDirPath}/src/bsp/app_config.h"
    "${MicroEjRootDirPath}/src/bsp/board.c"
    "${MicroEjRootDirPath}/src/bsp/board.h"
    "${MicroEjRootDirPath}/src/bsp/clock_config.c"
    "${MicroEjRootDirPath}/src/bsp/clock_config.h"
    "${MicroEjRootDirPath}/src/bsp/core_test_natives.c"
    "${MicroEjRootDirPath}/src/bsp/dcd.c"
    "${MicroEjRootDirPath}/src/bsp/dcd.h"
    "${MicroEjRootDirPath}/src/bsp/display_support.c"
    "${MicroEjRootDirPath}/src/bsp/display_support.h"
    "${MicroEjRootDirPath}/src/bsp/event_queue_test_natives.c"
    "${MicroEjRootDirPath}/src/bsp/FreeRTOSConfig.h"
    "${MicroEjRootDirPath}/src/bsp/gettimeofday_impl.c"
    "${MicroEjRootDirPath}/src/bsp/pin_mux.c"
    "${MicroEjRootDirPath}/src/bsp/pin_mux.h"
    "${MicroEjRootDirPath}/src/bsp/simple_gfx_app_imp.c"
    "${MicroEjRootDirPath}/src/bsp/ui_vglite_config.c"
    "${MicroEjRootDirPath}/common/bsp/sdmmc/ffconf.h"
    "${MicroEjRootDirPath}/common/bsp/sdmmc/sdmmc_config.c"
    "${MicroEjRootDirPath}/common/bsp/sdmmc/sdmmc_config.h"
    "${CMAKE_CURRENT_BINARY_DIR}/tree_version.c"
    "${WifiExtraFiles}"
    "${ADDITIONAL_SRC}"
    "${JLINKSCRIPT_SRC}"
)

IF(NOT DEFINED MICROEJ_DEST_OVERRIDE)
    SET( MICROEJ_DEST ${MicroEjRootDirPath})
    SET( MICROEJ_DEST_LIB "/lib")
ELSE()
    SET(MICROEJ_DEST ${CMAKE_BINARY_DIR})
    SET( MICROEJ_DEST_LIB "")
ENDIF()

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
    ${MicroEjRootDirPath}/src/main
    ${MicroEjRootDirPath}/src/bsp
    ${MicroEjRootDirPath}/
    ${MICROEJ_DEST}/inc
    ${MicroEjRootDirPath}/port/core/inc
    ${MicroEjRootDirPath}/port/cpuload/inc
    ${MicroEjRootDirPath}/port/ecom-wifi/inc
    ${MicroEjRootDirPath}/port/event-queue/inc
    ${MicroEjRootDirPath}/port/fs/inc
    ${MicroEjRootDirPath}/port/net/inc
    ${MicroEjRootDirPath}/port/inc
    ${MicroEjRootDirPath}/port/inc/intern
    ${MicroEjRootDirPath}/port/security/inc
    ${MicroEjRootDirPath}/port/ssl/inc
    ${MicroEjRootDirPath}/thirdparty/freetype/inc
    ${MicroEjRootDirPath}/thirdparty/harfbuzz/inc
    ${MicroEjRootDirPath}/port/trace/inc
    ${MicroEjRootDirPath}/port/ui/inc
    ${MicroEjRootDirPath}/port/ui_vglite/inc
    ${MicroEjRootDirPath}/port/util/inc
    ${MicroEjRootDirPath}/port/vg/inc
    ${MicroEjRootDirPath}/port/vg_vglite/inc
    ${MicroEjRootDirPath}/port/vglite_support
    ${MicroEjRootDirPath}/port/vglite_window
    ${HTTP_INCLUDE_PUBLIC_DIRS}
)

# mbedTLS Config
target_compile_definitions(${MCUX_SDK_PROJECT_NAME} PRIVATE MBEDTLS_CONFIG_FILE="microej_mbedtls_config.h")

set_source_files_properties("${MicroEjRootDirPath}/src/bsp/FreeRTOSConfig.h" PROPERTIES COMPONENT_CONFIG_FILE "middleware_freertos-kernel_template")
set_source_files_properties("${MicroEjRootDirPath}/common/bsp/sdmmc/ffconf.h" PROPERTIES COMPONENT_CONFIG_FILE "middleware_fatfs_template_sd")
set_source_files_properties("${MicroEjRootDirPath}/common/bsp/sdmmc/sdmmc_config.c" PROPERTIES COMPONENT_CONFIG_FILE "middleware_sdmmc_usdhc_template")
set_source_files_properties("${MicroEjRootDirPath}/common/bsp/sdmmc/sdmmc_config.h" PROPERTIES COMPONENT_CONFIG_FILE "middleware_sdmmc_usdhc_template")

set(CMAKE_MODULE_PATH
    ${MicroEjRootDirPath}/port/core
    ${MicroEjRootDirPath}/port/cpuload
    ${MicroEjRootDirPath}/port/event-queue
    ${MicroEjRootDirPath}/port/fs
    ${MicroEjRootDirPath}/port/gpio
    ${MicroEjRootDirPath}/port/kf
    ${MicroEjRootDirPath}/port/net
    ${MicroEjRootDirPath}/thirdparty/freetype
    ${MicroEjRootDirPath}/thirdparty/harfbuzz
    ${MicroEjRootDirPath}/port/security
    ${MicroEjRootDirPath}/port/ssl
    ${MicroEjRootDirPath}/port/ui
    ${MicroEjRootDirPath}/port/ui_vglite
    ${MicroEjRootDirPath}/port/util
    ${MicroEjRootDirPath}/port/vg
    ${MicroEjRootDirPath}/port/vg_vglite
    ${MicroEjRootDirPath}/port/vglite_support
    ${MicroEjRootDirPath}/port/vglite_window
    ${MicroEjRootDirPath}/common/bsp/sdmmc
    ${SdkOverlayRootDirPath}/core/components/exception_handling
    ${SdkOverlayRootDirPath}/middleware/vglite
    ${SdkOverlayRootDirPath}/rtos/freertos/freertos-kernel
    ${SdkMiddlewareDirPath}/coreHTTP
)

# include device
include(${VeeSdkRootDirPath}/devices/MIMXRT1176/all_lib_device.cmake)

# include modules
include(component_exception_handling_cm7)
include(httpFilePaths)
include(microej_core)
include(microej_cpuload)
include(microej_event_queue)
include(microej_fs)
include(microej_kf)
include(microej_net)
include(microej_security)
include(microej_ssl)
include(microej_ui)
include(microej_ui_vglite)
include(microej_util)
include(microej_vg)
include(microej_vg_vglite)
include(microej_vglite_support)
include(microej_vglite_window)
include(middleware_vglite_MIMXRT1176_cm7)
include(nxp_gpio)
include(utility_sdmmc)
include(thirdparty_freetype)
include(thirdparty_harfbuzz)


SET(TARGET_LINK_SYSTEM_LIBRARIES "-lm -lc -lgcc -lnosys")

if (ENABLE_SYSTEM_VIEW)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
        ${MicroEjRootDirPath}/common/segger
        ${MicroEjRootDirPath}/port/trace
    )

    include(microej_trace)
    include(utility_segger)
endif(ENABLE_SYSTEM_VIEW)

if (ENABLE_WIFI)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
        ${VeeSdkRootDirPath}/components/wifi_bt_module
        ${MicroEjRootDirPath}/port/ecom-wifi
    )

    include(component_wifi_bt_module_tx_pwr_limits)
    include(microej_ecom-wifi)
endif(ENABLE_WIFI)

if (ENABLE_AI)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
        ${MicroEjRootDirPath}/port/ai
    )

    include(nxpvee_ai)
endif(ENABLE_AI)

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--start-group)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE ${TARGET_LINK_SYSTEM_LIBRARIES})

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE ${MICROEJ_DEST}${MICROEJ_DEST_LIB}/microejruntime.a)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE ${MICROEJ_DEST}${MICROEJ_DEST_LIB}/microejapp.o)

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--end-group)

add_custom_command(TARGET ${MCUX_SDK_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Obinary ./${MCUX_SDK_PROJECT_NAME} ./${MCUX_SDK_PROJECT_NAME_NO_EXT}.bin)
add_custom_command(TARGET ${MCUX_SDK_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Oihex ./${MCUX_SDK_PROJECT_NAME} ./${MCUX_SDK_PROJECT_NAME_NO_EXT}.hex)

set_target_properties(${MCUX_SDK_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES ./${MCUX_SDK_PROJECT_NAME_NO_EXT}.bin)
set_target_properties(${MCUX_SDK_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES ./${MCUX_SDK_PROJECT_NAME_NO_EXT}.hex)
