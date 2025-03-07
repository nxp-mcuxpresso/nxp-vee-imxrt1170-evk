/*
 * C
 *
 * Copyright 2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#include "fsl_cache.h"
#include "ui_vglite.h"
#include "bsp_util.h"
#include "LLUI_DISPLAY.h"

BSP_DECLARE_BUFFER(images_heap)

#define CACHE_LINESIZE_BYTE FSL_FEATURE_L1DCACHE_LINESIZE_BYTE

static MICROUI_GraphicsContext *previous_gc;

static bool image_is_in_image_heap(MICROUI_Image *image)
{
	if (image == NULL) {
		/* By default, consider that the image is in the image heap
		 * It will trigger cache operations even if it is not usefull
		 */
		return true;
	}
	
	uint32_t image_address = (uint32_t)LLUI_DISPLAY_getBufferAddress(image);

	if ((image_address < images_heap_START) || (image_address > images_heap_END)) {
		return false;
	} else {
		return true;
	}
}

static void get_image_and_clip(MICROUI_GraphicsContext *gc, MICROUI_Image **image, ui_rect_t **clip)
{
	if (gc != NULL) {
		*image = &gc->image;
		*clip = &gc->clip;
	} else {
		if (previous_gc != NULL) {
			*image = &previous_gc->image;
			*clip = &previous_gc->clip;
		} else {
			*image = NULL;
			*clip = NULL;
		}
	}
}

static uint32_t get_clip_address(MICROUI_Image *image, ui_rect_t *clip)
{
	if (image != NULL && clip != NULL) {
		return (uint32_t)LLUI_DISPLAY_getBufferAddress(image) + (uint32_t)clip->y1 * LLUI_DISPLAY_getStrideInBytes(image);
	} else {
		return images_heap_START;
	}
}

static uint32_t get_clip_size(MICROUI_Image *image, ui_rect_t *clip)
{
	if (clip != NULL) {
		return (uint32_t)(clip->y2 - clip->y1 + 1) * LLUI_DISPLAY_getStrideInBytes(image);
	} else {
		return images_heap_END - images_heap_START;
	}
}

void UI_VGLITE_IMPL_notify_gpu_start(MICROUI_GraphicsContext *gc)
{
	MICROUI_Image *image;
	ui_rect_t *clip;

	/* Save context for the next call of stop function */
	previous_gc = gc;
	get_image_and_clip(gc, &image, &clip);

	if (image_is_in_image_heap(image)) {
		// Flush the cache to make new changes available to GPU
		L1CACHE_CleanDCacheByRange(
			(get_clip_address(image, clip) / CACHE_LINESIZE_BYTE) * CACHE_LINESIZE_BYTE,
			(get_clip_size(image, clip) / CACHE_LINESIZE_BYTE + 1) * CACHE_LINESIZE_BYTE);
	}
}

void UI_VGLITE_IMPL_notify_gpu_stop(MICROUI_GraphicsContext *gc)
{
	MICROUI_Image *image;
	ui_rect_t *clip;

	get_image_and_clip(gc, &image, &clip);

	if (image_is_in_image_heap(image)) {
		// Invalidate the cache to notify CPU that new changes has been made in the memory by the GPU
		L1CACHE_InvalidateDCacheByRange(
			(get_clip_address(image, clip) / CACHE_LINESIZE_BYTE) * CACHE_LINESIZE_BYTE,
			(get_clip_size(image, clip) / CACHE_LINESIZE_BYTE + 1) * CACHE_LINESIZE_BYTE);
	}
}
