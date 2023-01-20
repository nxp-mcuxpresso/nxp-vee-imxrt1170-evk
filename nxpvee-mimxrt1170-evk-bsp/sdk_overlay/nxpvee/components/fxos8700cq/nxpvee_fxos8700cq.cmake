# Copyright 2023 NXP
# SPDX-License-Identifier: BSD-3-Clause

include_guard()
message("nxpvee_fxos8700cq component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/nxpvee_fxos.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/.
)

list(APPEND CMAKE_MODULE_PATH ${SdkRootDirPath}/components/fxos8700cq)

include(driver_fxos8700cq)
