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

/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <LLUI_DISPLAY_impl.h>
#include "touch_manager.h"

#include <FreeRTOS.h>
#include <semphr.h>

#include "display_dma.h"
#include "display_utils.h"
#include "display_vglite.h"
#include "display_impl.h"
#include "framerate.h"

// -----------------------------------------------------------------------------
// Macros and Defines
// -----------------------------------------------------------------------------

#if (CUSTOM_VGLITE_MEMORY_CONFIG != 1)
#error "Application must be compiled with CUSTOM_VGLITE_MEMORY_CONFIG=1"
#else
#define VGLITE_COMMAND_BUFFER_SZ (128 * 1024)
#define VGLITE_HEAP_SZ 0x880000
#endif

#define DISPLAY_STACK_SIZE       (8 * 1024)
#define DISPLAY_TASK_PRIORITY    (tskIDLE_PRIORITY + 5)
#define DISPLAY_TASK_STACK_SIZE  (DISPLAY_STACK_SIZE / 4)

// -----------------------------------------------------------------------------
// Global Variables
// -----------------------------------------------------------------------------

#define AT_NONCACHEABLE_SDRAM_SECTION_INIT(var) __attribute__((section("SdramNonCacheable.init"))) var
#define AT_NONCACHEABLE_SDRAM_SECTION_ALIGN_INIT(var, alignbytes) \
    __attribute__((section("SdramNonCacheable.init"))) var __attribute__((aligned(alignbytes)))
#define AT_NONCACHEABLE_SDRAM_SECTION(var) __attribute__((section("SdramNonCacheable,\"aw\",%nobits @"))) var
#define AT_NONCACHEABLE_SDRAM_SECTION_ALIGN(var, alignbytes) \
    __attribute__((section("SdramNonCacheable,\"aw\",%nobits @"))) var __attribute__((aligned(alignbytes)))


/* Allocate the heap and set the command buffer(s) size */
AT_NONCACHEABLE_SDRAM_SECTION_ALIGN(uint8_t vglite_heap[VGLITE_HEAP_SZ], 64);

void *vglite_heap_base        = &vglite_heap;
uint32_t vglite_heap_size     = VGLITE_HEAP_SZ;
uint32_t vglite_cmd_buff_size = VGLITE_COMMAND_BUFFER_SZ;

/*
 * @brief: Semaphore to synchronize the display flush with MicroUI
 */
static SemaphoreHandle_t sync_flush;
static uint8_t* dirty_area_addr;	// Address of the source framebuffer
static int32_t dirty_area_ymin;	// Top-most coordinate of the area to synchronize
static int32_t dirty_area_ymax;	// Bottom-most coordinate of the area to synchronize

#define START_BUFFER_ADDRESS (0x83880000)
#define BUFFER_SIZE (0x1C2000)

// cppcheck-suppress [misra-c2012-9.3] array is fully initialized
const uint32_t s_frameBufferAddress[FRAME_BUFFER_COUNT] = {
		START_BUFFER_ADDRESS,
#if defined (FRAME_BUFFER_COUNT) && (FRAME_BUFFER_COUNT > 1)
		(START_BUFFER_ADDRESS + BUFFER_SIZE),
#endif
#if defined (FRAME_BUFFER_COUNT) && (FRAME_BUFFER_COUNT > 2)
		(START_BUFFER_ADDRESS + (2 * BUFFER_SIZE)),
#endif
};

// -----------------------------------------------------------------------------
// Private functions
// -----------------------------------------------------------------------------

/*
 * @brief: Flush current framebuffer to the display
 */
static void __display_task_swap_buffers(vg_lite_window_t* pWindow) {
	VGLITE_SwapBuffers(pWindow);
	FBDEV_GetFrameBuffer(&pWindow->display->g_fbdev, 0);
}

/*
 * @brief: Task to manage display flushes and synchronize with hardware rendering
 * operations
 */
static void __display_task(void * pvParameters) {
	(void)pvParameters;

	do {
		xSemaphoreTake(sync_flush, portMAX_DELAY);

		vg_lite_window_t* window = DISPLAY_VGLITE_get_window();

		// Two actions:
		// 1- wait for the end of previous swap (if not already done): wait the
		// end of sending of current frame buffer to display
		// 2- start sending of current_buffer to display (without waiting the
		// end)
		__display_task_swap_buffers(window);

		// Increment framerate
		framerate_increment();

#if defined (FRAME_BUFFER_COUNT) && (FRAME_BUFFER_COUNT > 1)
		vg_lite_buffer_t *current_buffer = VGLITE_GetRenderTarget(window);

		// Configure frame buffer powering; at that point current_buffer is back buffer
		// cppcheck-suppress [misra-c2012-11.3] cast to (framebuffer_t *) is valid
		DISPLAY_IMPL_update_frame_buffer_status(current_buffer->memory, (framebuffer_t *)dirty_area_addr);

#if defined (DISPLAY_DMA_ENABLED) && (DISPLAY_DMA_ENABLED != 0)
		DISPLAY_DMA_start(
			&((uint8_t *)current_buffer->memory)[dirty_area_ymin * current_buffer->stride],
			&((uint8_t *)dirty_area_addr)[dirty_area_ymin * current_buffer->stride],
			(dirty_area_ymax - dirty_area_ymin + 1) * current_buffer->stride);
#else
		LLUI_DISPLAY_flushDone(false);
#endif // defined DISPLAY_DMA_ENABLED
#endif // defined FRAME_BUFFER_COUNT

	} while (1);
}


// -----------------------------------------------------------------------------
// LLUI_DISPLAY_impl.h functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
void LLUI_DISPLAY_IMPL_initialize(LLUI_DISPLAY_SInitData* init_data) {

	/***************
	 * Init VGLite *
	 ***************/

	if (kStatus_Success != BOARD_PrepareVGLiteController()) {
		DISPLAY_IMPL_error(true, "Prepare VGlite controlor error");
	}

	memset(*(uint32_t*)(&s_frameBufferAddress[0]), 0,
            DEMO_BUFFER_WIDTH*DEMO_BUFFER_HEIGHT*FRAME_BUFFER_BYTE_PER_PIXEL);
#if defined (FRAME_BUFFER_COUNT) && (FRAME_BUFFER_COUNT > 1)
	memset(*(uint32_t*)(&s_frameBufferAddress[1]), 0,
            DEMO_BUFFER_WIDTH*DEMO_BUFFER_HEIGHT*FRAME_BUFFER_BYTE_PER_PIXEL);
#endif
	DISPLAY_VGLITE_init();

	/************
	 * Init DMA *
	 ************/

#if defined (DISPLAY_DMA_ENABLED) && (DISPLAY_DMA_ENABLED != 0)
	DISPLAY_DMA_initialize(s_frameBufferAddress);
#endif // defined DISPLAY_DMA_ENABLED

	/*************
	 * Init task *
	 *************/

	sync_flush = xSemaphoreCreateBinary();
	if (pdPASS != xTaskCreate(
			__display_task,
			"Display",
			DISPLAY_TASK_STACK_SIZE,
			NULL,
			DISPLAY_TASK_PRIORITY,
			NULL)){
		DISPLAY_IMPL_error(true, "failed to create task __display\n");
	}

	/****************
	 * Init MicroUI *
	 ****************/

	vg_lite_window_t* window = DISPLAY_VGLITE_get_window();
	vg_lite_buffer_t *buffer = VGLITE_GetRenderTarget(window);
	init_data->binary_semaphore_0 = (void*)xSemaphoreCreateBinary();
	init_data->binary_semaphore_1 = (void*)xSemaphoreCreateBinary();
	init_data->lcd_width = window->width;
	init_data->lcd_height = window->height;
	init_data->back_buffer_address = (uint8_t*)buffer->memory;

	// notify that the display is initialized
	DISPLAY_IMPL_initialized();
}

// See the header file for the function documentation
uint8_t* LLUI_DISPLAY_IMPL_flush(MICROUI_GraphicsContext* gc, uint8_t* addr, uint32_t xmin, uint32_t ymin, uint32_t xmax, uint32_t ymax) {

	// x bounds are not used by the DMA
	(void)gc;
	(void)xmin;
	(void)xmax;

	uint8_t* ret = (uint8_t*) DISPLAY_VGLITE_get_next_graphics_buffer()->memory;

	// store dirty area to restore after the flush
	dirty_area_addr = addr;
	dirty_area_ymin = 0;
	dirty_area_ymax = gc->image.height - 1;

	// wakeup display task
	xSemaphoreGive(sync_flush);

	return ret;
}

// See the header file for the function documentation
void LLUI_DISPLAY_IMPL_binarySemaphoreTake(void* sem) {
	xSemaphoreTake((SemaphoreHandle_t)sem, portMAX_DELAY);
}

// See the header file for the function documentation
void LLUI_DISPLAY_IMPL_binarySemaphoreGive(void* sem, bool under_isr) {

	if (under_isr) {
		portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR((SemaphoreHandle_t)sem, &xHigherPriorityTaskWoken);
		if(xHigherPriorityTaskWoken != pdFALSE ) {
			// Force a context switch here.
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
	}
	else  {
		xSemaphoreGive((SemaphoreHandle_t)sem);
	}
}

// See the header file for the function documentation
uint32_t LLUI_DISPLAY_IMPL_getNewImageStrideInBytes(jbyte image_format, uint32_t image_width, uint32_t image_height, uint32_t default_stride) {

	(void)image_height;
	(void)default_stride;

	/*
	 * VGLite library requires a stride on 16 pixels: 64 bytes for 32bpp, 32 bytes
	 * for 16bpp and 16 bytes for 8bpp. Exception 8 bytes for <= 4bpp.
	 */
	uint32_t bpp = DISPLAY_UTILS_get_bpp((MICROUI_ImageFormat)image_format);
	return (bpp >= (uint32_t)8) ? ALIGN(image_width, (uint32_t)16) * (bpp / (uint32_t)8) : ALIGN(image_width, (uint32_t)8);
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------
