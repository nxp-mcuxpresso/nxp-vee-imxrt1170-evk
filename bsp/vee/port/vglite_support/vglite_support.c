/*
 * Copyright (c) 2019, NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright 2020-2023 MicroEJ Corp. This file has been modified by MicroEJ Corp.
 */

/*
 * @file
 * @brief MicroEJ MicroUI library low level API: implementation over VG-Lite
 * @author MicroEJ Developer Team
 * @version 4.0.0
 *
 * see vglite_support.h
 */

#include "vglite_support.h"
#include "fsl_clock.h"
// #include "fsl_power.h"
#include "vg_lite.h"
#include "vg_lite_platform.h"
#include "bsp_util.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MAX_CONTIGUOUS_SIZE         0x200000
#define VG_LITE_COMMAND_BUFFER_SIZE (128 << 10)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

static uint32_t registerMemBase = 0x41800000;
static uint32_t gpu_mem_base    = 0x0;

/*
 * In case custom VGLite memory parameters are used, the application needs to
 * allocate and publish the VGLite heap base, its size and the size of the
 * command buffer(s) using the following global variables:
 */
extern void *vglite_heap_base;
extern uint32_t vglite_heap_size;
extern uint32_t vglite_cmd_buff_size;

#if (CUSTOM_VGLITE_MEMORY_CONFIG == 0)
/* VGLite driver heap */
AT_NONCACHEABLE_SECTION_ALIGN(uint8_t contiguous_mem[MAX_CONTIGUOUS_SIZE], 64);

void *vglite_heap_base        = &contiguous_mem;
uint32_t vglite_heap_size     = MAX_CONTIGUOUS_SIZE;
uint32_t vglite_cmd_buff_size = VG_LITE_COMMAND_BUFFER_SIZE;
#endif /* CUSTOM_VGLITE_MEMORY_CONFIG */

/*******************************************************************************
 * Code
 ******************************************************************************/

/*
 * @brief Notifies the CCO MicroUI-VGLite about the GPU interrupt. When this CCO
 * is not installed / available, the default function is used and does nothing.
 */
BSP_DECLARE_WEAK_FCNT void UI_VGLITE_IRQHandler(void) {
	// does nothing by default
}

void GPU2D_IRQHandler(void)
{
    vg_lite_IRQHandler();
    UI_VGLITE_IRQHandler();
}

// modified by MicroEJ
status_t BOARD_InitVGliteClock(void)
{
    const clock_root_config_t gc355ClockConfig = {
        .clockOff = false,
        .mux      = kCLOCK_GC355_ClockRoot_MuxVideoPllOut, /*!< 984MHz */
        .div      = 2,
    };

    CLOCK_SetRootClock(kCLOCK_Root_Gc355, &gc355ClockConfig);

    CLOCK_GetRootClockFreq(kCLOCK_Root_Gc355);

    CLOCK_EnableClock(kCLOCK_Gpu2d);

    NVIC_SetPriority(GPU2D_IRQn, 3);

    EnableIRQ(GPU2D_IRQn);

    return kStatus_Success;
}

// added by MicroEJ
status_t BOARD_StartVGliteClock(void)
{
    // CLOCK_AttachClk(kMAIN_CLK_to_GPU_CLK);
    // CLOCK_SetClkDiv(kCLOCK_DivGpuClk, 1);
    // CLOCK_EnableClock(kCLOCK_Gpu);

    return kStatus_Success;
}

// added by MicroEJ
status_t BOARD_StopVGliteClock(void)
{
    // CLOCK_AttachClk(kNONE_to_GPU_CLK);
    // CLOCK_SetClkDiv(kCLOCK_DivGpuClk, 0);
    // CLOCK_DisableClock(kCLOCK_Gpu);

    return kStatus_Success;
}

status_t BOARD_PrepareVGLiteController(void)
{
    status_t status;

    status = BOARD_InitVGliteClock();

    if (kStatus_Success != status)
    {
        return status;
    }

    vg_lite_init_mem(registerMemBase, gpu_mem_base, vglite_heap_base, vglite_heap_size);

    vg_lite_set_command_buffer_size(vglite_cmd_buff_size);

    return kStatus_Success;
}
