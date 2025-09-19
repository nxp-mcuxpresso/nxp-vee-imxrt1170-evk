/*
 * C
 *
 * Copyright 2024-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief Redirection of all MicroVG library drawing functions. This file is required
 * (must be compiled in the C project) but should not be modified (except when the VEE
 * port uses more than 3 destination formats, see below).
 *
 * This file follows strictly the same concept as ui_drawing.c but for MicroVG
 * library.
 *
 * @author MicroEJ Developer Team
 * @version 7.0.1
 * @see ui_drawing.c
 */

// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------

#include <math.h>

#include <LLVG_FONT_impl.h>
#include <LLVG_MATRIX_impl.h>

#include "ui_font_drawing.h"
#include "vg_drawing.h"
#include "vg_freetype.h"

// --------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------

/*
 * @brief Redirects all ui_font_drawing.h functions to this file
 */
#define UI_FONT_DRAWING_VG_FUNCTIONS_SUFFIX GET_CUSTOM_FONT_FUNCTIONS_SUFFIX(UI_FONT_FORMAT_VG)
#define UI_FONT_DRAWING_VG_stringWidth CONCAT(UI_FONT_DRAWING_stringWidth_custom, UI_FONT_DRAWING_VG_FUNCTIONS_SUFFIX)
#define UI_FONT_DRAWING_VG_initializeRenderableStringSNIContext CONCAT( \
			UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom, UI_FONT_DRAWING_VG_FUNCTIONS_SUFFIX)
#define UI_FONT_DRAWING_VG_drawString CONCAT(UI_FONT_DRAWING_drawString_custom, UI_FONT_DRAWING_VG_FUNCTIONS_SUFFIX)
#define UI_FONT_DRAWING_VG_drawRenderableString CONCAT(UI_FONT_DRAWING_drawRenderableString_custom, \
													   UI_FONT_DRAWING_VG_FUNCTIONS_SUFFIX)
#define UI_FONT_DRAWING_VG_drawScaledStringBilinear CONCAT(UI_FONT_DRAWING_drawScaledStringBilinear_custom, \
														   UI_FONT_DRAWING_VG_FUNCTIONS_SUFFIX)
#define UI_FONT_DRAWING_VG_drawScaledRenderableStringBilinear CONCAT( \
			UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom, UI_FONT_DRAWING_VG_FUNCTIONS_SUFFIX)
#define UI_FONT_DRAWING_VG_drawCharWithRotationBilinear CONCAT(UI_FONT_DRAWING_drawCharWithRotationBilinear_custom, \
															   UI_FONT_DRAWING_VG_FUNCTIONS_SUFFIX)
#define UI_FONT_DRAWING_VG_drawCharWithRotationNearestNeighbor CONCAT( \
			UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom, UI_FONT_DRAWING_VG_FUNCTIONS_SUFFIX)

// --------------------------------------------------------------------------------
// Typedefs
// --------------------------------------------------------------------------------

/**
 * @brief Represents a MicroUI Font extended by MicroVG
 *
 * @see getFont(int size) in VectorFont.java
 */
typedef struct {
	/**
	 * @brief The MicroUI Font header
	 */
	MICROUI_Font ui_font;

	/**
	 * @brief The identifier of the MicroVG Font.
	 */
	jint vg_font_handle;

	/**
	 * @brief The size to apply on the MicroVG Font.
	 */
	jint vg_font_size;
} MICROUI_FontVG;

// --------------------------------------------------------------------------------
// ui_font_drawing.h functions
// --------------------------------------------------------------------------------

// See the header file for the function documentation
jint UI_FONT_DRAWING_VG_stringWidth(jchar *chars, jint length, MICROUI_Font *font) {
#if defined VG_FEATURE_FONT &&                                                              \
	(defined VG_FEATURE_FONT_FREETYPE_VECTOR || defined VG_FEATURE_FONT_FREETYPE_BITMAP) && \
	(VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR || VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_BITMAP)
	MICROUI_FontVG *vg_font = (MICROUI_FontVG *)font;
	return (int)ceil(VG_FREETYPE_string_width(chars, length, vg_font->vg_font_handle, vg_font->vg_font_size,
	                                          LLVG_FONT_DEFAULT_LETTER_SPACING));
#else
	(void)chars;
	(void)length;
	(void)font;
	return 0;
#endif
}

// See the header file for the function documentation
DRAWING_Status UI_FONT_DRAWING_VG_drawString(MICROUI_GraphicsContext *gc, jchar *chars, jint length, MICROUI_Font *font,
                                             jint x, jint y) {
	MICROUI_FontVG *vg_font = (MICROUI_FontVG *)font;
	jfloat translated_matrix[LLVG_MATRIX_SIZE];
	LLVG_MATRIX_IMPL_setTranslate(translated_matrix, x, y);
	return VG_DRAWING_drawString(gc, chars, length, vg_font->vg_font_handle, vg_font->vg_font_size, translated_matrix,
	                             0xff /* fully opaque */, LLVG_BLEND_SRC_OVER, LLVG_FONT_DEFAULT_LETTER_SPACING);
}

// See the header file for the function documentation
DRAWING_Status UI_FONT_DRAWING_VG_drawScaledStringBilinear(MICROUI_GraphicsContext *gc, jchar *chars, jint length,
                                                           MICROUI_Font *font, jint x, jint y, jfloat xRatio,
                                                           jfloat yRatio) {
	MICROUI_FontVG *vg_font = (MICROUI_FontVG *)font;
	jfloat translated_matrix[LLVG_MATRIX_SIZE];
	LLVG_MATRIX_IMPL_setScale(translated_matrix, xRatio, yRatio);
	LLVG_MATRIX_IMPL_postTranslate(translated_matrix, x, y);
	return VG_DRAWING_drawString(gc, chars, length, vg_font->vg_font_handle, vg_font->vg_font_size, translated_matrix,
	                             0xff /* fully opaque */, LLVG_BLEND_SRC_OVER, LLVG_FONT_DEFAULT_LETTER_SPACING);
}

// See the header file for the function documentation
DRAWING_Status UI_FONT_DRAWING_VG_drawCharWithRotationBilinear(MICROUI_GraphicsContext *gc, jchar c, MICROUI_Font *font,
                                                               jint x, jint y, jint xRotation, jint yRotation,
                                                               jfloat angle, jint alpha) {
	MICROUI_FontVG *vg_font = (MICROUI_FontVG *)font;
	jfloat translated_matrix[LLVG_MATRIX_SIZE];

	// retrieve the non-translated rotation center
	jint relative_x_rotation = xRotation - x;
	jint relative_y_rotation = yRotation - y;

	// rotate around the rotation center
	LLVG_MATRIX_IMPL_setTranslate(translated_matrix, -relative_x_rotation, -relative_y_rotation);
	LLVG_MATRIX_IMPL_postRotate(translated_matrix, angle);
	LLVG_MATRIX_IMPL_postTranslate(translated_matrix, relative_x_rotation, relative_y_rotation);

	// translate to the destination point
	LLVG_MATRIX_IMPL_postTranslate(translated_matrix, x, y);

	return VG_DRAWING_drawString(gc, &c, 1, vg_font->vg_font_handle, vg_font->vg_font_size, translated_matrix, alpha,
	                             LLVG_BLEND_SRC_OVER, LLVG_FONT_DEFAULT_LETTER_SPACING);
}

// See the header file for the function documentation
DRAWING_Status UI_FONT_DRAWING_VG_drawCharWithRotationNearestNeighbor(MICROUI_GraphicsContext *gc, jchar c,
                                                                      MICROUI_Font *font, jint x, jint y,
                                                                      jint xRotation, jint yRotation, jfloat angle,
                                                                      jint alpha) {
	return UI_FONT_DRAWING_VG_drawCharWithRotationBilinear(gc, c, font, x, y, xRotation, yRotation, angle, alpha);
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------
