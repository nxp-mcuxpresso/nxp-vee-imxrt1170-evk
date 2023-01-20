/*
 * C
 *
 * Copyright 2020-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief MicroEJ MicroUI library low level API: implementation over VG-Lite
 * @author MicroEJ Developer Team
 * @version 4.0.0
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <LLUI_DISPLAY.h>

#include "display_dma.h"
#include "display_impl.h"
#include "vglite_window.h"

#include "fsl_edma.h"
#include "fsl_dmamux.h"
#include "fsl_debug_console.h"

#ifdef SEGGER_DEBUG
#include "SEGGER_SYSVIEW.h"
#include "SEGGER_debug_def.h"
#define MARKER_DMA 0xdead0005
#endif

// -----------------------------------------------------------------------------
// Macros and Defines
// -----------------------------------------------------------------------------

#define DMA_TRANSFER_WIDTH 32 // 256 bits width transfers.
#define DMA_TRANSFER_BYTES_EACH_REQUEST (4 * 1024)

#define EXAMPLE_DMA                 DMA0
#define EXAMPLE_DMAMUX              DMAMUX0
#define CHANNEL4 (4)

// -----------------------------------------------------------------------------
// Global Variables
// -----------------------------------------------------------------------------

#if defined (DISPLAY_DMA_ENABLED) && (DISPLAY_DMA_ENABLED != 0)
static edma_handle_t g_EDMA_Handle;

edma_config_t userConfig;

#endif // DISPLAY_DMA_ENABLED != 0

// -----------------------------------------------------------------------------
// Internal function definitions
// -----------------------------------------------------------------------------

/*
 * @brief: Callback called when a DMA transfer is done
 *
 * @param[in] handle: Handle on the DMA transfer
 * @param[in] param: User parameters for the callback
 * @param[in] transfer_done: Flag indicating if the current transfer is done
 */
static void dma_callback(edma_handle_t *handle, void *param, bool transfer_done, uint32_t tcds);

// -----------------------------------------------------------------------------
// display_dma.h
// -----------------------------------------------------------------------------

#if defined (DISPLAY_DMA_ENABLED) && (DISPLAY_DMA_ENABLED != 0)
// See the header file for the function documentation
void DISPLAY_DMA_initialize(const framebuffer_t * framebuffers[]) {
    (void)framebuffers;
#if defined(FSL_FEATURE_DMAMUX_HAS_A_ON) && FSL_FEATURE_DMAMUX_HAS_A_ON
    DMAMUX_EnableAlwaysOn(EXAMPLE_DMAMUX, CHANNEL4, true);
#else
    DMAMUX_SetSource(EXAMPLE_DMAMUX, CHANNEL4, 63);
#endif /* FSL_FEATURE_DMAMUX_HAS_A_ON */
    DMAMUX_EnableChannel(EXAMPLE_DMAMUX, CHANNEL4);
    /* Configure EDMA one shot transfer */
    /*
     * userConfig.enableRoundRobinArbitration = false;
     * userConfig.enableHaltOnError = true;
     * userConfig.enableContinuousLinkMode = false;
     * userConfig.enableDebugMode = false;
     */
    EDMA_CreateHandle(&g_EDMA_Handle, EXAMPLE_DMA, CHANNEL4);
    EDMA_SetCallback(&g_EDMA_Handle, dma_callback, NULL);
}
#define TCD_QUEUE_SIZE             1U

// See the header file for the function documentation
void DISPLAY_DMA_start(framebuffer_t *dst,framebuffer_t *src, uint32_t size) {
    edma_transfer_config_t transferConfig;
	DISPLAY_IMPL_notify_dma_start();
#ifdef SEGGER_DEBUG
	SEGGER_SYSVIEW_MarkStart(MARKER_DMA);
#endif // SEGGER_DEBUG
	EDMA_PrepareTransfer(&transferConfig, 
/* void *srcAddr,                       */   src,
/* uint32_t srcWidth,                   */   DMA_TRANSFER_WIDTH,
/* void *destAddr,                      */   dst,
/* uint32_t destWidth,                  */   DMA_TRANSFER_WIDTH,
/* uint32_t bytesEachRequest,           */   DMA_TRANSFER_BYTES_EACH_REQUEST,
/* uint32_t transferBytes,              */   size, // (ymax - ymin + 1) * FRAME_BUFFER_STRIDE_BYTE, 
/* edma_transfer_type_t transferType);  */   kEDMA_MemoryToMemory);
    EDMA_SubmitTransfer(&g_EDMA_Handle, &transferConfig);
    EDMA_StartTransfer(&g_EDMA_Handle);
}

// -----------------------------------------------------------------------------
// Internal functions
// -----------------------------------------------------------------------------

// See the section 'Internal function definitions' for the function documentation
static void dma_callback(edma_handle_t *handle, void *param, bool transfer_done, uint32_t tcds)
{
	(void)handle;
	(void)param;
	(void)tcds;

	if (transfer_done)
	{
		uint8_t it = interrupt_enter();
		LLUI_DISPLAY_flushDone(true);
		DISPLAY_IMPL_notify_dma_stop();
#ifdef SEGGER_DEBUG
		SEGGER_SYSVIEW_MarkStop(MARKER_DMA);
#endif // SEGGER_DEBUG
		interrupt_leave(it);
	}
}

#endif // DISPLAY_DMA_ENABLED != 0

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------
