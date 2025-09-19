/*
 * C
 *
 * Copyright 2023-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief Implementation of a set of vg_drawing.h drawing functions (library MicroVG).
 * These are implementations over VGLite and the destination buffer format is the BufferedVectorImage
 * format (VEE Port specific). When the drawing cannot be performed by the GPU, the stub implementation
 * is used insted.
 *
 * Like vg_drawing_vglite.c, this file redirects the drawings of the strings and the images to some
 * third-party files. These files don't perform the drawing themselves and let the caller do it. The
 * drawing function is specified by vg_drawing_bvi.c to target BufferedVectorImage buffer.
 *
 * @author MicroEJ Developer Team
 * @version 9.0.1
 * @see vg_drawing_vglite.c
 * @see ui_drawing_bvi.c
 */

#include "vg_configuration.h"
#if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "vg_drawing_bvi.h"
#include "vg_bvi_vglite.h"
#include "ui_vglite.h"
#include "vg_drawing_vglite.h"
#include "vg_helper.h"
#include "vg_vglite_helper.h"

// -----------------------------------------------------------------------------
// Private globals
// -----------------------------------------------------------------------------

/**
 * @brief Current BufferedVectorImage to update
 */
static void *target;

// -----------------------------------------------------------------------------
// Private functions
// -----------------------------------------------------------------------------

// VG_FREETYPE_draw_glyph_t
static jint _add_glyph_color(VG_PATH_HEADER_t *path, jfloat *matrix, uint32_t color, bool fill_rule_even_odd,
                             void *user_data) {
	MICROVG_VGLITE_draw_glyph_data_t *data = (MICROVG_VGLITE_draw_glyph_data_t *)user_data;
	return VG_BVI_VGLITE_add_draw_path(target, VG_DRAWING_VGLITE_to_vglite_path(path),
	                                   fill_rule_even_odd ? VG_LITE_FILL_EVEN_ODD : VG_LITE_FILL_NON_ZERO,
	                                   (vg_lite_matrix_t *)matrix, data->blend, (vg_lite_color_t)color);
}

// VG_FREETYPE_draw_glyph_t
static jint _add_glyph_gradient(VG_PATH_HEADER_t *path, jfloat *matrix, uint32_t color, bool fill_rule_even_odd,
                                void *user_data) {
	(void)color;
	MICROVG_VGLITE_draw_glyph_data_t *data = (MICROVG_VGLITE_draw_glyph_data_t *)user_data;
	return VG_BVI_VGLITE_add_draw_gradient(target, VG_DRAWING_VGLITE_to_vglite_path(path),
	                                       fill_rule_even_odd ? VG_LITE_FILL_EVEN_ODD : VG_LITE_FILL_NON_ZERO,
	                                       (vg_lite_matrix_t *)matrix, data->gradient, data->blend);
}

static DRAWING_Status _draw_string_on_circle(MICROUI_GraphicsContext *gc, const jchar *text, jint length,
                                             jint faceHandle, jfloat size, const jfloat *matrix, jint alpha, jint blend,
                                             jfloat letterSpacing, jfloat radius, jint direction) {
	if (UI_VGLITE_enable_vg_lite_scissor(gc)) {
		// prepare color
		vg_lite_blend_t vglite_blend = VG_VGLITE_HELPER_get_blend(blend);
		vg_lite_color_t vglite_color = (vg_lite_color_t)((gc->foreground_color & 0x00FFFFFF) +
		                                                 (int)(((unsigned int)alpha) << 24));

		// prepare draw_glyph's data
		MICROVG_VGLITE_draw_glyph_data_t data;
		data.blend = vglite_blend;

		// add operation
		target = MAP_BVI_ON_GC(gc);
		jint error = VG_FREETYPE_draw_string(&_add_glyph_color, text, length, faceHandle, size, matrix, vglite_color,
		                                     letterSpacing, radius, direction, &data);
		VG_VGLITE_HELPER_handle_error(gc, error);
	}
	// else no error: the drawing is just "not performed"

	return DRAWING_DONE;
}

static DRAWING_Status _draw_string_on_circle_gradient(MICROUI_GraphicsContext *gc, const jchar *text, jint length,
                                                      jint faceHandle, jfloat size, const jfloat *matrix, jint alpha,
                                                      jint blend, jfloat letterSpacing, jfloat radius, jint direction,
                                                      const jint *gradientData, const jfloat *gradientMatrix) {
	DRAWING_Status ret = DRAWING_DONE;

	if (UI_VGLITE_enable_vg_lite_scissor(gc)) {
		// parepare gradient
		vg_lite_blend_t vglite_blend = VG_VGLITE_HELPER_get_blend(blend);
		vg_lite_linear_gradient_t vglite_gradient;
		vg_lite_error_t vglite_error = VG_DRAWING_VGLITE_prepare_gradient(&vglite_gradient, gradientData,
		                                                                  gradientMatrix, matrix, alpha);

		if (VG_LITE_SUCCESS == vglite_error) {
			// prepare draw_glyph's data
			MICROVG_VGLITE_draw_glyph_data_t data;
			data.blend = vglite_blend;
			data.gradient = &vglite_gradient;

			// add operation
			target = MAP_BVI_ON_GC(gc);
			jint error = VG_FREETYPE_draw_string(&_add_glyph_gradient, text, length, faceHandle, size, matrix, 0,
			                                     letterSpacing, radius, direction, &data);
			VG_VGLITE_HELPER_handle_error(gc, error);

			// post operation
			VG_DRAWING_VGLITE_clear_gradient(&vglite_gradient);
		} else {
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
DRAWING_Status VG_DRAWING_BVI_drawPath(MICROUI_GraphicsContext *gc, const jbyte *pathData, const jfloat *matrix,
                                       jint fillRule, jint blend, jint color) {
	if (UI_VGLITE_enable_vg_lite_scissor(gc)) {
		// prepare path
		const vg_lite_path_t *vglite_path = VG_DRAWING_VGLITE_to_vglite_path((VG_PATH_HEADER_t *)pathData);
		vg_lite_blend_t vglite_blend = VG_VGLITE_HELPER_get_blend(blend);
		vg_lite_fill_t vglite_fill = VG_VGLITE_HELPER_get_fill_rule(fillRule);

		// add operation
		jint ret = VG_BVI_VGLITE_add_draw_path(MAP_BVI_ON_GC(gc), vglite_path, vglite_fill, (vg_lite_matrix_t *)matrix,
		                                       vglite_blend, (vg_lite_color_t)color);
		VG_VGLITE_HELPER_handle_error(gc, ret);
	}
	// else no error: the drawing is just "not performed"

	return DRAWING_DONE;
}

// See the header file for the function documentation
DRAWING_Status VG_DRAWING_BVI_drawGradient(MICROUI_GraphicsContext *gc, const jbyte *pathData, const jfloat *matrix,
                                           jint fillRule, jint alpha, jint blend, const jint *gradientData,
                                           const jfloat *gradientMatrix) {
	if (UI_VGLITE_enable_vg_lite_scissor(gc)) {
		// prepare path
		const vg_lite_path_t *vglite_path = VG_DRAWING_VGLITE_to_vglite_path((VG_PATH_HEADER_t *)pathData);
		vg_lite_blend_t vglite_blend = VG_VGLITE_HELPER_get_blend(blend);
		vg_lite_fill_t vglite_fill = VG_VGLITE_HELPER_get_fill_rule(fillRule);

		// parepare gradient
		vg_lite_linear_gradient_t vglite_gradient;
		vg_lite_error_t vglite_error = VG_DRAWING_VGLITE_prepare_gradient(&vglite_gradient, gradientData,
		                                                                  gradientMatrix, matrix, alpha);

		if (VG_LITE_SUCCESS == vglite_error) {
			// add operation
			jint ret = VG_BVI_VGLITE_add_draw_gradient(MAP_BVI_ON_GC(gc), vglite_path, vglite_fill,
			                                           (vg_lite_matrix_t *)matrix, &vglite_gradient, vglite_blend);
			VG_VGLITE_HELPER_handle_error(gc, ret);

			// post operation
			VG_DRAWING_VGLITE_clear_gradient(&vglite_gradient);
		} else {
			UI_VGLITE_report_vglite_error(gc, vglite_error);
		}
	}
	// else no error: the drawing is just "not performed"

	return DRAWING_DONE;
}

DRAWING_Status VG_DRAWING_BVI_drawString(MICROUI_GraphicsContext *gc, const jchar *text, jint length, jint faceHandle,
                                         jfloat size, const jfloat *matrix, jint alpha, jint blend,
                                         jfloat letterSpacing) {
	return _draw_string_on_circle(gc, text, length, faceHandle, size, matrix, alpha, blend, letterSpacing, (jfloat)0,
	                              (jint)0);
}

DRAWING_Status VG_DRAWING_BVI_drawStringGradient(MICROUI_GraphicsContext *gc, const jchar *text, jint length,
                                                 jint faceHandle, jfloat size, const jfloat *matrix, jint alpha,
                                                 jint blend, jfloat letterSpacing, const jint *gradientData,
                                                 const jfloat *gradientMatrix) {
	return _draw_string_on_circle_gradient(gc, text, length, faceHandle, size, matrix, alpha, blend, letterSpacing,
	                                       (jfloat)0, (jint)0, gradientData, gradientMatrix);
}

DRAWING_Status VG_DRAWING_BVI_drawStringOnCircle(MICROUI_GraphicsContext *gc, const jchar *text, jint length,
                                                 jint faceHandle, jfloat size, const jfloat *matrix, jint alpha,
                                                 jint blend, jfloat letterSpacing, jfloat radius, jint direction) {
	return _draw_string_on_circle(gc, text, length, faceHandle, size, matrix, alpha, blend, letterSpacing, radius,
	                              direction);
}

DRAWING_Status VG_DRAWING_BVI_drawStringOnCircleGradient(MICROUI_GraphicsContext *gc, const jchar *text, jint length,
                                                         jint faceHandle, jfloat size, const jfloat *matrix, jint alpha,
                                                         jint blend, jfloat letterSpacing, jfloat radius,
                                                         jint direction, const jint *gradientData,
                                                         const jfloat *gradientMatrix) {
	return _draw_string_on_circle_gradient(gc, text, length, faceHandle, size, matrix, alpha, blend, letterSpacing,
	                                       radius, direction, gradientData, gradientMatrix);
}

DRAWING_Status VG_DRAWING_BVI_drawImage(MICROUI_GraphicsContext *gc, const void *image, const jfloat *matrix,
                                        jint alpha, jlong elapsed, const float color_matrix[], jint *errno) {
	if (UI_VGLITE_enable_vg_lite_scissor(gc)) {
		*errno = VG_BVI_VGLITE_add_draw_image(MAP_BVI_ON_GC(gc), image, matrix, alpha, elapsed, color_matrix);
		VG_VGLITE_HELPER_handle_error(gc, *errno);
	}
	// nothing to draw

	return DRAWING_DONE;
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#endif // #if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE
