# CROSS COMPILER SETTING
SET(CMAKE_SYSTEM_NAME Generic)
CMAKE_MINIMUM_REQUIRED (VERSION 3.10.0)

# THE VERSION NUMBER
SET (Tutorial_VERSION_MAJOR 1)
SET (Tutorial_VERSION_MINOR 0)
SET(CONFIG_USE_driver_enet 1) 

# ENABLE ASM
ENABLE_LANGUAGE(ASM)

SET(CMAKE_STATIC_LIBRARY_PREFIX)
SET(CMAKE_STATIC_LIBRARY_SUFFIX)

SET(CMAKE_EXECUTABLE_LIBRARY_PREFIX)
SET(CMAKE_EXECUTABLE_LIBRARY_SUFFIX)

# CURRENT DIRECTORY
SET(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR})

SET(EXECUTABLE_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})
SET(LIBRARY_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})


project(nxpvee_ui)

set(MCUX_BUILD_TYPES flexspi_nor_sdram_debug flexspi_nor_sdram_release sdram_debug sdram_release)

set(MCUX_SDK_PROJECT_NAME nxpvee_ui.elf)

if (NOT DEFINED MicroEjRootDirPath)
    SET(MicroEjRootDirPath ${ProjDirPath}/../../microej)
endif()

if (NOT DEFINED SdkOverlayRootDirPath)
    SET(SdkOverlayRootDirPath ${ProjDirPath}/../../../sdk_overlay/)
endif()

if (NOT DEFINED SdkRootDirPath)
    SET(SdkRootDirPath ${ProjDirPath}/../../../../../mcux-sdk/core/)
endif()

if (NOT DEFINED SdkMiddlewareDirPath)
    SET(SdkMiddlewareDirPath ${ProjDirPath}/../../../../../mcux-sdk/middleware/)
endif()

if (NOT DEFINED RtosRootDirPath)
    SET(RtosRootDirPath ${ProjDirPath}/../../../../../mcux-sdk/rtos/)
endif()


include(${ProjDirPath}/flags.cmake)

include(${ProjDirPath}/config.cmake)

include(${SdkMiddlewareDirPath}/coreHTTP/httpFilePaths.cmake)

add_executable(${MCUX_SDK_PROJECT_NAME}
    "${ProjDirPath}/../app/nxpvee_ui.c"
    "${ProjDirPath}/../bsp/board.c"
    "${ProjDirPath}/../bsp/board.h"
    "${ProjDirPath}/../bsp/clock_config.c"
    "${ProjDirPath}/../bsp/clock_config.h"
    "${ProjDirPath}/../bsp/dcd.c"
    "${ProjDirPath}/../bsp/dcd.h"
    "${ProjDirPath}/../bsp/display_support.c"
    "${ProjDirPath}/../bsp/display_support.h"
    "${ProjDirPath}/../bsp/evkmimxrt1170_connect_cm4_cm7side.jlinkscript"
    "${ProjDirPath}/../bsp/FreeRTOSConfig.h"
    "${ProjDirPath}/../bsp/pin_mux.c"
    "${ProjDirPath}/../bsp/pin_mux.h"
    "${ProjDirPath}/../bsp/simple_gfx_app_imp.c"
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
    ${ProjDirPath}/../bsp
    ${MicroEjRootDirPath}/
    ${MicroEjRootDirPath}/core/inc
    ${MicroEjRootDirPath}/fs/inc
    ${MicroEjRootDirPath}/net/inc
    ${MicroEjRootDirPath}/platform/inc
    ${MicroEjRootDirPath}/platform/inc/intern
    ${MicroEjRootDirPath}/ssl/inc
    ${MicroEjRootDirPath}/security/inc
    ${MicroEjRootDirPath}/thirdparty/freetype/inc
    ${MicroEjRootDirPath}/thirdparty/harfbuzz/inc
    ${MicroEjRootDirPath}/ui/inc
    ${MicroEjRootDirPath}/util/inc
    ${MicroEjRootDirPath}/vg/inc
    ${MicroEjRootDirPath}/vglite_support
    ${MicroEjRootDirPath}/vglite_window
    ${HTTP_INCLUDE_PUBLIC_DIRS}
)

# mbedTLS Config
target_compile_definitions(${MCUX_SDK_PROJECT_NAME}  PRIVATE MBEDTLS_CONFIG_FILE="microej_mbedtls_config.h")

set(CMAKE_MODULE_PATH
    ${MicroEjRootDirPath}/core
    ${MicroEjRootDirPath}/fs
    ${MicroEjRootDirPath}/net
    ${MicroEjRootDirPath}/thirdparty/freetype
    ${MicroEjRootDirPath}/thirdparty/harfbuzz
    ${MicroEjRootDirPath}/ui
    ${MicroEjRootDirPath}/util
    ${MicroEjRootDirPath}/vg
    ${MicroEjRootDirPath}/vglite_support
    ${MicroEjRootDirPath}/vglite_window
    ${ProjDirPath}/../../common/bsp/sdmmc
    ${ProjDirPath}/../../common/segger
    ${SdkOverlayRootDirPath}/components/phy/mdio/enet
    ${SdkOverlayRootDirPath}/middleware/vglite
    ${SdkOverlayRootDirPath}/middleware/vglite
    ${SdkOverlayRootDirPath}/rtos/freertos/freertos-kernel
    ${SdkOverlayRootDirPath}/rtos/freertos/freertos-kernel
    ${SdkMiddlewareDirPath}/coreHTTP
    ${SdkMiddlewareDirPath}/fatfs
    ${SdkMiddlewareDirPath}/lwip
    ${SdkMiddlewareDirPath}/mbedtls
    ${SdkMiddlewareDirPath}/sdmmc
    ${SdkRootDirPath}/boards/evkmimxrt1170/xip
    ${SdkRootDirPath}/boards/evkmimxrt1170/xmcd
    ${SdkRootDirPath}/CMSIS/Core/Include
    ${SdkRootDirPath}/CMSIS/Core/Include
    ${SdkRootDirPath}/CMSIS/DSP
    ${SdkRootDirPath}/components/gpio
    ${SdkRootDirPath}/components/gt911
    ${SdkRootDirPath}/components/lists
    ${SdkRootDirPath}/components/osa
    ${SdkRootDirPath}/components/phy
    ${SdkRootDirPath}/components/phy/device/phyrtl8211f
    ${SdkRootDirPath}/components/serial_manager
    ${SdkRootDirPath}/components/silicon_id
    ${SdkRootDirPath}/components/silicon_id/socs/imxrt1170
    ${SdkRootDirPath}/components/uart
    ${SdkRootDirPath}/components/video
    ${SdkRootDirPath}/components/video/display
    ${SdkRootDirPath}/components/video/display/dc
    ${SdkRootDirPath}/components/video/display/dc/dsi_cmd
    ${SdkRootDirPath}/components/video/display/dc/elcdif
    ${SdkRootDirPath}/components/video/display/dc/lcdifv2
    ${SdkRootDirPath}/components/video/display/fbdev
    ${SdkRootDirPath}/components/video/display/hx8394
    ${SdkRootDirPath}/components/video/display/mipi_dsi_cmd
    ${SdkRootDirPath}/components/video/display/rm68191
    ${SdkRootDirPath}/components/video/display/rm68200
    ${SdkRootDirPath}/devices/MIMXRT1176
    ${SdkRootDirPath}/devices/MIMXRT1176/drivers
    ${SdkRootDirPath}/devices/MIMXRT1176/drivers/cm7
    ${SdkRootDirPath}/devices/MIMXRT1176/utilities
    ${SdkRootDirPath}/devices/MIMXRT1176/xip
    ${SdkRootDirPath}/drivers/caam
    ${SdkRootDirPath}/drivers/common
    ${SdkRootDirPath}/drivers/dmamux
    ${SdkRootDirPath}/drivers/edma
    ${SdkRootDirPath}/drivers/elcdif
    ${SdkRootDirPath}/drivers/enet
    ${SdkRootDirPath}/drivers/igpio
    ${SdkRootDirPath}/drivers/lcdifv2
    ${SdkRootDirPath}/drivers/lpi2c
    ${SdkRootDirPath}/drivers/lpuart
    ${SdkRootDirPath}/drivers/mipi_dsi_split
    ${SdkRootDirPath}/drivers/usdhc
    ${SdkRootDirPath}/middleware/vglite
    ${MicroEjRootDirPath}/security
    ${MicroEjRootDirPath}/ssl
    ${SdkRootDirPath}/utilities
    ${SdkRootDirPath}/utilities/assert
    ${SdkRootDirPath}/utilities/misc_utilities
)

# include modules
include(CMSIS_DSP_Source)
include(CMSIS_Include_core_cm)
include(CMSIS_Include_core_cm)
include(component_igpio_adapter)
include(component_lists)
include(component_lpuart_adapter)
include(component_osa)
include(component_osa_free_rtos)
include(component_serial_manager)
include(component_serial_manager_uart)
include(component_silicon_id_MIMXRT1176_cm7)
include(device_CMSIS)
include(device_startup_MIMXRT1176_cm7)
include(device_system_MIMXRT1176_cm7)
include(driver_anatop_ai)
include(driver_cache_armv7_m7)
include(driver_caam)
include(driver_clock)
include(driver_common)
include(driver_dcdc_soc)
include(driver_dc-fb-common)
include(driver_dc-fb-dsi-cmd)
include(driver_dc-fb-elcdif)
include(driver_dc-fb-lcdifv2)
include(driver_display-common)
include(driver_display-hx8394)
include(driver_display-mipi-dsi-cmd)
include(driver_display-rm68191)
include(driver_display-rm68200)
include(driver_edma)
include(driver_elcdif)
include(driver_enet)
include(driver_fbdev)
include(driver_gt911)
include(driver_iomuxc)
include(driver_lcdifv2)
include(driver_lpi2c)
include(driver_lpi2c_freertos)
include(driver_lpuart)
include(driver_memory)
include(driver_mipi_dsi_split)
include(driver_phy-device-rtl8211f)
include(driver_pmu_1)
include(driver_soc_mipi_csi2rx)
include(driver_soc_src)
include(driver_video-common)
include(driver_xip_board_evkmimxrt1170)
include(driver_xip_device)
include(driver_xmcd_evkmimxrt1170)
include(httpFilePaths)
include(microej_core)
include(microej_fs)
include(microej_net)
include(microej_security)
include(microej_ssl)
include(microej_ui)
include(microej_util)
include(microej_vg)
include(microej_vglite_support)
include(microej_vglite_window)
include(middleware_freertos-kernel_extension)
include(middleware_freertos-kernel_heap_4)
include(middleware_freertos-kernel_MIMXRT1176_cm7)
include(middleware_lwip_enet_ethernetif_MIMXRT1176_cm7)
include(middleware_mbedtls_MIMXRT1176_cm7)
include(middleware_mbedtls_port_ksdk)
include(middleware_mbedtls_rt2)
include(middleware_vglite_MIMXRT1176_cm7)
include(utilities_misc_utilities_MIMXRT1176_cm7)
include(utility_assert)
include(utility_debug_console)
include(utility_segger)
include(thirdparty_freetype)
include(thirdparty_harfbuzz)

IF(NOT DEFINED TARGET_LINK_SYSTEM_LIBRARIES)
    SET(TARGET_LINK_SYSTEM_LIBRARIES "-lm -lc -lgcc -lnosys")
ENDIF()

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--start-group)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE ${TARGET_LINK_SYSTEM_LIBRARIES})

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE ${MicroEjRootDirPath}/platform/lib/microejruntime.a)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE ${MicroEjRootDirPath}/platform/lib/microejapp.o)

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--end-group)
