/*
 * C
 *
 * Copyright 2023-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief This file implements all MicroVG drawing native functions.
 * @see LLVG_PAINTER_impl.h file comment
 * @author MicroEJ Developer Team
 * @version 6.0.1
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// implements LLVG_PAINTER_impl functions
#include <LLVG_PAINTER_impl.h>

// use graphical engine functions to synchronize drawings
#include <LLUI_DISPLAY.h>

#include <LLVG_FONT_impl.h>
#include <LLVG_MATRIX_impl.h>

// calls vg_drawing functions
#include "vg_drawing.h"
#include "vg_helper.h"
#include "vg_trace.h"

// -----------------------------------------------------------------------------
// Macros and Defines
// -----------------------------------------------------------------------------

/*
 * @brief Macro to add a DRAWING event and its type.
 */
#define LOG_MICROVG_DRAWING_START(fn) LOG_MICROVG_START(LOG_MICROVG_DRAWING_ID, CONCAT_DEFINES(LOG_MICROVG_DRAW_, fn))
#define LOG_MICROVG_DRAWING_END(fn) LOG_MICROVG_END(LOG_MICROVG_DRAWING_ID, CONCAT_DEFINES(LOG_MICROVG_DRAW_, fn))

// -----------------------------------------------------------------------------
// LLVG_PAINTER_impl.h functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
jint LLVG_PAINTER_IMPL_drawPath(MICROUI_GraphicsContext *gc, jbyte *pathData, jint x, jint y, jfloat *matrix,
                                jint fillRule, jint blend, jint color) {
	if (LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) & LLVG_PAINTER_IMPL_drawPath)) {
		LOG_MICROVG_DRAWING_START(path);
		jfloat translated_matrix[LLVG_MATRIX_SIZE];
		VG_HELPER_prepare_matrix(translated_matrix, x, y, matrix);
		DRAWING_Status status = VG_DRAWING_drawPath(gc, pathData, translated_matrix, fillRule, blend, color);
		LLUI_DISPLAY_setDrawingStatus(status);
		LOG_MICROVG_DRAWING_END(path);
	}
	return LLVG_SUCCESS;
}

// See the header file for the function documentation
jint LLVG_PAINTER_IMPL_drawGradient(MICROUI_GraphicsContext *gc, jbyte *pathData, jint x, jint y, jfloat *matrix,
                                    jint fillRule, jint alpha, jint blend, jint *gradientData, jfloat *gradientMatrix) {
	if (LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) & LLVG_PAINTER_IMPL_drawGradient)) {
		LOG_MICROVG_DRAWING_START(pathGradient);
		jfloat translated_matrix[LLVG_MATRIX_SIZE];
		VG_HELPER_prepare_matrix(translated_matrix, x, y, matrix);
		DRAWING_Status status = VG_DRAWING_drawGradient(gc, pathData, translated_matrix, fillRule, alpha, blend,
		                                                gradientData, gradientMatrix);
		LLUI_DISPLAY_setDrawingStatus(status);
		LOG_MICROVG_DRAWING_END(pathGradient);
	}
	return LLVG_SUCCESS;
}

// See the header file for the function documentation
jint LLVG_PAINTER_IMPL_drawString(MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle, jfloat size, jfloat x,
                                  jfloat y, jfloat *matrix, jint alpha, jint blend, jfloat letterSpacing) {
	jint ret;
	if (LLVG_FONT_UNLOADED == faceHandle) {
		ret = (jint)LLVG_RESOURCE_CLOSED;
	} else {
		if (LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) & (LLVG_PAINTER_IMPL_drawString))) {
			LOG_MICROVG_DRAWING_START(string);
			jfloat translated_matrix[LLVG_MATRIX_SIZE];
			VG_HELPER_prepare_matrix(translated_matrix, x, y, matrix);
			LLUI_DISPLAY_setDrawingStatus(VG_DRAWING_drawString(gc, text, faceHandle, size, translated_matrix, alpha,
			                                                    blend, letterSpacing));
			LOG_MICROVG_DRAWING_END(string);
		}
		ret = (jint)LLVG_SUCCESS;
	}
	return ret;
}

// See the header file for the function documentation
jint LLVG_PAINTER_IMPL_drawStringGradient(MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle, jfloat size,
                                          jfloat x, jfloat y, jfloat *matrix, jint alpha, jint blend,
                                          jfloat letterSpacing, jint *gradientData, jfloat *gradientMatrix) {
	jint ret;

	if (LLVG_FONT_UNLOADED == faceHandle) {
		ret = (jint)LLVG_RESOURCE_CLOSED;
	} else {
		if (LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) & (LLVG_PAINTER_IMPL_drawStringGradient))) {
			LOG_MICROVG_DRAWING_START(stringGradient);
			jfloat translated_matrix[LLVG_MATRIX_SIZE];
			VG_HELPER_prepare_matrix(translated_matrix, x, y, matrix);
			LLUI_DISPLAY_setDrawingStatus(VG_DRAWING_drawStringGradient(gc, text, faceHandle, size, translated_matrix,
			                                                            alpha, blend, letterSpacing, gradientData,
			                                                            gradientMatrix));
			LOG_MICROVG_DRAWING_END(stringGradient);
		}
		ret = (jint)LLVG_SUCCESS;
	}

	return ret;
}

// See the header file for the function documentation
jint LLVG_PAINTER_IMPL_drawStringOnCircle(MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle, jfloat size,
                                          jint x, jint y, jfloat *matrix, jint alpha, jint blend, jfloat letterSpacing,
                                          jfloat radius, jint direction) {
	jint ret;

	if (LLVG_FONT_UNLOADED == faceHandle) {
		ret = (jint)LLVG_RESOURCE_CLOSED;
	} else {
		if (LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) & (LLVG_PAINTER_IMPL_drawStringOnCircle))) {
			LOG_MICROVG_DRAWING_START(stringOnCircle);
			jfloat translated_matrix[LLVG_MATRIX_SIZE];
			VG_HELPER_prepare_matrix(translated_matrix, x, y, matrix);
			LLUI_DISPLAY_setDrawingStatus(VG_DRAWING_drawStringOnCircle(gc, text, faceHandle, size, translated_matrix,
			                                                            alpha, blend, letterSpacing, radius,
			                                                            direction));
			LOG_MICROVG_DRAWING_END(stringOnCircle);
		}
		ret = (jint)LLVG_SUCCESS;
	}

	return ret;
}

// See the header file for the function documentation
jint LLVG_PAINTER_IMPL_drawStringOnCircleGradient(MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle,
                                                  jfloat size, jint x, jint y, jfloat *matrix, jint alpha, jint blend,
                                                  jfloat letterSpacing, jfloat radius, jint direction,
                                                  jint *gradientData, jfloat *gradientMatrix) {
	jint ret;

	if (LLVG_FONT_UNLOADED == faceHandle) {
		ret = (jint)LLVG_RESOURCE_CLOSED;
	} else {
		if (LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) & (LLVG_PAINTER_IMPL_drawStringOnCircleGradient))) {
			LOG_MICROVG_DRAWING_START(stringOnCircleGradient);
			jfloat translated_matrix[LLVG_MATRIX_SIZE];
			VG_HELPER_prepare_matrix(translated_matrix, x, y, matrix);
			LLUI_DISPLAY_setDrawingStatus(VG_DRAWING_drawStringOnCircleGradient(gc, text, faceHandle, size,
			                                                                    translated_matrix, alpha, blend,
			                                                                    letterSpacing, radius, direction,
			                                                                    gradientData, gradientMatrix));
			LOG_MICROVG_DRAWING_END(stringOnCircleGradient);
		}
		ret = (jint)LLVG_SUCCESS;
	}

	return ret;
}

// See the header file for the function documentation
jint LLVG_PAINTER_IMPL_drawImage(MICROUI_GraphicsContext *gc, MICROVG_Image *image, jint x, jint y, jfloat *matrix,
                                 jint alpha, jlong elapsed, const float color_matrix[]) {
	jint error = LLVG_SUCCESS;

	if (LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) & LLVG_PAINTER_IMPL_drawImage)) {
		DRAWING_Status status;
		LOG_MICROVG_DRAWING_START(image);
		if (!VG_DRAWING_image_is_closed(image) && alpha > (uint32_t)0) {
			// cppcheck-suppress [misra-c2012-18.8] LLVG_MATRIX_SIZE is a fixed size
			jfloat translated_matrix[LLVG_MATRIX_SIZE];
			VG_HELPER_prepare_matrix(translated_matrix, x, y, matrix);

			status = VG_DRAWING_drawImage(gc, image, translated_matrix, alpha, elapsed, color_matrix, &error);

			/* FIXME LLVG_OUT_OF_MEMORY errors are not returned as they are reported with error flags.
			 * See M0092MEJAUI-2908.
			 */
			if (LLVG_OUT_OF_MEMORY == error) {
				error = LLVG_SUCCESS;
			}
		} else {
			status = DRAWING_DONE;
		}
		LLUI_DISPLAY_setDrawingStatus(status);
		LOG_MICROVG_DRAWING_END(image);
	}
	return error;
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------
