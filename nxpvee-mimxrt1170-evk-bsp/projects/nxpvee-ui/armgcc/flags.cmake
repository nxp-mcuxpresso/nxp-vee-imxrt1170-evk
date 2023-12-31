IF(NOT DEFINED FPU)  
    SET(FPU "-mfloat-abi=hard -mfpu=fpv5-d16")  
ENDIF()  

IF(NOT DEFINED SPECS)  
    SET(SPECS "--specs=nano.specs --specs=nosys.specs")  
ENDIF()  

IF(NOT DEFINED DEBUG_CONSOLE_CONFIG)  
    SET(DEBUG_CONSOLE_CONFIG "-DSDK_DEBUGCONSOLE=1")  
ENDIF()  

SET(CMAKE_ASM_FLAGS_FLEXSPI_NOR_SDRAM_DEBUG " \
    ${CMAKE_ASM_FLAGS_FLEXSPI_NOR_SDRAM_DEBUG} \
    -D__STARTUP_CLEAR_BSS \
    -D__STARTUP_INITIALIZE_RAMFUNCTION \
    -DDEBUG \
    -D__STARTUP_INITIALIZE_NONCACHEDATA \
    -mcpu=cortex-m7 \
    -mthumb \
    ${FPU} \
")
SET(CMAKE_ASM_FLAGS_FLEXSPI_NOR_SDRAM_RELEASE " \
    ${CMAKE_ASM_FLAGS_FLEXSPI_NOR_SDRAM_RELEASE} \
    -D__STARTUP_CLEAR_BSS \
    -D__STARTUP_INITIALIZE_RAMFUNCTION \
    -DNDEBUG \
    -D__STARTUP_INITIALIZE_NONCACHEDATA \
    -mcpu=cortex-m7 \
    -mthumb \
    ${FPU} \
")
SET(CMAKE_ASM_FLAGS_SDRAM_DEBUG " \
    ${CMAKE_ASM_FLAGS_SDRAM_DEBUG} \
    -D__STARTUP_CLEAR_BSS \
    -DDEBUG \
    -D__STARTUP_INITIALIZE_NONCACHEDATA \
    -mcpu=cortex-m7 \
    -mthumb \
    ${FPU} \
")
SET(CMAKE_ASM_FLAGS_SDRAM_RELEASE " \
    ${CMAKE_ASM_FLAGS_SDRAM_RELEASE} \
    -D__STARTUP_CLEAR_BSS \
    -DNDEBUG \
    -D__STARTUP_INITIALIZE_NONCACHEDATA \
    -mcpu=cortex-m7 \
    -mthumb \
    ${FPU} \
")
SET(CMAKE_C_FLAGS_FLEXSPI_NOR_SDRAM_DEBUG " \
    ${CMAKE_C_FLAGS_FLEXSPI_NOR_SDRAM_DEBUG} \
    -DXIP_EXTERNAL_FLASH=1 \
    -DXIP_BOOT_HEADER_ENABLE=1 \
    -DXIP_BOOT_HEADER_DCD_ENABLE=0 \
    -DXIP_BOOT_HEADER_XMCD_ENABLE=1 \
    -DFREESCALE_KSDK_BM \
    -DCACHE_MODE_WRITE_THROUGH=1 \
    -DCRYPTO_USE_DRIVER_CAAM \
    -DUSE_SDRAM \
    -DDATA_SECTION_IS_CACHEABLE=1 \
    -DDEBUG \
    -DCPU_MIMXRT1176DVMAA_cm7 \
    -DCUSTOM_VGLITE_MEMORY_CONFIG=1 \
    -DFSL_SDK_DRIVER_QUICK_ACCESS_ENABLE=1 \
    -DVG_COMMAND_CALL=1 \
    -DVG_TARGET_FAST_CLEAR=0 \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DSDK_OS_FREE_RTOS \
    -DMCUXPRESSO_SDK \
    -DVG_DRIVER_SINGLE_THREAD \
    -DVG_BLIT_WORKAROUND=0 \
    -DLLUI_GC_SUPPORTED_FORMATS=2 \
    -DSDK_I2C_BASED_COMPONENT_USED \
    -DPRINTF_FLOAT_ENABLE=1 \
    -DPRINTF_ADVANCED_ENABLE=1 \
    -DFSL_FEATURE_PHYKSZ8081_USE_RMII50M_MODE \
    -g \
    -O0 \
    -mcpu=cortex-m7 \
    -Wall \
    -mthumb \
    -MMD \
    -MP \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
    -fno-builtin \
    -mapcs \
    -std=gnu99 \
    ${FPU} \
    ${DEBUG_CONSOLE_CONFIG} \
    -DFT2_BUILD_LIBRARY \
    -DHAVE_CONFIG_H \
    -DLWIP_ENET_FLEXIBLE_CONFIGURATION \
    -DFSL_SDK_ENABLE_DRIVER_CACHE_CONTROL=1 \
    -DLWIP_TIMEVAL_PRIVATE=0 \
    -DLWIP_DISABLE_PBUF_POOL_SIZE_SANITY_CHECKS=1 \
    -DDEMO_SDCARD \
    -DSD_ENABLED \
    -DSYSTICKS_EVENTS_DISABLE \
")
SET(CMAKE_C_FLAGS_FLEXSPI_NOR_SDRAM_RELEASE " \
    ${CMAKE_C_FLAGS_FLEXSPI_NOR_SDRAM_RELEASE} \
    -DXIP_EXTERNAL_FLASH=1 \
    -DXIP_BOOT_HEADER_ENABLE=1 \
    -DXIP_BOOT_HEADER_DCD_ENABLE=0 \
    -DXIP_BOOT_HEADER_XMCD_ENABLE=1 \
    -DFREESCALE_KSDK_BM \
    -DCACHE_MODE_WRITE_THROUGH=1 \
    -DCRYPTO_USE_DRIVER_CAAM\
    -DUSE_SDRAM \
    -DDATA_SECTION_IS_CACHEABLE=1 \
    -DNDEBUG \
    -DCPU_MIMXRT1176DVMAA_cm7 \
    -DCUSTOM_VGLITE_MEMORY_CONFIG=1 \
    -DFSL_SDK_DRIVER_QUICK_ACCESS_ENABLE=1 \
    -DVG_COMMAND_CALL=1 \
    -DVG_TARGET_FAST_CLEAR=0 \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DFREESCALE_KSDK_BM \
    -DCACHE_MODE_WRITE_THROUGH=1 \
    -DCRYPTO_USE_DRIVER_CAAM \
    -DSDK_OS_FREE_RTOS \
    -DMCUXPRESSO_SDK \
    -DVG_DRIVER_SINGLE_THREAD \
    -DVG_BLIT_WORKAROUND=0 \
    -DLLUI_GC_SUPPORTED_FORMATS=2 \
    -DFSL_FEATURE_PHYKSZ8081_USE_RMII50M_MODE \
    -DSDK_I2C_BASED_COMPONENT_USED \
    -DPRINTF_FLOAT_ENABLE=1 \
    -DPRINTF_ADVANCED_ENABLE=1 \
    -Os \
    -mcpu=cortex-m7 \
    -Wall \
    -mthumb \
    -MMD \
    -MP \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
    -fno-builtin \
    -mapcs \
    -std=gnu99 \
    ${FPU} \
    ${DEBUG_CONSOLE_CONFIG} \
    -DFT2_BUILD_LIBRARY \
    -DHAVE_CONFIG_H \
    -DLWIP_ENET_FLEXIBLE_CONFIGURATION \
    -DFSL_SDK_ENABLE_DRIVER_CACHE_CONTROL=1 \
    -DLWIP_TIMEVAL_PRIVATE=0 \
    -DLWIP_DISABLE_PBUF_POOL_SIZE_SANITY_CHECKS=1 \
    -DDEMO_SDCARD \
    -DSD_ENABLED \
    -DSYSTICKS_EVENTS_DISABLE \
")
SET(CMAKE_C_FLAGS_SDRAM_DEBUG " \
    ${CMAKE_C_FLAGS_SDRAM_DEBUG} \
    -DUSE_SDRAM \
    -DDATA_SECTION_IS_CACHEABLE=1 \
    -DDEBUG \
    -DCPU_MIMXRT1176DVMAA_cm7 \
    -DCUSTOM_VGLITE_MEMORY_CONFIG=1 \
    -DFSL_SDK_DRIVER_QUICK_ACCESS_ENABLE=1 \
    -DVG_COMMAND_CALL=1 \
    -DVG_TARGET_FAST_CLEAR=0 \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DFREESCALE_KSDK_BM \
    -DCACHE_MODE_WRITE_THROUGH=1 \
    -DCRYPTO_USE_DRIVER_CAAM \
    -DSDK_OS_FREE_RTOS \
    -DMCUXPRESSO_SDK \
    -DVG_DRIVER_SINGLE_THREAD \
    -DVG_BLIT_WORKAROUND=0 \
    -DLLUI_GC_SUPPORTED_FORMATS=2 \
    -DFSL_FEATURE_PHYKSZ8081_USE_RMII50M_MODE \
    -DSDK_I2C_BASED_COMPONENT_USED \
    -DPRINTF_FLOAT_ENABLE=1 \
    -DPRINTF_ADVANCED_ENABLE=1 \
    -g \
    -O0 \
    -mcpu=cortex-m7 \
    -Wall \
    -mthumb \
    -MMD \
    -MP \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
    -fno-builtin \
    -mapcs \
    -std=gnu99 \
    ${FPU} \
    ${DEBUG_CONSOLE_CONFIG} \
    -DFT2_BUILD_LIBRARY \
    -DHAVE_CONFIG_H \
    -DLWIP_ENET_FLEXIBLE_CONFIGURATION \
    -DFSL_SDK_ENABLE_DRIVER_CACHE_CONTROL=1 \
    -DLWIP_TIMEVAL_PRIVATE=0 \
    -DLWIP_DISABLE_PBUF_POOL_SIZE_SANITY_CHECKS=1 \
    -DDEMO_SDCARD \
    -DSD_ENABLED \
    -DSYSTICKS_EVENTS_DISABLE \
")
SET(CMAKE_C_FLAGS_SDRAM_RELEASE " \
    ${CMAKE_C_FLAGS_SDRAM_RELEASE} \
    -DUSE_SDRAM \
    -DDATA_SECTION_IS_CACHEABLE=1 \
    -DNDEBUG \
    -DCPU_MIMXRT1176DVMAA_cm7 \
    -DCUSTOM_VGLITE_MEMORY_CONFIG=1 \
    -DFSL_SDK_DRIVER_QUICK_ACCESS_ENABLE=1 \
    -DVG_COMMAND_CALL=1 \
    -DVG_TARGET_FAST_CLEAR=0 \
    -DFREESCALE_KSDK_BM \
    -DCACHE_MODE_WRITE_THROUGH=1 \
    -DCRYPTO_USE_DRIVER_CAAM \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DFREESCALE_KSDK_BM \
    -DCACHE_MODE_WRITE_THROUGH=1 \
    -DCRYPTO_USE_DRIVER_CAAM\
    -DSDK_OS_FREE_RTOS \
    -DMCUXPRESSO_SDK \
    -DVG_DRIVER_SINGLE_THREAD \
    -DVG_BLIT_WORKAROUND=0 \
    -DLLUI_GC_SUPPORTED_FORMATS=2 \
    -DFSL_FEATURE_PHYKSZ8081_USE_RMII50M_MODE \
    -DSDK_I2C_BASED_COMPONENT_USED \
    -DPRINTF_FLOAT_ENABLE=1 \
    -DPRINTF_ADVANCED_ENABLE=1 \
    -Os \
    -mcpu=cortex-m7 \
    -Wall \
    -mthumb \
    -MMD \
    -MP \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
    -fno-builtin \
    -mapcs \
    -std=gnu99 \
    ${FPU} \
    ${DEBUG_CONSOLE_CONFIG} \
    -DFT2_BUILD_LIBRARY \
    -DHAVE_CONFIG_H \
    -DLWIP_ENET_FLEXIBLE_CONFIGURATION \
    -DFSL_SDK_ENABLE_DRIVER_CACHE_CONTROL=1 \
    -DLWIP_TIMEVAL_PRIVATE=0 \
    -DLWIP_DISABLE_PBUF_POOL_SIZE_SANITY_CHECKS=1 \
    -DDEMO_SDCARD \
    -DSD_ENABLED \
    -DSYSTICKS_EVENTS_DISABLE \
")
SET(CMAKE_CXX_FLAGS_FLEXSPI_NOR_SDRAM_DEBUG " \
    ${CMAKE_CXX_FLAGS_FLEXSPI_NOR_SDRAM_DEBUG} \
    -DDEBUG \
    -DCPU_MIMXRT1176DVMAA_cm7 \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DFREESCALE_KSDK_BM \
    -DCACHE_MODE_WRITE_THROUGH=1 \
    -DCRYPTO_USE_DRIVER_CAAM \
    -DMCUXPRESSO_SDK \
    -DCUSTOM_VGLITE_MEMORY_CONFIG=1 \
    -g \
    -O0 \
    -mcpu=cortex-m7 \
    -Wall \
    -mthumb \
    -MMD \
    -MP \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
    -fno-builtin \
    -mapcs \
    -fno-rtti \
    -fno-exceptions \
    ${FPU} \
    ${DEBUG_CONSOLE_CONFIG} \
")
SET(CMAKE_CXX_FLAGS_FLEXSPI_NOR_SDRAM_RELEASE " \
    ${CMAKE_CXX_FLAGS_FLEXSPI_NOR_SDRAM_RELEASE} \
    -DNDEBUG \
    -DCPU_MIMXRT1176DVMAA_cm7 \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DFREESCALE_KSDK_BM \
    -DCACHE_MODE_WRITE_THROUGH=1 \
    -DCRYPTO_USE_DRIVER_CAAM \
    -DMCUXPRESSO_SDK \
    -DCUSTOM_VGLITE_MEMORY_CONFIG=1 \
    -Os \
    -mcpu=cortex-m7 \
    -Wall \
    -mthumb \
    -MMD \
    -MP \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
    -fno-builtin \
    -mapcs \
    -fno-rtti \
    -fno-exceptions \
    ${FPU} \
    ${DEBUG_CONSOLE_CONFIG} \
")
SET(CMAKE_CXX_FLAGS_SDRAM_DEBUG " \
    ${CMAKE_CXX_FLAGS_SDRAM_DEBUG} \
    -DDEBUG \
    -DCPU_MIMXRT1176DVMAA_cm7 \
    -DFREESCALE_KSDK_BM \
    -DCACHE_MODE_WRITE_THROUGH=1 \
    -DCRYPTO_USE_DRIVER_CAAM \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DFREESCALE_KSDK_BM \
    -DCACHE_MODE_WRITE_THROUGH=1 \
    -DCRYPTO_USE_DRIVER_CAAM\
    -DMCUXPRESSO_SDK \
    -DCUSTOM_VGLITE_MEMORY_CONFIG=1 \
    -g \
    -O0 \
    -mcpu=cortex-m7 \
    -Wall \
    -mthumb \
    -MMD \
    -MP \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
    -fno-builtin \
    -mapcs \
    -fno-rtti \
    -fno-exceptions \
    ${FPU} \
    ${DEBUG_CONSOLE_CONFIG} \
")
SET(CMAKE_CXX_FLAGS_SDRAM_RELEASE " \
    ${CMAKE_CXX_FLAGS_SDRAM_RELEASE} \
    -DNDEBUG \
    -DCPU_MIMXRT1176DVMAA_cm7 \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DFREESCALE_KSDK_BM \
    -DCACHE_MODE_WRITE_THROUGH=1 \
    -DCRYPTO_USE_DRIVER_CAAM \
    -DMCUXPRESSO_SDK \
    -DCUSTOM_VGLITE_MEMORY_CONFIG=1 \
    -Os \
    -mcpu=cortex-m7 \
    -Wall \
    -mthumb \
    -MMD \
    -MP \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
    -fno-builtin \
    -mapcs \
    -fno-rtti \
    -fno-exceptions \
    ${FPU} \
    ${DEBUG_CONSOLE_CONFIG} \
")
SET(CMAKE_EXE_LINKER_FLAGS_FLEXSPI_NOR_SDRAM_DEBUG " \
    ${CMAKE_EXE_LINKER_FLAGS_FLEXSPI_NOR_SDRAM_DEBUG} \
    -g \
    -mcpu=cortex-m7 \
    -Wall \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
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
    -Map=${ProjDirPath}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}.map \
    -Wl,--print-memory-usage \
    ${FPU} \
    ${SPECS} \
    -T${ProjDirPath}/MIMXRT1176xxxxx_cm7_flexspi_nor_sdram.ld -static \
")
SET(CMAKE_EXE_LINKER_FLAGS_FLEXSPI_NOR_SDRAM_RELEASE " \
    ${CMAKE_EXE_LINKER_FLAGS_FLEXSPI_NOR_SDRAM_RELEASE} \
    -mcpu=cortex-m7 \
    -Wall \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
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
    -Map=${ProjDirPath}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}.map \
    -Wl,--print-memory-usage \
    ${FPU} \
    ${SPECS} \
    -T${ProjDirPath}/MIMXRT1176xxxxx_cm7_flexspi_nor_sdram.ld -static \
")
SET(CMAKE_EXE_LINKER_FLAGS_SDRAM_DEBUG " \
    ${CMAKE_EXE_LINKER_FLAGS_SDRAM_DEBUG} \
    -g \
    -mcpu=cortex-m7 \
    -Wall \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
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
    -Map=${ProjDirPath}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}.map \
    -Wl,--print-memory-usage \
    ${FPU} \
    ${SPECS} \
    -T${ProjDirPath}/MIMXRT1176xxxxx_cm7_sdram.ld -static \
")
SET(CMAKE_EXE_LINKER_FLAGS_SDRAM_RELEASE " \
    ${CMAKE_EXE_LINKER_FLAGS_SDRAM_RELEASE} \
    -mcpu=cortex-m7 \
    -Wall \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
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
    -Map=${ProjDirPath}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}.map \
    -Wl,--print-memory-usage \
    ${FPU} \
    ${SPECS} \
    -T${ProjDirPath}/MIMXRT1176xxxxx_cm7_sdram.ld -static \
")
