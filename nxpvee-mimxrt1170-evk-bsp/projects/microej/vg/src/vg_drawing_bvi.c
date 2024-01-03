/*
 * C
 *
 * Copyright 2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief Implementation of a set of vg_drawing.h drawing functions (library MicroVG).
 * These are implementations over VG-Lite and the destination buffer format is the BufferedVectorImage
 * format (VEE Port specific). When the drawing cannot be performed by the GPU, the stub implementation
 * is used insted.
 *
 * Like vg_drawing_vglite.c, this file redirects the drawings of the strings and the images to some
 * third-party files. These files don't perform the drawing themselves and let the caller do it. The
 * drawing function is specified by vg_drawing_bvi.c to target BufferedVectorImage buffer.
 *
 * @author MicroEJ Developer Team
 * @version 6.1.0
 * @see vg_drawing_vglite.c
 * @see ui_drawing_bvi.c
 */

#include "microvg_configuration.h"
#if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "vg_drawing_bvi.h"
#include "bvi_vglite.h"
#include "ui_vglite.h"
#include "vg_drawing_vglite.h"
#include "microvg_helper.h"
#include "microvg_vglite_helper.h"

// -----------------------------------------------------------------------------
// Private globals
// -----------------------------------------------------------------------------

/**
 * @brief Current BufferedVectorImage to update
 */
static void* target;

// -----------------------------------------------------------------------------
// Private functions
// -----------------------------------------------------------------------------

// VG_DRAWING_VGLITE_draw_glyph_t
static jint _add_glyph_color(vg_lite_path_t * path, vg_lite_fill_t fill_rule, vg_lite_matrix_t * matrix, vg_lite_blend_t blend, vg_lite_color_t color, vg_lite_linear_gradient_t * grad) {
	(void)grad;
	return BVI_VGLITE_add_draw_path(target, path, fill_rule, matrix, blend, color);
}

// VG_DRAWING_VGLITE_draw_glyph_t
static jint _add_glyph_gradient(vg_lite_path_t * path, vg_lite_fill_t fill_rule, vg_lite_matrix_t * matrix, vg_lite_blend_t blend, vg_lite_color_t color, vg_lite_linear_gradient_t * grad) {
	(void)color;
	return BVI_VGLITE_add_draw_gradient(target, path, fill_rule, matrix, grad, blend);
}

static DRAWING_Status _draw_string_on_circle(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size, jfloat x, jfloat y, jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing, jfloat radius, jint direction){

	if (UI_VGLITE_enable_vg_lite_scissor(gc)) {

		// prepare matrix
		vg_lite_matrix_t vglite_matrix;
		VG_DRAWING_VGLITE_prepare_matrix(&vglite_matrix, x, y, matrix);

		// prepare color
		vg_lite_blend_t vglite_blend = MICROVG_VGLITE_HELPER_get_blend(blend);
		vg_lite_color_t vglite_color = (vg_lite_color_t)((gc->foreground_color & 0x00FFFFFF) + (int)(((unsigned int) alpha) << 24));

		// add operation
		bool rendered;
		target = MAP_BVI_ON_GC(gc);
		jint error = VG_DRAWING_VGLITE_draw_string(&_add_glyph_color, text, faceHandle, size, &vglite_matrix, vglite_blend, vglite_color, MICROVG_HELPER_NULL_GRADIENT, letterSpacing, radius, direction, &rendered);
		MICROVG_VGLITE_handle_error(gc, error);
	}
	// else no error: the drawing is just "not performed"

	return DRAWING_DONE;
}

static DRAWING_Status _draw_string_on_circle_gradient(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size, jfloat x, jfloat y, jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing, jfloat radius, jint direction, jint *gradientData, jfloat *gradientMatrix){
	DRAWING_Status ret = DRAWING_DONE;

	if (UI_VGLITE_enable_vg_lite_scissor(gc)) {

		// prepare matrix
		vg_lite_matrix_t vglite_matrix;
		VG_DRAWING_VGLITE_prepare_matrix(&vglite_matrix, x, y, matrix);

		// parepare gradient
		vg_lite_blend_t vglite_blend = MICROVG_VGLITE_HELPER_get_blend(blend);
		vg_lite_linear_gradient_t vglite_gradient;
		vg_lite_error_t vglite_error = VG_DRAWING_VGLITE_prepare_gradient(&vglite_gradient, gradientData, gradientMatrix, &vglite_matrix, alpha);

		if (VG_LITE_SUCCESS == vglite_error) {

			// add operation
			bool rendered;
			target = MAP_BVI_ON_GC(gc);
			jint error = VG_DRAWING_VGLITE_draw_string(&_add_glyph_gradient, text, faceHandle, size, &vglite_matrix, vglite_blend, 0, &vglite_gradient, letterSpacing, radius, direction, &rendered);
			MICROVG_VGLITE_handle_error(gc, error);

			// post operation
			VG_DRAWING_VGLITE_clear_gradient(&vglite_gradient);
		}
		else {
			ret = UI_VGLITE_report_vglite_error(gc, vglite_error);
		}
	}
	// else no error: the drawing is just "not performed"

	return ret;
}

// --------------------------------------------------------------------------------
// vg_drawing_bvi.h functions
// (the function names differ according to the index of destination format)
// --------------------------------------------------------------------------------

// See the header file for the function documentation
DRAWING_Status VG_DRAWING_BVI_drawPath(MICROUI_GraphicsContext* gc, jbyte* pathData, jint x, jint y, jfloat* matrix, jint fillRule, jint blend, jint color) {

	if (UI_VGLITE_enable_vg_lite_scissor(gc)) {

		// prepare path
		vg_lite_path_t* vglite_path = VG_DRAWING_VGLITE_to_vglite_path((MICROVG_PATH_HEADER_t*)pathData);
		vg_lite_blend_t vglite_blend = MICROVG_VGLITE_HELPER_get_blend(blend);
		vg_lite_fill_t vglite_fill = MICROVG_VGLITE_HELPER_get_fill_rule(fillRule);

		// prepare matrix
		vg_lite_matrix_t vglite_matrix;
		VG_DRAWING_VGLITE_prepare_matrix(&vglite_matrix, x, y, matrix);

		// add operation
		jint ret = BVI_VGLITE_add_draw_path(MAP_BVI_ON_GC(gc), vglite_path, vglite_fill, &vglite_matrix, vglite_blend, (vg_lite_color_t)color);
		MICROVG_VGLITE_handle_error(gc, ret);
	}
	// else no error: the drawing is just "not performed"

	return DRAWING_DONE;
}

// See the header file for the function documentation
DRAWING_Status VG_DRAWING_BVI_drawGradient(MICROUI_GraphicsContext* gc, jbyte* pathData, jint x, jint y, jfloat* matrix, jint fillRule, jint alpha, jint blend, jint* gradientData, jfloat* gradientMatrix){

	if (UI_VGLITE_enable_vg_lite_scissor(gc)) {

		// prepare path
		vg_lite_path_t* vglite_path = VG_DRAWING_VGLITE_to_vglite_path((MICROVG_PATH_HEADER_t*)pathData);
		vg_lite_blend_t vglite_blend = MICROVG_VGLITE_HELPER_get_blend(blend);
		vg_lite_fill_t vglite_fill = MICROVG_VGLITE_HELPER_get_fill_rule(fillRule);

		// prepare matrix
		vg_lite_matrix_t vglite_matrix;
		VG_DRAWING_VGLITE_prepare_matrix(&vglite_matrix, x, y, matrix);

		// parepare gradient
		vg_lite_linear_gradient_t vglite_gradient;
		vg_lite_error_t vglite_error = VG_DRAWING_VGLITE_prepare_gradient(&vglite_gradient, gradientData, gradientMatrix, &vglite_matrix, alpha);

		if (VG_LITE_SUCCESS == vglite_error) {

			// add operation
			jint ret = BVI_VGLITE_add_draw_gradient(MAP_BVI_ON_GC(gc), vglite_path, vglite_fill, &vglite_matrix, &vglite_gradient, vglite_blend);
			MICROVG_VGLITE_handle_error(gc, ret);

			// post operation
			VG_DRAWING_VGLITE_clear_gradient(&vglite_gradient);
		}
		else {
			UI_VGLITE_report_vglite_error(gc, vglite_error);
		}

	}
	// else no error: the drawing is just "not performed"

	return DRAWING_DONE;
}

DRAWING_Status VG_DRAWING_BVI_drawString(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jfloat x, jfloat y,  jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing){
	return _draw_string_on_circle(gc, text, faceHandle, size,  x, y, matrix, alpha, blend, letterSpacing, (jfloat)0, (jint)0);
}

DRAWING_Status VG_DRAWING_BVI_drawStringGradient(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jfloat x, jfloat y,  jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing, jint *gradientData, jfloat *gradientMatrix){
	return _draw_string_on_circle_gradient(gc, text, faceHandle, size, x, y, matrix, alpha, blend, letterSpacing, (jfloat)0, (jint)0, gradientData, gradientMatrix);
}

DRAWING_Status VG_DRAWING_BVI_drawStringOnCircle(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jint x, jint y, jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing, jfloat radius, jint direction){
	return _draw_string_on_circle(gc, text, faceHandle, size, (jfloat)x, (jfloat)y, matrix, alpha, blend, letterSpacing, radius, direction);
}

DRAWING_Status VG_DRAWING_BVI_drawStringOnCircleGradient(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jint x, jint y, jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing, jfloat radius, jint direction, jint *gradientData, jfloat *gradientMatrix){
	return _draw_string_on_circle_gradient(gc, text, faceHandle, size, (jfloat)x, (jfloat)y, matrix, alpha, blend, letterSpacing, radius, direction, gradientData, gradientMatrix);
}

DRAWING_Status VG_DRAWING_BVI_drawImage(MICROUI_GraphicsContext* gc, void* image, jfloat *matrix, jint alpha, jlong elapsed, const float color_matrix[], jint* errno){

	float width;
	float height;
	VG_DRAWING_get_image_size(image, &width, &height);

	if (UI_VGLITE_enable_vg_lite_scissor_area(gc, width, height, matrix)) {
		*errno = BVI_VGLITE_add_draw_image(MAP_BVI_ON_GC(gc), image, matrix, alpha, elapsed, color_matrix);
		MICROVG_VGLITE_handle_error(gc, *errno);
	}
	// nothing to draw

	return DRAWING_DONE;
}


// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#endif // #if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE
