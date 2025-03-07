/*
 * C
 *
 * Copyright 2020-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief MicroEJ MicroUI library low level API: implementation over VG-Lite
 * @author MicroEJ Developer Team
 * @version 6.0.1
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
#include "ui_display_brs.h"

#include <FreeRTOS.h>
#include <semphr.h>

#include "display_impl.h"
#include "framerate.h"
#include "ui_vglite.h"

#include "vglite_window.h"

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
uint8_t dirty_area_flush; // identifier of the flush

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

/*
 * @brief VGLite display context
 */
static vg_lite_display_t display;

/*
 * @brief VGLite window context
 */
static vg_lite_window_t window;

// -----------------------------------------------------------------------------
// Private functions
// -----------------------------------------------------------------------------

/*
 * @brief: Flush current framebuffer to the display
 */
static void __display_task_swap_buffers(vg_lite_window_t* pWindow) {
	VGLITE_SwapBuffers(pWindow);
}

/*
 * @brief: Task to manage display flushes and synchronize with hardware rendering
 * operations
 */
static void __display_task(void * pvParameters) {
	(void)pvParameters;

	do {
		xSemaphoreTake(sync_flush, portMAX_DELAY);


		// save the flush conf: can be modified by the next call to flush() as soon as LLUI_DISPLAY_setDrawingBuffer() will wake up the Graphics Engine
		uint8_t flush_identifier = dirty_area_flush;

		// Two actions:
		// 1- wait for the end of previous swap (if not already done): wait the
		// end of sending of current frame buffer to display
		// 2- start sending of current_buffer to display (without waiting the
		// end)
		__display_task_swap_buffers(&window);

		// Increment framerate
		framerate_increment();

#if defined (FRAME_BUFFER_COUNT) && (FRAME_BUFFER_COUNT > 1)

#if defined (FRAME_BUFFER_COUNT) && (FRAME_BUFFER_COUNT == 2)
		// have to wait the LCD swapping before restoring the new back buffer
		FBDEV_GetFrameBuffer(&window.display->g_fbdev, 0);
#endif

		vg_lite_buffer_t *current_buffer = VGLITE_GetRenderTarget(&window);

		// back buffer not restored but can be used for next drawing
		if (!LLUI_DISPLAY_setDrawingBuffer(flush_identifier, current_buffer->memory, false)) {
			// end of flush not expected; the Graphics Engine keeps using previous back buffer;
			// have to cancel the buffers swap
			VGLITE_CancelSwapBuffers();
		}


#if defined (FRAME_BUFFER_COUNT) && (FRAME_BUFFER_COUNT > 2)
		// have to wait the LCD swapping before allowing a new flush()
		FBDEV_GetFrameBuffer(&window.display->g_fbdev, 0);
#endif

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
		UI_VGLITE_IMPL_error(true, "Prepare VGlite controlor error");
	}

	vg_lite_error_t ret = VG_LITE_SUCCESS;

	if (VG_LITE_SUCCESS != VGLITE_CreateDisplay(&display)) {
		UI_VGLITE_IMPL_error(true, "VGLITE_CreateDisplay failed: VGLITE_CreateDisplay() returned error %d", ret);
	}

	// Initialize the window.
	if (VG_LITE_SUCCESS != VGLITE_CreateWindow(&display, &window)) {
		UI_VGLITE_IMPL_error(true, "VGLITE_CreateWindow failed: VGLITE_CreateWindow() returned error %d", ret);
	}

	// force to empty the counter
	for (uint8_t i = 1 /*FIXME*/; i < window.bufferCount; i++) {
		(void)FBDEV_GetFrameBuffer(&(display.g_fbdev), 0);
	}

	memset((uint32_t*)(s_frameBufferAddress[0]), 0,
			DEMO_BUFFER_WIDTH*DEMO_BUFFER_HEIGHT*FRAME_BUFFER_BYTE_PER_PIXEL);
#if defined (FRAME_BUFFER_COUNT) && (FRAME_BUFFER_COUNT > 1)
	memset((uint32_t*)(s_frameBufferAddress[1]), 0,
			DEMO_BUFFER_WIDTH*DEMO_BUFFER_HEIGHT*FRAME_BUFFER_BYTE_PER_PIXEL);
#endif

	UI_VGLITE_initialize();

	UI_VGLITE_start((void*)xSemaphoreCreateBinary());

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
		UI_VGLITE_IMPL_error(true, "failed to create task __display\n");
	}

	/****************
	 * Init MicroUI *
	 ****************/

	vg_lite_buffer_t *buffer = VGLITE_GetRenderTarget(&window);
	init_data->binary_semaphore_0 = (void*)xSemaphoreCreateBinary();
	init_data->binary_semaphore_1 = (void*)xSemaphoreCreateBinary();
	init_data->lcd_width = window.width;
	init_data->lcd_height = window.height;
	init_data->back_buffer_address = (uint8_t*)buffer->memory;
}

// See the header file for the function documentation
void LLUI_DISPLAY_IMPL_flush(MICROUI_GraphicsContext* gc, uint8_t flush_identifier, const ui_rect_t areas[], size_t length) {
	uint8_t* addr = LLUI_DISPLAY_getBufferAddress(&gc->image);
	
	// store dirty area to restore after the flush
	dirty_area_addr = addr;
	dirty_area_flush = flush_identifier;

	// wakeup display task
	xSemaphoreGive(sync_flush);
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

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------
