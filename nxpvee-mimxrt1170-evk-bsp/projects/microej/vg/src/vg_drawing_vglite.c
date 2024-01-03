/*
 * C
 *
 * Copyright 2021-2023 MicroEJ Corp. All rights reserved.
Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief Implementation of a set of vg_drawing.h drawing functions (library MicroVG).
 * These are implementations over VG-Lite and the destination buffer format is the format specified
 * in the VEE port.
 *
 * This file redirects the drawings of the strings and the images to some third-party files.
 * These files don't perform the drawing themselves and let the caller do it. The drawing function
 * is specified by vg_drawing_vglite.c to target standard destination buffers (display buffer and all
 * MicroUI BufferedImage).
 *
 * @author MicroEJ Developer Team
 * @version 6.1.0
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <math.h>

#include <LLVG_MATRIX_impl.h>
#include <LLVG_PATH_impl.h>

#include "vg_drawing_vglite.h"
#include "ui_drawing_vglite.h"
#include "microvg_helper.h"
#include "microvg_vglite_helper.h"
#include "color.h"
#include "ui_vglite.h"

// -----------------------------------------------------------------------------
// Defines
// -----------------------------------------------------------------------------

#ifndef VG_FEATURE_PATH
#error "This implementation is only compatible when VG_FEATURE_PATH is set"
#endif

/*
 * @brief Specific error code when VG-Lite library throws an error.
 */
#define RET_ERROR_VGLITE -1

/*
 * @brief Specific error code when gradient data is invalid.
 */
#define RET_ERROR_GRADIENT -2

// -----------------------------------------------------------------------------
// Private globals
// -----------------------------------------------------------------------------

static vg_lite_path_t render_path;
static vg_lite_buffer_t* render_buffer;
static bool draw_gradient_flushed;

// -----------------------------------------------------------------------------
// Private functions
// -----------------------------------------------------------------------------

static inline DRAWING_Status _post_operation(MICROUI_GraphicsContext* gc, jint error, bool rendered) {
	DRAWING_Status ret;
	if (LLVG_SUCCESS != error) {
		ret = MICROVG_VGLITE_report_error(gc, error);
	}
	else if (rendered) {
		// start GPU operation and do not wait for it to end
		UI_VGLITE_start_operation(true);
		ret = DRAWING_RUNNING;
	}
	else {
		// else: nothing has been drawn (empty drawing, out of clip, etc.): not an error
		ret = DRAWING_DONE;
	}
	if (DRAWING_RUNNING != ret) {
		// GPU is useless now, can be disabled. No GPU access should be done after this line
		UI_VGLITE_IMPL_notify_gpu_stop();
	}
	if (!UI_VGLITE_need_to_premultiply() && (VG_LITE_SUCCESS != vg_lite_enable_premultiply())) {
		// cannot restore premultiplication
		UI_VGLITE_IMPL_error(false, "vg_lite engine premultiply error: cannot restore the pre multiplication");
	}
	return ret;
}

inline static bool _need_to_premultiply(vg_lite_blend_t blend) {
	bool need_to_premultiply = UI_VGLITE_need_to_premultiply();
	if (!need_to_premultiply) {
		switch (blend) {
			case VG_LITE_BLEND_NONE:
			case VG_LITE_BLEND_SRC_IN:
			case VG_LITE_BLEND_DST_IN:
			case VG_LITE_BLEND_SUBTRACT:
				if (VG_LITE_SUCCESS != vg_lite_disable_premultiply()) {
					// have to disable the premultiplication but cannot
					UI_VGLITE_IMPL_error(false, "vg_lite engine premultiply error: cannot disable the pre multiplication");
				}
				else {
					need_to_premultiply = true;
				}
				break;
			default:
				// use the GPU premultiplication
				break;
		}
	}
	return need_to_premultiply;
}

inline static void _update_color(vg_lite_color_t* color, vg_lite_blend_t blend) {
	if (_need_to_premultiply(blend)) {
		// premultiply color
		*color = UI_VGLITE_premultiply(*color);
	}
}

static void _update_gradient(vg_lite_linear_gradient_t* gradient, vg_lite_blend_t blend) {
	if (_need_to_premultiply(blend)) {
		// premultiply each color
		uint32_t count = gradient->count;
		uint32_t* colors = gradient->colors;
		for (uint32_t i = 0; i < count; i++) {
			colors[i] = UI_VGLITE_premultiply(colors[i]);
		}
	}

	// update the VG-Lite internal image that represents the gradient
	(void)vg_lite_update_grad(gradient); // always success
}

static inline jint _convert_vglite_error_to_llvg_error(vg_lite_error_t vg_lite_error) {
	jint ret;
	switch (vg_lite_error) {
	case VG_LITE_SUCCESS:
		ret = LLVG_SUCCESS;
		break;
	case VG_LITE_OUT_OF_MEMORY:
		ret = LLVG_OUT_OF_MEMORY;
		break;
	default:
		ret = LLVG_DATA_INVALID;
		break;
	}
	return ret;
}

// VG_DRAWING_VGLITE_draw_image_element_t
static jint _draw_image_element(vg_lite_path_t * path, vg_lite_fill_t fill_rule, vg_lite_matrix_t * matrix, vg_lite_blend_t blend, vg_lite_color_t color, vg_lite_linear_gradient_t * grad, bool is_new_gradient) {
	vg_lite_error_t ret;
	if (NULL == grad) {
		_update_color(&color, blend);
		ret = vg_lite_draw(render_buffer, path, fill_rule, matrix, blend, color);
	}
	else {

		if (is_new_gradient) {
			// not same gradient than previous: have to:

			// 1- flush the previous gradient drawings because we will update the shared gradient's image
			if (!draw_gradient_flushed) {
				UI_VGLITE_start_operation(false);
			}

			// 2- update the shared gradient's image.
			_update_gradient(grad, blend);
		}
		// else: same gradient than previous: no need to update gradient's image again

		ret = vg_lite_draw_gradient(render_buffer, path, fill_rule, matrix, grad, blend);
		draw_gradient_flushed = false;
	}
	return _convert_vglite_error_to_llvg_error(ret);
}

// VG_DRAWING_VGLITE_draw_glyph_t
static jint _draw_glyph_color(vg_lite_path_t * path, vg_lite_fill_t fill_rule, vg_lite_matrix_t * matrix, vg_lite_blend_t blend, vg_lite_color_t color, vg_lite_linear_gradient_t * grad) {
	(void)grad;
	return _convert_vglite_error_to_llvg_error(vg_lite_draw(render_buffer, path, fill_rule, matrix, blend, color));
}

// VG_DRAWING_VGLITE_draw_glyph_t
static jint _draw_glyph_gradient(vg_lite_path_t * path, vg_lite_fill_t fill_rule, vg_lite_matrix_t * matrix, vg_lite_blend_t blend, vg_lite_color_t color, vg_lite_linear_gradient_t * grad) {
	(void)color;
	return _convert_vglite_error_to_llvg_error(vg_lite_draw_gradient(render_buffer, path, fill_rule, matrix, grad, blend));
}

static DRAWING_Status _draw_string_on_circle(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jfloat x, jfloat y, jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing, jfloat radius, jint direction){
	DRAWING_Status ret = DRAWING_DONE;

	if (UI_VGLITE_enable_vg_lite_scissor(gc)) {

		// prepare matrix
		vg_lite_matrix_t vglite_matrix;
		VG_DRAWING_VGLITE_prepare_matrix(&vglite_matrix, x, y, matrix);

		// prepare color
		vg_lite_blend_t vglite_blend = MICROVG_VGLITE_HELPER_get_blend(blend);
		vg_lite_color_t vglite_color = (vg_lite_color_t)((gc->foreground_color & 0x00FFFFFF) + (int)(((unsigned int) alpha) << 24));
		_update_color(&vglite_color, vglite_blend);

		// draw
		bool rendered;
		render_buffer = UI_VGLITE_configure_destination(gc);
		jint error = VG_DRAWING_VGLITE_draw_string(&_draw_glyph_color, text, faceHandle, size, &vglite_matrix, vglite_blend, vglite_color, MICROVG_HELPER_NULL_GRADIENT, letterSpacing, radius, direction, &rendered);
		ret = _post_operation(gc, error, rendered);
	}
	// else no error: the drawing is just "not performed"

	return ret;
}

static DRAWING_Status _draw_string_on_circle_gradient(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jfloat x, jfloat y, jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing, jfloat radius, jint direction, jint *gradientData, jfloat *gradientMatrix){
	DRAWING_Status ret = DRAWING_DONE;

	if (UI_VGLITE_enable_vg_lite_scissor(gc)) {

		// prepare matrix
		vg_lite_matrix_t vglite_matrix;
		VG_DRAWING_VGLITE_prepare_matrix(&vglite_matrix, x, y, matrix);

		// parepare gradient
		vg_lite_linear_gradient_t vglite_gradient;
		vg_lite_error_t vglite_error = VG_DRAWING_VGLITE_prepare_gradient(&vglite_gradient, gradientData, gradientMatrix, &vglite_matrix, alpha);

		if (VG_LITE_SUCCESS == vglite_error) {

			// create the gradient image
			vg_lite_blend_t vglite_blend = MICROVG_VGLITE_HELPER_get_blend(blend);
			_update_gradient(&vglite_gradient, vglite_blend);

			// draw
			bool rendered;
			render_buffer = UI_VGLITE_configure_destination(gc);
			jint error = VG_DRAWING_VGLITE_draw_string(&_draw_glyph_gradient, text, faceHandle, size, &vglite_matrix, vglite_blend, 0, &vglite_gradient, letterSpacing, radius, direction, &rendered);
			ret = _post_operation(gc, error, rendered);

			VG_DRAWING_VGLITE_clear_gradient(&vglite_gradient);
		}
		else {
			ret = UI_VGLITE_report_vglite_error(gc, vglite_error);
		}
	}
	// else no error: the drawing is just "not performed"

	return ret;
}

// -----------------------------------------------------------------------------
// microvg_path.h functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
void MICROVG_PATH_initialize(void) {
	vg_lite_init_path(
			&render_path,
			(vg_lite_format_t)MICROVG_PATH_get_path_encoder_format(), // default value
			VG_LITE_HIGH,
			0,
			NULL,
			-4000, // Left
			-4000, // Top
			4000, // Right
			4000 // Bottom
	);
	render_path.path_type = VG_LITE_DRAW_FILL_PATH;
}

// See the header file for the function documentation
uint8_t MICROVG_PATH_get_path_encoder_format(void) {
	return VG_LITE_FP32;
}

// See the header file for the function documentation
uint32_t MICROVG_PATH_convert_path_command(jint command) {
	uint32_t ret;
	switch (command) {
	default: // unknown -> close (should not occur)
	case LLVG_PATH_CMD_CLOSE:
		ret = VLC_OP_END;
		break;
	case LLVG_PATH_CMD_MOVE:
		ret = VLC_OP_MOVE;
		break;
	case LLVG_PATH_CMD_MOVE_REL:
		ret = VLC_OP_MOVE_REL;
		break;
	case LLVG_PATH_CMD_LINE:
		ret = VLC_OP_LINE;
		break;
	case LLVG_PATH_CMD_LINE_REL:
		ret = VLC_OP_LINE_REL;
		break;
	case LLVG_PATH_CMD_QUAD:
		ret = VLC_OP_QUAD;
		break;
	case LLVG_PATH_CMD_QUAD_REL:
		ret = VLC_OP_QUAD_REL;
		break;
	case LLVG_PATH_CMD_CUBIC:
		ret = VLC_OP_CUBIC;
		break;
	case LLVG_PATH_CMD_CUBIC_REL:
		ret = VLC_OP_CUBIC_REL;
		break;
	}
	return ret;
}

// See the header file for the function documentation
uint32_t MICROVG_PATH_get_command_parameter_number(jint command) {
	uint32_t ret;
	switch (command) {
	default: // unknown -> close (should not occur)
	case VLC_OP_END:
	case VLC_OP_CLOSE:
		ret = 0;
		break;
	case VLC_OP_MOVE:
	case VLC_OP_MOVE_REL:
	case VLC_OP_LINE:
	case VLC_OP_LINE_REL:
		ret = 2;
		break;
	case VLC_OP_QUAD:
	case VLC_OP_QUAD_REL:
		ret = 4;
		break;
	case VLC_OP_CUBIC:
	case VLC_OP_CUBIC_REL:
		ret = 6;
		break;
	}
	return ret;
}

// --------------------------------------------------------------------------------
// vg_drawing_vglite.h functions
// --------------------------------------------------------------------------------

void VG_DRAWING_VGLITE_prepare_matrix(vg_lite_matrix_t* vg_lite_matrix, jfloat x, jfloat y, jfloat* matrix) {
	jfloat* local_matrix = MICROVG_HELPER_check_matrix(matrix);
	jfloat* mapped_matrix = MAP_VGLITE_MATRIX(vg_lite_matrix);

	if((0 != x) || (0 != y)) {
		// Create translate matrix for initial x,y translation from graphicscontext.
		LLVG_MATRIX_IMPL_setTranslate(mapped_matrix, x, y);
		LLVG_MATRIX_IMPL_concatenate(mapped_matrix, local_matrix);
	}
	else {
		// use original matrix
		LLVG_MATRIX_IMPL_copy(mapped_matrix, local_matrix);
	}
}

vg_lite_error_t VG_DRAWING_VGLITE_prepare_gradient(vg_lite_linear_gradient_t* gradient, jint* gradientData, jfloat* gradientMatrix, vg_lite_matrix_t* vg_lite_matrix, int alpha) {
	jfloat* local_gradient_matrix = MICROVG_HELPER_check_matrix(gradientMatrix);
	return MICROVG_VGLITE_HELPER_to_vg_lite_gradient(gradient, gradientData, local_gradient_matrix, MAP_VGLITE_MATRIX(vg_lite_matrix), alpha);
}

/*
 * @brief vg_lite_init_grad allocates a buffer in VGLite buffer, we must free it.
 * No error even if init_grad is never called because vg_lite_clear_grad
 * checks the allocation.
 */
void VG_DRAWING_VGLITE_clear_gradient(vg_lite_linear_gradient_t* gradient) {
	vg_lite_clear_grad(gradient);
}

/*
 * @brief Fills a VGLite path with RAW path data
 */
vg_lite_path_t* VG_DRAWING_VGLITE_to_vglite_path(MICROVG_PATH_HEADER_t* p){
	render_path.bounding_box[0] = (vg_lite_float_t)(p->bounds_xmin);
	render_path.bounding_box[1] = (vg_lite_float_t)(p->bounds_ymin);
	render_path.bounding_box[2] = (vg_lite_float_t)(p->bounds_xmax);
	render_path.bounding_box[3] = (vg_lite_float_t)(p->bounds_ymax);
	render_path.format = (vg_lite_format_t)(p->format);
	render_path.path_length = (int32_t)(p->data_size);
	render_path.path = (void*)&(((uint8_t*)p)[MICROVG_PATH_get_path_header_size()]);
	render_path.path_changed = 1;
	return &render_path;
}

// --------------------------------------------------------------------------------
// vg_drawing_vglite.h functions
// (the function names differ according to the available number of destination formats)
// --------------------------------------------------------------------------------

// See the header file for the function documentation
DRAWING_Status VG_DRAWING_VGLITE_drawPath(MICROUI_GraphicsContext* gc, jbyte* pathData, jint x, jint y, jfloat* matrix, jint fillRule, jint blend, jint color) {
	DRAWING_Status ret = DRAWING_DONE;

	if (UI_VGLITE_enable_vg_lite_scissor(gc)) {

		// prepare path
		vg_lite_path_t* vglite_path = VG_DRAWING_VGLITE_to_vglite_path((MICROVG_PATH_HEADER_t*)pathData);
		vg_lite_blend_t vglite_blend = MICROVG_VGLITE_HELPER_get_blend(blend);
		vg_lite_fill_t vglite_fill = MICROVG_VGLITE_HELPER_get_fill_rule(fillRule);

		// prepare matrix
		vg_lite_matrix_t vglite_matrix;
		VG_DRAWING_VGLITE_prepare_matrix(&vglite_matrix, x, y, matrix);

		// prepare color
		vg_lite_color_t vglite_color = (vg_lite_color_t)color;
		_update_color(&vglite_color, vglite_blend);

		// draw
		render_buffer = UI_VGLITE_configure_destination(gc);
		vg_lite_error_t vglite_error = vg_lite_draw(render_buffer, vglite_path, vglite_fill, &vglite_matrix, vglite_blend, vglite_color);

		// post operation
		ret = UI_VGLITE_post_operation(gc, vglite_error);
	}
	// else no error: the drawing is just "not performed"

	return ret;
}

// See the header file for the function documentation
DRAWING_Status VG_DRAWING_VGLITE_drawGradient(MICROUI_GraphicsContext* gc, jbyte* pathData, jint x, jint y, jfloat* matrix, jint fillRule, jint alpha, jint blend, jint* gradientData, jfloat* gradientMatrix){
	DRAWING_Status ret = DRAWING_DONE;

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

			// create the gradient image
			_update_gradient(&vglite_gradient, vglite_blend);

			// draw
			render_buffer = UI_VGLITE_configure_destination(gc);
			vglite_error = vg_lite_draw_gradient(render_buffer, vglite_path, vglite_fill, &vglite_matrix, &vglite_gradient, vglite_blend);

			// post operation
			ret = UI_VGLITE_post_operation(gc, vglite_error);
			VG_DRAWING_VGLITE_clear_gradient(&vglite_gradient);
		}
		else {
			ret = UI_VGLITE_report_vglite_error(gc, vglite_error);
		}

	}
	// else no error: the drawing is just "not performed"

	return ret;
}

DRAWING_Status VG_DRAWING_VGLITE_drawString(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jfloat x, jfloat y, jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing){
	return _draw_string_on_circle(gc, text, faceHandle, size,  x, y, matrix, alpha, blend, letterSpacing, (jfloat)0, (jint)0);
}

DRAWING_Status VG_DRAWING_VGLITE_drawStringGradient(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size, jfloat x, jfloat y,  float* matrix, jint alpha, jint blend, jfloat letterSpacing, jint *gradientData, jfloat *gradientMatrix){
	return _draw_string_on_circle_gradient(gc, text, faceHandle, size, x, y, matrix, alpha, blend, letterSpacing, (jfloat)0, (jint)0, gradientData, gradientMatrix);
}

DRAWING_Status VG_DRAWING_VGLITE_drawStringOnCircle(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jint x, jint y, jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing, jfloat radius, jint direction){
	return _draw_string_on_circle(gc, text, faceHandle, size,  (jfloat)x, (jfloat)y, matrix, alpha, blend, letterSpacing, radius, direction);
}

DRAWING_Status VG_DRAWING_VGLITE_drawStringOnCircleGradient(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jint x, jint y, jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing, jfloat radius, jint direction, jint *gradientData, jfloat *gradientMatrix){
	return _draw_string_on_circle_gradient(gc, text, faceHandle, size, (jfloat)x, (jfloat)y, matrix, alpha, blend, letterSpacing, radius, direction, gradientData, gradientMatrix);
}

DRAWING_Status VG_DRAWING_VGLITE_drawImage(MICROUI_GraphicsContext* gc, void* image, jfloat *matrix, jint alpha, jlong elapsed, const float color_matrix[], jint* errno){

	DRAWING_Status ret = DRAWING_DONE;
	float width;
	float height;
	VG_DRAWING_get_image_size(image, &width, &height);

	if (UI_VGLITE_enable_vg_lite_scissor_area(gc, width, height, matrix)) {
		render_buffer = UI_VGLITE_configure_destination(gc);
		draw_gradient_flushed = true; // no drawing with gradient has been added yet
		bool rendered;
		*errno = VG_DRAWING_VGLITE_draw_image(&_draw_image_element, image, matrix, alpha, elapsed, color_matrix, &rendered);
		ret = _post_operation(gc, *errno, rendered);
	}
	// nothing to draw

	return ret;
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------
