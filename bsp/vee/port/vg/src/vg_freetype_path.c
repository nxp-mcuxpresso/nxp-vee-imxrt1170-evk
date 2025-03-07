/*
 * C
 *
 * Copyright 2020-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief MicroEJ MicroVG library low level API: implementation over Freetype.
 * @author MicroEJ Developer Team
 * @version 6.0.1
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "vg_configuration.h"

#if defined VG_FEATURE_FONT && (VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR)

#include <math.h>

#include <freetype/internal/ftobjs.h>
#include <freetype/ftcolor.h>
#include "ftvector/ftvector.h"

#include <LLVG_FONT_impl.h>
#include <LLVG_PATH_impl.h>
#include <LLVG_GRADIENT_impl.h>
#include <LLVG_MATRIX_impl.h>
#include <sni.h>

#include "vg_helper.h"
#include "bsp_util.h"

// -----------------------------------------------------------------------------
// Macros and Defines
// -----------------------------------------------------------------------------

#define FT_COLOR_TO_INT(x) (*((int *)&(x)))

#define DIRECTION_CLOCK_WISE 0

// -----------------------------------------------------------------------------
// Extern Variables
// -----------------------------------------------------------------------------

extern FT_Library library;
extern FT_Renderer renderer;

// -----------------------------------------------------------------------------
// Internal functions
// -----------------------------------------------------------------------------

/*
 * @brief Computes the scale to apply to the font.
 *
 * @param[in] size: the font size
 * @param[in] face: the face of the font
 */
static inline float __get_scale(jfloat size, FT_Face face) {
	return size / face->units_per_EM;
}

/*
 * @brief Sets renderer parameters.
 */
static void __set_renderer(FTVECTOR_draw_glyph_data_t *data) {
	FT_Parameter params[1];

	params[0].tag = FT_PARAM_TAG_DRAWER;
	// cppcheck-suppress [misra-c2012-11.1] pointer conversion to store the drawer
	params[0].data = (void *)data;

	FT_Set_Renderer(library, renderer, 1, &params[0]);
}

/*
 * @brief Updates the angle to use for the next glyph when drawn on an arc.
 * When drawing on an arc, the glyph position is defined by its angle. We thus
 * convert the advance (distance to the next glyph) to an angle.
 */
static float __get_angle(float advance, float radius) {
	float angle = advance / radius;
	angle *= 180.0f;
	angle /= M_PI;
	return angle;
}

/**
 * @brief load and render the selected glyph. If the glyph is a multilayer glyph,
 * this function will retrieve the different layers glyphs with theirs colors and
 * update the renderer to draw the glyph with the correct color.
 *
 * @param[in] face: the face of the font.
 * @param[in] glyph: the glyph index.
 *
 * @return FT_ERR( Ok ) on a success, a different value otherwise.
 */
static FT_Error __render_glyph(FT_Face face, FT_UInt glyph_index, FT_Color *palette,
                               FTVECTOR_draw_glyph_data_t *drawer_data) {
	FT_Error error = FT_ERR(Ok);

	uint32_t default_color = drawer_data->color;
	FT_UInt layer_glyph_index;
	FT_UInt layer_color_index;
	FT_LayerIterator iterator;
	iterator.p = NULL;

	FT_Bool have_layers = palette && FT_Get_Color_Glyph_Layer(face, glyph_index, &layer_glyph_index, &layer_color_index,
	                                                          &iterator);

	do {
		if (have_layers) {
			// Update renderer color with layer_color
			if (layer_color_index != 0xFFFF) {
				drawer_data->color = VG_FREETYPE_IMPL_convert_color(FT_COLOR_TO_INT(palette[layer_color_index]));
			}
		} else {
			// Use main glyph_index as layer_glyph_index
			layer_glyph_index = glyph_index;
		}

		if (layer_glyph_index != glyph_index) {
			error = FT_Load_Glyph(face, layer_glyph_index, FT_LOAD_NO_SCALE);
		}

		if (FT_ERR(Ok) == error) {
			// convert to an anti-aliased bitmap
			error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		} else {
			MEJ_LOG_ERROR_MICROVG("Error while loading glyphid %d: 0x%x, refer to fterrdef.h\n", layer_glyph_index,
			                      error);
		}
	}while ((layer_glyph_index != glyph_index) && (FT_ERR(Ok) == error) && (FT_ERR(Ok) != FT_Get_Color_Glyph_Layer(face,
	                                                                                                               glyph_index,
	                                                                                                               &
	                                                                                                               layer_glyph_index,
	                                                                                                               &
	                                                                                                               layer_color_index,
	                                                                                                               &
	                                                                                                               iterator)));

	// Revert renderer color to original color in case it has been modified.
	drawer_data->color = default_color;

	return error;
}

// -----------------------------------------------------------------------------
// vg_freetype.h painter functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
jint VG_FREETYPE_draw_string(VG_FREETYPE_draw_glyph_t drawer, const jchar *text, jint faceHandle, jfloat size,
                             const jfloat *matrix, uint32_t color, jfloat letterSpacing, jfloat radius, jint direction,
                             void *user_data) {
	jint result = LLVG_SUCCESS;
	int length = (int)SNI_getArrayLength(text);

	if (0 < length) {
		FT_Face face = (FT_Face)faceHandle;
		FT_Color *palette;

		// Select palette
		if (0 != FT_Palette_Select(face, 0, &palette)) {
			palette = NULL;
		}

		float scale = __get_scale(size, face);
		float letterSpacingScaled = letterSpacing / scale;
		float radiusScaled = radius / scale;
		short baselineposition = face->ascender;

		// transformation to apply on all glyphs
		float scaled_matrix[LLVG_MATRIX_SIZE];
		LLVG_MATRIX_IMPL_copy(scaled_matrix, matrix);
		LLVG_MATRIX_IMPL_scale(scaled_matrix, scale, scale);

		float working_matrix[LLVG_MATRIX_SIZE];
		LLVG_MATRIX_IMPL_copy(working_matrix, scaled_matrix); // TODO Is it necessary?

		FTVECTOR_draw_glyph_data_t drawer_data;
		drawer_data.drawer = drawer;
		drawer_data.matrix = working_matrix;
		drawer_data.color = color;
		drawer_data.user_data = user_data;

		// give drawing parameters to freetype
		__set_renderer(&drawer_data);

		int glyph_index;  // current glyph index
		int glyph_offset_y;
		int glyph_advance_x;
		int glyph_advance_y;
		int glyph_offset_x;
		int advance_x = 0;
		int advance_y = 0;
		int previous_glyph_index = 0; // previous glyph index for kerning

		VG_HELPER_layout_configure(faceHandle, text, length);

		while ((LLVG_SUCCESS == result) && (VG_HELPER_layout_load_glyph(&glyph_index, &glyph_advance_x,
		                                                                &glyph_advance_y, &glyph_offset_x,
		                                                                &glyph_offset_y))) {
			// At that point the current glyph has been loaded by Freetype

			int charWidth = glyph_advance_x;

			if (0 == previous_glyph_index) {
				// first glyph: remove the first blank line
				if (0 == face->glyph->metrics.width) {
					advance_x -= charWidth;
				} else {
					advance_x -= face->glyph->metrics.horiBearingX;
				}
			}

			// reset drawer's matrix
			LLVG_MATRIX_IMPL_copy(working_matrix, scaled_matrix);

			if (0.f == radius) {
				LLVG_MATRIX_IMPL_translate(working_matrix, advance_x + glyph_offset_x,
				                           baselineposition + advance_y + glyph_offset_y);
			} else {
				float sign = (DIRECTION_CLOCK_WISE != direction) ? -1.f : 1.f;

				// Space characters joining bboxes at baseline
				float angleDegrees = 90 + __get_angle(advance_x + glyph_offset_x,
				                                      radiusScaled) + __get_angle(charWidth / 2, radiusScaled);

				// Rotate to angle
				LLVG_MATRIX_IMPL_rotate(working_matrix, sign * angleDegrees);

				// Translate left to center of bbox
				// Translate baseline over circle
				LLVG_MATRIX_IMPL_translate(working_matrix, -charWidth / 2, -sign * radiusScaled);
			}

			// Draw the glyph
			FT_Error error = __render_glyph(face, glyph_index, palette, &drawer_data);
			if (FT_ERR(Ok) != error) {
				MEJ_LOG_ERROR_MICROVG("Error while rendering glyphid %d: 0x%x, refer to fterrdef.h\n", glyph_index,
				                      error);
				result = (FT_ERR(Out_Of_Memory) == error) ? LLVG_OUT_OF_MEMORY : LLVG_DATA_INVALID;
				continue;
			}

			// Compute advance to next glyph
			advance_x += charWidth;
			advance_x += (int)letterSpacingScaled;
			advance_y += glyph_advance_y;

			previous_glyph_index = glyph_index;
		}
	}

	return result;
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT jint VG_FREETYPE_IMPL_convert_color(jint color) {
	return color;
}

#endif // defined VG_FEATURE_FONT && (VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR)

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------
