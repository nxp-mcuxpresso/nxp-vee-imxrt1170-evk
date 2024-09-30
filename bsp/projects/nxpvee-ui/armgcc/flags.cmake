IF(NOT DEFINED FPU)
    SET(FPU "-mfloat-abi=hard -mfpu=fpv5-d16")
ENDIF()

IF(NOT DEFINED SPECS)
    SET(SPECS "--specs=nano.specs --specs=nosys.specs")
ENDIF()

IF(NOT DEFINED DEBUG_CONSOLE_CONFIG)
    SET(DEBUG_CONSOLE_CONFIG "-DSDK_DEBUGCONSOLE=1")
ENDIF()

# Select the revision of the i.MX RT1170 EVK your are using:
#       - If set: use MIMXRT1170-EVKB revision.
#       - If commented: use MIMXRT1170-EVK revision.
IF(NOT DEFINED MIMXRT1170_EVKB)
    SET(MIMXRT1170_EVKB 1)
ENDIF()

IF(DEFINED MIMXRT1170_EVKB)
    SET(X_LINKER_FLAG "-Xlinker")
    SET(DEF_SIM_FLAG "--defsym=__use_flash64MB__=1")
ENDIF()

# To enable the WIFI support uncomment the following line.
# SET(ENABLE_WIFI 1)

IF(DEFINED ENABLE_WIFI)
    SET(ENABLE_SDIO_FLAG "-DSDIO_ENABLED")
    SET(ENABLE_WIFI_FLAG "-DENABLE_WIFI")
ELSE()
    SET(ENABLE_SD_ENABLED "-DSD_ENABLED")
ENDIF()

SET(MICROUI_VGLITE_FLAGS "-DVG_DRIVER_SINGLE_THREAD")


SET(ASM_COMMON_FLAGS_FLEXSPI_NOR_SDRAM " \
    -D__STARTUP_CLEAR_BSS \
    -D__STARTUP_INITIALIZE_RAMFUNCTION \
    -D__STARTUP_INITIALIZE_NONCACHEDATA \
    -mcpu=cortex-m7 \
    -mthumb \
    ${FPU} \
")
SET(CMAKE_ASM_FLAGS_FLEXSPI_NOR_SDRAM_DEBUG " \
    ${CMAKE_ASM_FLAGS_FLEXSPI_NOR_SDRAM_DEBUG} \
    ${ASM_COMMON_FLAGS_FLEXSPI_NOR_SDRAM} \
    -DDEBUG \
")
SET(CMAKE_ASM_FLAGS_FLEXSPI_NOR_SDRAM_RELEASE " \
    ${CMAKE_ASM_FLAGS_FLEXSPI_NOR_SDRAM_RELEASE} \
    ${ASM_COMMON_FLAGS_FLEXSPI_NOR_SDRAM} \
    -DNDEBUG \
")


SET(C_COMMON_FLAGS_FLEXSPI_NOR_SDRAM " \
    -include ${ProjDirPath}/../bsp/app_config.h \
    -DXIP_EXTERNAL_FLASH=1 \
    -DXIP_BOOT_HEADER_ENABLE=1 \
    -DXIP_BOOT_HEADER_DCD_ENABLE=0 \
    -DXIP_BOOT_HEADER_XMCD_ENABLE=1 \
    -DUSE_SDRAM \
    -DDATA_SECTION_IS_CACHEABLE=1 \
    -DCPU_MIMXRT1176DVMAA_cm7 \
    -DCUSTOM_VGLITE_MEMORY_CONFIG=1 \
    -DFSL_SDK_DRIVER_QUICK_ACCESS_ENABLE=1 \
    -DVG_COMMAND_CALL=1 \
    -DVG_TARGET_FAST_CLEAR=0 \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DCACHE_MODE_WRITE_THROUGH=1 \
    -DCRYPTO_USE_DRIVER_CAAM \
    -DSDK_OS_FREE_RTOS \
    -DMCUXPRESSO_SDK \
    -DVG_BLIT_WORKAROUND=0 \
    -DLLUI_GC_SUPPORTED_FORMATS=2 \
    ${ENABLE_SDIO_FLAG} \
    ${ENABLE_WIFI_FLAG} \
    -DFSL_FEATURE_PHYKSZ8081_USE_RMII50M_MODE \
    -DEIQ_GUI_PRINTF \
    -DTF_LITE_STATIC_MEMORY \
    -DCMSIS_NN \
    -DARM_MATH_CM7 \
    -D__FPU_PRESENT=1 \
    -DSDK_I2C_BASED_COMPONENT_USED \
    -DPRINTF_FLOAT_ENABLE=1 \
    -DPRINTF_ADVANCED_ENABLE=1 \
    -mcpu=cortex-m7 \
    -Wall \
    -mthumb \
    -MMD \
    -MP \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -fno-builtin \
    -mapcs \
    -std=gnu99 \
    ${X_LINKER_FLAG} \
    ${DEF_SIM_FLAG} \
    ${FPU} \
    ${DEBUG_CONSOLE_CONFIG} \
    ${MICROUI_VGLITE_FLAGS} \
    -DFT2_BUILD_LIBRARY \
    -DHAVE_CONFIG_H \
    -DLWIP_ENET_FLEXIBLE_CONFIGURATION \
    -DFSL_SDK_ENABLE_DRIVER_CACHE_CONTROL=1 \
    -DLWIP_TIMEVAL_PRIVATE=0 \
    -DLWIP_DISABLE_PBUF_POOL_SIZE_SANITY_CHECKS=1 \
    -DDEMO_SDCARD \
    ${ENABLE_SD_ENABLED} \
    -DSYSTICKS_EVENTS_DISABLE \
")
SET(CMAKE_C_FLAGS_FLEXSPI_NOR_SDRAM_DEBUG " \
    ${CMAKE_C_FLAGS_FLEXSPI_NOR_SDRAM_DEBUG} \
    ${C_COMMON_FLAGS_FLEXSPI_NOR_SDRAM} \
    -DDEBUG \
    -g \
    -O0 \
")
SET(CMAKE_C_FLAGS_FLEXSPI_NOR_SDRAM_RELEASE " \
    ${CMAKE_C_FLAGS_FLEXSPI_NOR_SDRAM_RELEASE} \
    ${C_COMMON_FLAGS_FLEXSPI_NOR_SDRAM} \
    -DNDEBUG \
    -Os \
")


SET(CXX_COMMON_FLAGS_FLEXSPI_NOR_SDRAM " \
    -include ${ProjDirPath}/../bsp/app_config.h \
    -DCPU_MIMXRT1176DVMAA_cm7 \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DCACHE_MODE_WRITE_THROUGH=1 \
    -DCRYPTO_USE_DRIVER_CAAM \
    ${ENABLE_SDIO} \
    -DMCUXPRESSO_SDK \
    -DCUSTOM_VGLITE_MEMORY_CONFIG=1 \
    -DEIQ_GUI_PRINTF \
    -DTF_LITE_STATIC_MEMORY \
    -DCMSIS_NN \
    -DARM_MATH_CM7 \
    -D__FPU_PRESENT=1 \
    -mcpu=cortex-m7 \
    -Wall \
    -mthumb \
    -MMD \
    -MP \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -fno-builtin \
    -mapcs \
    -fno-rtti \
    -fno-exceptions \
    ${FPU} \
    ${DEBUG_CONSOLE_CONFIG} \
    ${MICROUI_VGLITE_FLAGS} \
")
SET(CMAKE_CXX_FLAGS_FLEXSPI_NOR_SDRAM_DEBUG " \
    ${CMAKE_CXX_FLAGS_FLEXSPI_NOR_SDRAM_DEBUG} \
    ${CXX_COMMON_FLAGS_FLEXSPI_NOR_SDRAM} \
    -DDEBUG \
    -g \
    -O0 \
")
SET(CMAKE_CXX_FLAGS_FLEXSPI_NOR_SDRAM_RELEASE " \
    ${CMAKE_CXX_FLAGS_FLEXSPI_NOR_SDRAM_RELEASE} \
    ${CXX_COMMON_FLAGS_FLEXSPI_NOR_SDRAM} \
    -DNDEBUG \
    -Os \
")


SET(EXE_LINKER_COMMON_FLAGS_FLEXSPI_NOR_SDRAM " \
    -mcpu=cortex-m7 \
    -Wall \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -fno-builtin \
    -u _printf_float \
    -mthumb \
    -mapcs \
    -Xlinker \
    --gc-sections \
    -Xlinker \
    -static \
    -Xlinker \
    -z \
    -Xlinker \
    muldefs \
    -Xlinker \
    -Map=${PROJECT_NAME}.map \
    -Wl,--print-memory-usage \
    ${FPU} \
    ${SPECS} \
    -T${ProjDirPath}/MIMXRT1176xxxxx_cm7_flexspi_nor_sdram.ld -static \
")
SET(CMAKE_EXE_LINKER_FLAGS_FLEXSPI_NOR_SDRAM_DEBUG " \
    ${CMAKE_EXE_LINKER_FLAGS_FLEXSPI_NOR_SDRAM_DEBUG} \
    ${EXE_LINKER_COMMON_FLAGS_FLEXSPI_NOR_SDRAM} \
    -g \
")
SET(CMAKE_EXE_LINKER_FLAGS_FLEXSPI_NOR_SDRAM_RELEASE " \
    ${CMAKE_EXE_LINKER_FLAGS_FLEXSPI_NOR_SDRAM_RELEASE} \
    ${EXE_LINKER_COMMON_FLAGS_FLEXSPI_NOR_SDRAM} \
")
