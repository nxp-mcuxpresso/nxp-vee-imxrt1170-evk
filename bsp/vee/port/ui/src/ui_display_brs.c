/*
 * Copyright 2023-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief This file implements is the common part of all display buffer strategies (BRS).
 * @author MicroEJ Developer Team
 * @version 14.2.0
 */

// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------

#include <string.h>

#include "ui_display_brs.h"
#include "bsp_util.h"
#include "ui_drawing.h"

// --------------------------------------------------------------------------------
// ui_display_brs.h API
// --------------------------------------------------------------------------------

/*
 * @brief Provides a simple implementation of the restore: use memcpy().
 * See the header file for the function documentation
 */
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_DISPLAY_BRS_restore(MICROUI_GraphicsContext *gc, MICROUI_Image *old_back_buffer,
                                                            ui_rect_t *rect) {
	DRAWING_Status ret;
	uint32_t bpp = LLUI_DISPLAY_getImageBPP(old_back_buffer);
	uint32_t width = UI_RECT_get_width(rect);
	uint32_t height = UI_RECT_get_height(rect);

	if (8u > bpp) {
		// this weak function is not designed to target this kind of buffer
		// -> let the standard function copyImage() performing the job
		ret = UI_DRAWING_copyImage(gc, old_back_buffer, rect->x1, rect->y1, width, height, rect->x1, rect->y1);
	} else {
		uint8_t *dst = LLUI_DISPLAY_getBufferAddress(&gc->image);
		uint8_t *src = LLUI_DISPLAY_getBufferAddress(old_back_buffer);
		uint32_t stride = LLUI_DISPLAY_getStrideInBytes(&gc->image);

		dst += rect->y1 * stride;
		src += rect->y1 * stride;

		if ((0 == rect->x1) && (old_back_buffer->width == width)) {
			// a simple memcpy is required!
			(void)memcpy(dst, src, stride * height);
		} else {
			// have to perform one memcpy per line
			dst += rect->x1 * bpp / 8u;
			src += rect->x1 * bpp / 8u;
			uint32_t size = width * bpp / 8u;

			for (uint32_t y = 0; y < height; y++) {
				(void)memcpy(dst, src, size);
				dst += stride;
				src += stride;
			}
		}

		ret = DRAWING_DONE;
	}
	return ret;
}

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------
