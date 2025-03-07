/*
 * C
 *
 * Copyright 2023-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief Implementation of a set of ui_drawing.h drawing functions (MicroUI and Drawing libraries).
 * These are implementations over VGLite and the destination buffer format is the format specified
 * in the VEE port. When the drawing cannot be performed by the GPU, the software implementation
 * is used insted.
 *
 * This file just configures the destination buffer and redirects all drawings to drawing_vglite.c.
 * The file drawing_vglite.c converts the MicroUI drawing in a path can call a specific function to
 * draw the path; it does not perform the drawing itself and let the caller do it. This drawing function
 * is specified by ui_drawing_vglite.c to target standard destination buffers (display buffer and all
 * MicroUI BufferedImage).
 *
 * Some MicroUI and Drawing drawing functions are disabled for performance reasons or rendering quality
 * reasons. See ui_vglite_configuration.h to force the use of the GPU.
 *
 * @author MicroEJ Developer Team
 * @version 10.0.0
 */

// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------

#include "ui_drawing_vglite.h"
#include "ui_drawing_soft.h"
#include "dw_drawing_soft.h"
#include "ui_image_drawing.h"
#include "ui_configuration.h"
#include "ui_drawing_vglite_process.h"

// --------------------------------------------------------------------------------
// Private functions
// --------------------------------------------------------------------------------

static DRAWING_Status _draw_path(MICROUI_GraphicsContext *gc, vg_lite_path_t *path, vg_lite_fill_t fill_rule,
                                 vg_lite_matrix_t *matrix, vg_lite_blend_t blend, vg_lite_color_t color) {
	vg_lite_buffer_t *target = UI_VGLITE_configure_destination(gc);
	vg_lite_error_t err = vg_lite_draw(target, path, fill_rule, matrix, blend, color);
	return UI_VGLITE_post_operation(gc, err);
}

#ifdef VGLITE_USE_GPU_FOR_SIMPLE_DRAWINGS

static DRAWING_Status _clear(MICROUI_GraphicsContext *gc, vg_lite_rectangle_t *rect, vg_lite_color_t color) {
	vg_lite_buffer_t *target = UI_VGLITE_configure_destination(gc);
	vg_lite_error_t err = vg_lite_clear(target, rect, color);
	return UI_VGLITE_post_operation(gc, err);
}

#endif // VGLITE_USE_GPU_FOR_SIMPLE_DRAWINGS

static DRAWING_Status _blit_rect(MICROUI_GraphicsContext *gc, vg_lite_buffer_t *source, uint32_t *rect,
                                 vg_lite_matrix_t *matrix, vg_lite_blend_t blend, vg_lite_color_t color,
                                 vg_lite_filter_t filter) {
	vg_lite_buffer_t *target = UI_VGLITE_configure_destination(gc);
	vg_lite_error_t err = vg_lite_blit_rect(target, source, rect, matrix, blend, color, filter);
	return UI_VGLITE_post_operation(gc, err);
}

static DRAWING_Status _draw_region(MICROUI_GraphicsContext *gc, vg_lite_buffer_t *source, uint32_t *rect,
                                   vg_lite_matrix_t *matrix, vg_lite_blend_t blend, vg_lite_color_t color,
                                   vg_lite_filter_t filter, uint32_t element_index) {
	vg_lite_buffer_t *target = UI_VGLITE_configure_destination(gc);
	DRAWING_Status ret = DRAWING_RUNNING;

	// rect[x,y,w,h]
	uint32_t rect_index = element_index + (uint32_t)2;

	// retrieve band's size (width or height)
	jint size = rect[rect_index];
	rect[rect_index] = (uint32_t)(matrix->m[element_index][2] - rect[element_index]);

	// go to x + band size
	rect[element_index] += size;
	matrix->m[element_index][2] = rect[element_index] + rect[rect_index];

	while (size > 0) {
		// adjust band's size
		if (size < rect[rect_index]) {
			rect[rect_index] = size;
		}

		// adjust src & dest positions
		rect[element_index] -= rect[rect_index];
		matrix->m[element_index][2] -= rect[rect_index];

		size -= rect[rect_index];

		if (VG_LITE_SUCCESS != vg_lite_blit_rect(target, source, rect, matrix, blend, color, filter)) {
			LLUI_DISPLAY_reportError(gc, DRAWING_LOG_LIBRARY_INCIDENT);
			ret = DRAWING_DONE;
			size = 0; // stop the loop
		} else {
			// wakeup task only for the last iteration, otherwise waits the end of the drawing before continue
			UI_VGLITE_start_operation(size <= 0);
		}
	}

	return ret;
}

// --------------------------------------------------------------------------------
// ui_drawing.h / ui_drawing_vglite.h functions
// (the function names differ according to the available number of destination formats)
// --------------------------------------------------------------------------------

#ifdef VGLITE_USE_GPU_FOR_SIMPLE_DRAWINGS

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_drawLine(MICROUI_GraphicsContext *gc, jint startX, jint startY, jint endX, jint endY) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
		status = UI_DRAWING_SOFT_drawLine(gc, startX, startY, endX, endY);
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	status = UI_DRAWING_VGLITE_PROCESS_drawLine(&_draw_path, gc, startX, startY, endX, endY);

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

#endif // VGLITE_USE_GPU_FOR_SIMPLE_DRAWINGS

#ifdef VGLITE_USE_GPU_FOR_SIMPLE_DRAWINGS

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_drawHorizontalLine(MICROUI_GraphicsContext *gc, jint x1, jint x2, jint y) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
		status = UI_DRAWING_SOFT_drawHorizontalLine(gc, x1, x2, y);
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	status = UI_DRAWING_VGLITE_PROCESS_drawHorizontalLine(&_draw_path, gc, x1, x2, y);

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

#endif // VGLITE_USE_GPU_FOR_SIMPLE_DRAWINGS

#ifdef VGLITE_USE_GPU_FOR_SIMPLE_DRAWINGS

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_drawVerticalLine(MICROUI_GraphicsContext *gc, jint x, jint y1, jint y2) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
		status = UI_DRAWING_SOFT_drawVerticalLine(gc, x, y1, y2);
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	status = UI_DRAWING_VGLITE_PROCESS_drawVerticalLine(&_draw_path, gc, x, y1, y2);

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

#endif // VGLITE_USE_GPU_FOR_SIMPLE_DRAWINGS

#ifdef VGLITE_USE_GPU_FOR_SIMPLE_DRAWINGS

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_fillRectangle(MICROUI_GraphicsContext *gc, jint x1, jint y1, jint x2, jint y2) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
		status = UI_DRAWING_SOFT_fillRectangle(gc, x1, y1, x2, y2);
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	status = UI_DRAWING_VGLITE_PROCESS_fillRectangle(&_clear, gc, x1, y1, x2, y2);

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

#endif // VGLITE_USE_GPU_FOR_SIMPLE_DRAWINGS

#ifdef VGLITE_USE_GPU_FOR_SIMPLE_DRAWINGS

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_drawRoundedRectangle(MICROUI_GraphicsContext *gc, jint x, jint y, jint width,
                                                      jint height, jint cornerEllipseWidth, jint cornerEllipseHeight) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
		status = UI_DRAWING_SOFT_drawRoundedRectangle(gc, x, y, width, height, cornerEllipseWidth, cornerEllipseHeight);
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	status = UI_DRAWING_VGLITE_PROCESS_drawRoundedRectangle(&_draw_path, gc, x, y, width, height, cornerEllipseWidth,
	                                                        cornerEllipseHeight);

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

#endif // VGLITE_USE_GPU_FOR_SIMPLE_DRAWINGS

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_fillRoundedRectangle(MICROUI_GraphicsContext *gc, jint x, jint y, jint width,
                                                      jint height, jint cornerEllipseWidth, jint cornerEllipseHeight) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
		status = UI_DRAWING_SOFT_fillRoundedRectangle(gc, x, y, width, height, cornerEllipseWidth, cornerEllipseHeight);
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	status = UI_DRAWING_VGLITE_PROCESS_fillRoundedRectangle(&_draw_path, gc, x, y, width, height, cornerEllipseWidth,
	                                                        cornerEllipseHeight);

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

#ifdef VGLITE_USE_GPU_FOR_SIMPLE_DRAWINGS

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_drawCircleArc(MICROUI_GraphicsContext *gc, jint x, jint y, jint diameter,
                                               jfloat startAngle, jfloat arcAngle) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
		status = UI_DRAWING_SOFT_drawCircleArc(gc, x, y, diameter, startAngle, arcAngle);
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	status = UI_DRAWING_VGLITE_PROCESS_drawCircleArc(&_draw_path, gc, x, y, diameter, startAngle, arcAngle);

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

#endif // VGLITE_USE_GPU_FOR_SIMPLE_DRAWINGS

#ifdef VGLITE_USE_GPU_FOR_SIMPLE_DRAWINGS

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_drawEllipseArc(MICROUI_GraphicsContext *gc, jint x, jint y, jint width, jint height,
                                                jfloat startAngle, jfloat arcAngle) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
		status = UI_DRAWING_SOFT_drawEllipseArc(gc, x, y, width, height, startAngle, arcAngle);
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	status = UI_DRAWING_VGLITE_PROCESS_drawEllipseArc(&_draw_path, gc, x, y, width, height, startAngle, arcAngle);

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

#endif // VGLITE_USE_GPU_FOR_SIMPLE_DRAWINGS

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_fillCircleArc(MICROUI_GraphicsContext *gc, jint x, jint y, jint diameter,
                                               jfloat startAngle, jfloat arcAngle) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
		status = UI_DRAWING_SOFT_fillCircleArc(gc, x, y, diameter, startAngle, arcAngle);
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	status = UI_DRAWING_VGLITE_PROCESS_fillCircleArc(&_draw_path, gc, x, y, diameter, startAngle, arcAngle);

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_fillEllipseArc(MICROUI_GraphicsContext *gc, jint x, jint y, jint width, jint height,
                                                jfloat startAngle, jfloat arcAngle) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
		status = UI_DRAWING_SOFT_fillEllipseArc(gc, x, y, width, height, startAngle, arcAngle);
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	status = UI_DRAWING_VGLITE_PROCESS_fillEllipseArc(&_draw_path, gc, x, y, width, height, startAngle, arcAngle);

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

#ifdef VGLITE_USE_GPU_FOR_SIMPLE_DRAWINGS

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_drawEllipse(MICROUI_GraphicsContext *gc, jint x, jint y, jint width, jint height) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
		status = UI_DRAWING_SOFT_drawEllipse(gc, x, y, width, height);
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	status = UI_DRAWING_VGLITE_PROCESS_drawEllipse(&_draw_path, gc, x, y, width, height);

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

#endif // VGLITE_USE_GPU_FOR_SIMPLE_DRAWINGS

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_fillEllipse(MICROUI_GraphicsContext *gc, jint x, jint y, jint width, jint height) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
		status = UI_DRAWING_SOFT_fillEllipse(gc, x, y, width, height);
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	status = UI_DRAWING_VGLITE_PROCESS_fillEllipse(&_draw_path, gc, x, y, width, height);

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

#ifdef VGLITE_USE_GPU_FOR_SIMPLE_DRAWINGS

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_drawCircle(MICROUI_GraphicsContext *gc, jint x, jint y, jint diameter) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
		status = UI_DRAWING_SOFT_drawCircle(gc, x, y, diameter);
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	status = UI_DRAWING_VGLITE_PROCESS_drawCircle(&_draw_path, gc, x, y, diameter);

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

#endif // VGLITE_USE_GPU_FOR_SIMPLE_DRAWINGS

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_fillCircle(MICROUI_GraphicsContext *gc, jint x, jint y, jint diameter) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
		status = UI_DRAWING_SOFT_fillCircle(gc, x, y, diameter);
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	status = UI_DRAWING_VGLITE_PROCESS_fillCircle(&_draw_path, gc, x, y, diameter);

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_drawImage(MICROUI_GraphicsContext *gc, MICROUI_Image *img, jint regionX, jint regionY,
                                           jint width, jint height, jint x, jint y, jint alpha) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
#if !defined(UI_FEATURE_IMAGE_CUSTOM_FORMATS)
		status = UI_DRAWING_SOFT_drawImage(gc, img, regionX, regionY, width, height, x, y, alpha);
#else
		status = UI_IMAGE_DRAWING_draw(gc, img, regionX, regionY, width, height, x, y, alpha);
#endif
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	vg_lite_color_t color;
	vg_lite_matrix_t matrix;
	uint32_t blit_rect[4];

	vg_lite_buffer_t *source_buffer = UI_DRAWING_VGLITE_PROCESS_prepare_draw_image(gc, img, regionX, regionY, width,
	                                                                               height, x, y, alpha, &color, &matrix,
	                                                                               blit_rect);

	if (NULL != source_buffer) {
		status = _blit_rect(gc, source_buffer, blit_rect, &matrix, VG_LITE_BLEND_SRC_OVER, color, VG_LITE_FILTER_POINT);
	} else {
#if !defined(UI_FEATURE_IMAGE_CUSTOM_FORMATS)
		status = UI_DRAWING_SOFT_drawImage(gc, img, regionX, regionY, width, height, x, y, alpha);
#else
		status = UI_IMAGE_DRAWING_draw(gc, img, regionX, regionY, width, height, x, y, alpha);
#endif
	}

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

#ifdef VGLITE_USE_GPU_FOR_RGB565_IMAGES

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_copyImage(MICROUI_GraphicsContext *gc, MICROUI_Image *img, jint regionX, jint regionY,
                                           jint width, jint height, jint x, jint y) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
#if !defined(UI_FEATURE_IMAGE_CUSTOM_FORMATS)
		status = UI_DRAWING_SOFT_copyImage(gc, img, regionX, regionY, width, height, x, y);
#else
		status = UI_IMAGE_DRAWING_copy(gc, img, regionX, regionY, width, height, x, y);
#endif
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	status = (img == &gc->image) ?
	         // have to manage the overlap
	         UI_DRAWING_VGLITE_drawRegion(gc, regionX, regionY, width, height, x, y, 0xff)
	         // no overlap: draw image as usual
	            : UI_DRAWING_VGLITE_drawImage(gc, img, regionX, regionY, width, height, x, y, 0xff);

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

#endif // VGLITE_USE_GPU_FOR_RGB565_IMAGES

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_drawRegion(MICROUI_GraphicsContext *gc, jint regionX, jint regionY, jint width,
                                            jint height, jint x, jint y, jint alpha) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
#if !defined(UI_FEATURE_IMAGE_CUSTOM_FORMATS)
		status = UI_DRAWING_SOFT_drawRegion(gc, regionX, regionY, width, height, x, y, alpha);
#else
		status = UI_IMAGE_DRAWING_drawRegion(gc, regionX, regionY, width, height, x, y, alpha);
#endif
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	vg_lite_color_t color;
	vg_lite_matrix_t matrix;
	uint32_t blit_rect[4];

	vg_lite_buffer_t *source_buffer = UI_DRAWING_VGLITE_PROCESS_prepare_draw_image(gc, &gc->image, regionX, regionY,
	                                                                               width, height, x, y, alpha, &color,
	                                                                               &matrix, blit_rect);

	if (NULL != source_buffer) {
		if ((y == regionY) && (x > regionX) && (x < (regionX + width))) {
			// draw with overlap: cut the drawings in several widths
			status = _draw_region(gc, source_buffer, blit_rect, &matrix, VG_LITE_BLEND_SRC_OVER, color,
			                      VG_LITE_FILTER_POINT, 0);
		} else if ((y > regionY) && (y < (regionY + height))) {
			// draw with overlap: cut the drawings in several heights
			status = _draw_region(gc, source_buffer, blit_rect, &matrix, VG_LITE_BLEND_SRC_OVER, color,
			                      VG_LITE_FILTER_POINT, 1);
		} else {
			// draw in one shot
			status = _blit_rect(gc, source_buffer, blit_rect, &matrix, VG_LITE_BLEND_SRC_OVER, color,
			                    VG_LITE_FILTER_POINT);
		}
	} else {
#if !defined(UI_FEATURE_IMAGE_CUSTOM_FORMATS)
		status = UI_DRAWING_SOFT_drawRegion(gc, regionX, regionY, width, height, x, y, alpha);
#else
		status = UI_IMAGE_DRAWING_drawRegion(gc, regionX, regionY, width, height, x, y, alpha);
#endif
	}

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_drawThickFadedPoint(MICROUI_GraphicsContext *gc, jint x, jint y, jint thickness,
                                                     jint fade) {
	DRAWING_Status status;

	if (!UI_DRAWING_VGLITE_IS_COMPATIBLE_FADE(fade)
#ifdef VGLITE_OPTION_TOGGLE_GPU
	    || !UI_VGLITE_is_hardware_rendering_enabled()
#endif // VGLITE_OPTION_TOGGLE_GPU
	    ) {
		DW_DRAWING_SOFT_drawThickFadedPoint(gc, x, y, thickness, fade);
		status = DRAWING_DONE;
	} else {
		status = UI_DRAWING_VGLITE_PROCESS_drawThickFadedPoint(&_draw_path, gc, x, y,
		                                                       UI_DRAWING_VGLITE_GET_THICKNESS(thickness, fade));
	}
	return status;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_drawThickFadedLine(MICROUI_GraphicsContext *gc, jint startX, jint startY, jint endX,
                                                    jint endY, jint thickness, jint fade, DRAWING_Cap startCap,
                                                    DRAWING_Cap endCap) {
	DRAWING_Status status;

	if (!UI_DRAWING_VGLITE_IS_COMPATIBLE_FADE(fade)
#ifdef VGLITE_OPTION_TOGGLE_GPU
	    || !UI_VGLITE_is_hardware_rendering_enabled()
#endif // VGLITE_OPTION_TOGGLE_GPU
	    ) {
		DW_DRAWING_SOFT_drawThickFadedLine(gc, startX, startY, endX, endY, thickness, fade, startCap, endCap);
		status = DRAWING_DONE;
	} else {
		status = UI_DRAWING_VGLITE_PROCESS_drawThickFadedLine(&_draw_path, gc, startX, startY, endX, endY,
		                                                      UI_DRAWING_VGLITE_GET_THICKNESS(thickness, fade),
		                                                      startCap, endCap);
	}

	return status;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_drawThickFadedCircle(MICROUI_GraphicsContext *gc, jint x, jint y, jint diameter,
                                                      jint thickness, jint fade) {
	DRAWING_Status status;

	if (!UI_DRAWING_VGLITE_IS_COMPATIBLE_FADE(fade)
#ifdef VGLITE_OPTION_TOGGLE_GPU
	    || !UI_VGLITE_is_hardware_rendering_enabled()
#endif // VGLITE_OPTION_TOGGLE_GPU
	    ) {
		DW_DRAWING_SOFT_drawThickFadedCircle(gc, x, y, diameter, thickness, fade);
		status = DRAWING_DONE;
	} else {
		status = UI_DRAWING_VGLITE_PROCESS_drawThickFadedCircle(&_draw_path, gc, x, y, diameter,
		                                                        UI_DRAWING_VGLITE_GET_THICKNESS(thickness, fade));
	}

	return status;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_drawThickFadedCircleArc(MICROUI_GraphicsContext *gc, jint x, jint y, jint diameter,
                                                         jfloat startAngle, jfloat arcAngle, jint thickness, jint fade,
                                                         DRAWING_Cap start, DRAWING_Cap end) {
	DRAWING_Status status;

	if (!UI_DRAWING_VGLITE_IS_COMPATIBLE_FADE(fade)
#ifdef VGLITE_OPTION_TOGGLE_GPU
	    || !UI_VGLITE_is_hardware_rendering_enabled()
#endif // VGLITE_OPTION_TOGGLE_GPU
	    ) {
		DW_DRAWING_SOFT_drawThickFadedCircleArc(gc, x, y, diameter, startAngle, arcAngle, thickness, fade, start, end);
		status = DRAWING_DONE;
	} else {
		status = UI_DRAWING_VGLITE_PROCESS_drawThickFadedCircleArc(&_draw_path, gc, x, y, diameter, startAngle,
		                                                           arcAngle,
		                                                           UI_DRAWING_VGLITE_GET_THICKNESS(thickness, fade),
		                                                           start, end);
	}

	return status;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_drawThickFadedEllipse(MICROUI_GraphicsContext *gc, jint x, jint y, jint width,
                                                       jint height, jint thickness, jint fade) {
	DRAWING_Status status;

	if (!UI_DRAWING_VGLITE_IS_COMPATIBLE_FADE(fade)
#ifdef VGLITE_OPTION_TOGGLE_GPU
	    || !UI_VGLITE_is_hardware_rendering_enabled()
#endif // VGLITE_OPTION_TOGGLE_GPU
	    ) {
		DW_DRAWING_SOFT_drawThickFadedEllipse(gc, x, y, width, height, thickness, fade);
		status = DRAWING_DONE;
	} else {
		status = UI_DRAWING_VGLITE_PROCESS_drawThickFadedEllipse(&_draw_path, gc, x, y, width, height,
		                                                         UI_DRAWING_VGLITE_GET_THICKNESS(thickness, fade));
	}

	return status;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_drawThickLine(MICROUI_GraphicsContext *gc, jint startX, jint startY, jint endX,
                                               jint endY, jint thickness) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
		status = DW_DRAWING_SOFT_drawThickLine(gc, startX, startY, endX, endY, thickness);
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	status = UI_DRAWING_VGLITE_PROCESS_drawThickLine(&_draw_path, gc, startX, startY, endX, endY, thickness);

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_drawThickCircle(MICROUI_GraphicsContext *gc, jint x, jint y, jint diameter,
                                                 jint thickness) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
		status = DW_DRAWING_SOFT_drawThickCircle(gc, x, y, diameter, thickness);
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	status = UI_DRAWING_VGLITE_PROCESS_drawThickCircle(&_draw_path, gc, x, y, diameter, thickness);

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_drawThickEllipse(MICROUI_GraphicsContext *gc, jint x, jint y, jint width, jint height,
                                                  jint thickness) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
		status = DW_DRAWING_SOFT_drawThickEllipse(gc, x, y, width, height, thickness);
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	status = UI_DRAWING_VGLITE_PROCESS_drawThickEllipse(&_draw_path, gc, x, y, width, height, thickness);

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_drawThickCircleArc(MICROUI_GraphicsContext *gc, jint x, jint y, jint diameter,
                                                    jfloat startAngle, jfloat arcAngle, jint thickness) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
		status = DW_DRAWING_SOFT_drawThickCircleArc(gc, x, y, diameter, startAngle, arcAngle, thickness);
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	status = UI_DRAWING_VGLITE_PROCESS_drawThickCircleArc(&_draw_path, gc, x, y, diameter, startAngle, arcAngle,
	                                                      thickness);

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_drawFlippedImage(MICROUI_GraphicsContext *gc, MICROUI_Image *img, jint regionX,
                                                  jint regionY, jint width, jint height, jint x, jint y,
                                                  DRAWING_Flip transformation, jint alpha) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
		status = DW_DRAWING_SOFT_drawFlippedImage(gc, img, regionX, regionY, width, height, x, y, transformation,
		                                          alpha);
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

#ifndef VGLITE_USE_GPU_FOR_RGB565_IMAGES
	// CPU (memcpy) is faster than GPU
	if ((MICROUI_IMAGE_FORMAT_RGB565 == img->format) && (DRAWING_FLIP_NONE == transformation) && (0xff == alpha)) {
		DW_DRAWING_SOFT_drawFlippedImage(gc, img, regionX, regionY, width, height, x, y, transformation, alpha);
		status = DRAWING_DONE;
	} else {
#endif // VGLITE_USE_GPU_FOR_RGB565_IMAGES

	bool is_gpu_compatible;
	status = UI_DRAWING_VGLITE_PROCESS_drawFlippedImage(&_blit_rect, gc, img, regionX, regionY, width, height, x, y,
	                                                    transformation, alpha, &is_gpu_compatible);

	if (!is_gpu_compatible) {
		DW_DRAWING_SOFT_drawFlippedImage(gc, img, regionX, regionY, width, height, x, y, transformation, alpha);
		status = DRAWING_DONE;
	}

#ifndef VGLITE_USE_GPU_FOR_RGB565_IMAGES
}

#endif // VGLITE_USE_GPU_FOR_RGB565_IMAGES

#ifdef VGLITE_OPTION_TOGGLE_GPU
}
#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_drawRotatedImageNearestNeighbor(MICROUI_GraphicsContext *gc, MICROUI_Image *img,
                                                                 jint x, jint y, jint rotationX, jint rotationY,
                                                                 jfloat angle, jint alpha) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
		status = DW_DRAWING_SOFT_drawRotatedImageNearestNeighbor(gc, img, x, y, rotationX, rotationY, angle, alpha);
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	bool is_gpu_compatible;
	status = UI_DRAWING_VGLITE_PROCESS_drawRotatedImageNearestNeighbor(&_blit_rect, gc, img, x, y, rotationX, rotationY,
	                                                                   angle, alpha, &is_gpu_compatible);

	if (!is_gpu_compatible) {
		DW_DRAWING_SOFT_drawRotatedImageNearestNeighbor(gc, img, x, y, rotationX, rotationY, angle, alpha);
		status = DRAWING_DONE;
	}

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_drawRotatedImageBilinear(MICROUI_GraphicsContext *gc, MICROUI_Image *img, jint x,
                                                          jint y, jint rotationX, jint rotationY, jfloat angle,
                                                          jint alpha) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
		status = DW_DRAWING_SOFT_drawRotatedImageBilinear(gc, img, x, y, rotationX, rotationY, angle, alpha);
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	bool is_gpu_compatible;
	status = UI_DRAWING_VGLITE_PROCESS_drawRotatedImageBilinear(&_blit_rect, gc, img, x, y, rotationX, rotationY, angle,
	                                                            alpha, &is_gpu_compatible);

	if (!is_gpu_compatible) {
		DW_DRAWING_SOFT_drawRotatedImageBilinear(gc, img, x, y, rotationX, rotationY, angle, alpha);
		status = DRAWING_DONE;
	}

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_drawScaledImageNearestNeighbor(MICROUI_GraphicsContext *gc, MICROUI_Image *img, jint x,
                                                                jint y, jfloat factorX, jfloat factorY, jint alpha) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
		status = DW_DRAWING_SOFT_drawScaledImageNearestNeighbor(gc, img, x, y, factorX, factorY, alpha);
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	bool is_gpu_compatible;
	status = UI_DRAWING_VGLITE_PROCESS_drawScaledImageNearestNeighbor(&_blit_rect, gc, img, x, y, factorX, factorY,
	                                                                  alpha, &is_gpu_compatible);

	if (!is_gpu_compatible) {
		DW_DRAWING_SOFT_drawScaledImageNearestNeighbor(gc, img, x, y, factorX, factorY, alpha);
		status = DRAWING_DONE;
	}

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_drawScaledImageBilinear(MICROUI_GraphicsContext *gc, MICROUI_Image *img, jint x,
                                                         jint y, jfloat factorX, jfloat factorY, jint alpha) {
	DRAWING_Status status;

#ifdef VGLITE_OPTION_TOGGLE_GPU
	if (!UI_VGLITE_is_hardware_rendering_enabled()) {
		status = DW_DRAWING_SOFT_drawScaledImageBilinear(gc, img, x, y, factorX, factorY, alpha);
	} else {
#endif // VGLITE_OPTION_TOGGLE_GPU

	bool is_gpu_compatible;
	status = UI_DRAWING_VGLITE_PROCESS_drawScaledImageBilinear(&_blit_rect, gc, img, x, y, factorX, factorY, alpha,
	                                                           &is_gpu_compatible);

	if (!is_gpu_compatible) {
		DW_DRAWING_SOFT_drawScaledImageBilinear(gc, img, x, y, factorX, factorY, alpha);
		status = DRAWING_DONE;
	}

#ifdef VGLITE_OPTION_TOGGLE_GPU
}

#endif // VGLITE_OPTION_TOGGLE_GPU

	return status;
}

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------
