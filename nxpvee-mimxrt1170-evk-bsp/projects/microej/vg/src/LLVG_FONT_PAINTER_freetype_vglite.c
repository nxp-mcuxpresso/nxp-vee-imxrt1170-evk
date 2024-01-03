/*
 * C
 *
 * Copyright 2020-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
* @file
* @brief MicroEJ MicroVG library low level API: implementation over Freetype and VG-Lite.
* @author MicroEJ Developer Team
* @version 6.1.0
*/

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "microvg_configuration.h"

#if defined VG_FEATURE_FONT && (VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR)

#include <math.h>

#include <freetype/internal/ftobjs.h>
#include <freetype/ftcolor.h>

#include <LLVG_FONT_impl.h>
#include <LLVG_PATH_impl.h>
#include <LLVG_GRADIENT_impl.h>
#include <LLVG_MATRIX_impl.h>
#include <sni.h>

#include "microvg_helper.h"
#include "microvg_font_freetype.h"
#include "microvg_vglite_helper.h"
#include "vg_lite.h"
#include "ftvector/ftvector.h"
#include "vg_drawing_vglite.h"

// -----------------------------------------------------------------------------
// Macros and Defines
// -----------------------------------------------------------------------------

#define FT_COLOR_TO_INT(x) (*((int*) &(x)))
#define INT_TO_FT_COLOR(x) (*((FT_Color*) &(x)))

#define DIRECTION_CLOCK_WISE 0

// -----------------------------------------------------------------------------
// Extern Variables
// -----------------------------------------------------------------------------

extern FT_Library library;
extern FT_Renderer renderer;

// -----------------------------------------------------------------------------
// Global Variables
// -----------------------------------------------------------------------------
static FT_Color *palette;

// -----------------------------------------------------------------------------
// Internal function definitions
// -----------------------------------------------------------------------------

/*
 * @brief Computes the scale to apply to the font.
 *
 * @param[in] size: the font size
 * @param[in] face: the face of the font
 */
static inline float __get_scale(jfloat size, FT_Face face);

/*
 * @brief Sets renderer parameters.
 *
 * @param[in] gc: the graphic context
 * @param[in] matrix: the transformation matrix
 * @param[in] alpha: the opacity of the string (from 0 to 255)
 * @param[in] gradient: the gradient to apply
 */
static void __set_renderer(VG_DRAWING_VGLITE_draw_glyph_t drawer, vg_lite_matrix_t* matrix, int color, vg_lite_linear_gradient_t* gradient, vg_lite_blend_t blend);

/*
 * @brief Sets renderer color parameter.
 *
 * @param[in] color: the color to use to render the glyph
 */
static void __set_color(int color);

/*
 * @brief Updates the angle to use for the next glyph when drawn on an arc.
 * When drawing on an arc, the glyph position is defined by its angle. We thus
 * convert the advance (distance to the next glyph) to an angle.
 */
static float __get_angle(float advance, float radius);

/**
 * @brief load and render the selected glyph. If the glyph is a multilayer glyph,
 * this function will retrieve the different layers glyphs with theirs colors and
 * update the renderer to draw the glyph with the correct color.
 *
 * @param[in] face: the face of the font.
 * @param[in] glyph: the glyph index.
 * @param[in] glyph: the original color of the string index.
 * @param[in,out] matrix: the transformation matrix.
 *
 * @return FT_ERR( Ok ) on a success, a different value otherwise.
 */
static FT_Error __render_glyph(FT_Face face, FT_UInt glyph_index, FT_Color color);

// -----------------------------------------------------------------------------
// Internal functions
// -----------------------------------------------------------------------------

static inline float __get_scale(jfloat size, FT_Face face){
	return size / face->units_per_EM;
}

static void __set_renderer(VG_DRAWING_VGLITE_draw_glyph_t drawer, vg_lite_matrix_t* matrix, int color, vg_lite_linear_gradient_t* gradient, vg_lite_blend_t blend){
	FT_Parameter vglite_params[5];

	vglite_params[0].tag = FT_PARAM_TAG_VGLITE_DESTINATION;
	// cppcheck-suppress [misra-c2012-11.1] pointer conversion to store the drawer
	vglite_params[0].data = (void *) drawer;

	vglite_params[1].tag = FT_PARAM_TAG_VGLITE_MATRIX;
	vglite_params[1].data = matrix;

	vglite_params[2].tag = FT_PARAM_TAG_VGLITE_BLEND;
	vglite_params[2].data = (void *) blend;

	vglite_params[3].tag = FT_PARAM_TAG_VGLITE_COLOR;
	// cppcheck-suppress [misra-c2012-11.6] pointer conversion to store the color
	vglite_params[3].data = (void *) color;

	vglite_params[4].tag = FT_PARAM_TAG_VGLITE_GRADIENT;
	vglite_params[4].data = (void *) gradient;

	FT_Set_Renderer(library, renderer, 5, &vglite_params[0]);
}

static void __set_color(int color) {

	FT_Renderer renderer = FT_Get_Renderer(library, FT_GLYPH_FORMAT_OUTLINE);
	FT_Renderer_SetModeFunc set_mode = renderer->clazz->set_mode;

	// cppcheck-suppress [misra-c2012-11.6] pointer conversion to pass color
	set_mode(renderer, FT_PARAM_TAG_VGLITE_COLOR, (void *)color);
}

static float __get_angle(float advance, float radius){
	return (advance/radius) * 180.0f / M_PI;
}

static FT_Error __render_glyph(FT_Face face, FT_UInt glyph_index, FT_Color color) {
	FT_LayerIterator iterator;

	FT_Bool have_layers;
	FT_UInt layer_glyph_index;
	FT_UInt layer_color_index;

	FT_Error error = FT_ERR( Ok );
	bool color_updated = false;

	iterator.p = NULL;
	have_layers = FT_Get_Color_Glyph_Layer(face, glyph_index,
			&layer_glyph_index, &layer_color_index, &iterator);

	do {
		if (palette && have_layers) {
			// Update renderer color with layer_color
			if (layer_color_index == 0xFFFF){
				__set_color(FT_COLOR_TO_INT(color));
			}
			else {
				__set_color(FT_COLOR_TO_INT(palette[layer_color_index]));
				color_updated = true;
			}
		}
		else {
			// Use main glyph_index as layer_glyph_index
			layer_glyph_index = glyph_index;
		}

		if(layer_glyph_index != glyph_index){
			error = FT_Load_Glyph(face, layer_glyph_index, FT_LOAD_NO_SCALE);
		}

		if (FT_ERR( Ok ) == error) {
			// convert to an anti-aliased bitmap
			error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		}
		else {
			MEJ_LOG_ERROR_MICROVG("Error while loading glyphid %d: 0x%x, refer to fterrdef.h\n",layer_glyph_index, error);
		}
	}
	while ((layer_glyph_index != glyph_index) && (FT_ERR( Ok ) == error) && (FT_ERR( Ok ) != FT_Get_Color_Glyph_Layer(face, glyph_index,
			&layer_glyph_index, &layer_color_index, &iterator)));

	if ((FT_ERR( Ok ) == error) && color_updated) {
		// Revert renderer color to original color in case it has been modified.
		__set_color(FT_COLOR_TO_INT(color));
	}

	return error;
}

// -----------------------------------------------------------------------------
// vg_drawing_vglite.h painter functions
// -----------------------------------------------------------------------------

jint VG_DRAWING_VGLITE_draw_string(VG_DRAWING_VGLITE_draw_glyph_t drawer, jchar* text, jint faceHandle, jfloat size, vg_lite_matrix_t* vgGlobalMatrix, vg_lite_blend_t blend, int color, vg_lite_linear_gradient_t *gradient, jfloat letterSpacing, jfloat radius, jint direction, bool* rendered){
	FT_Face face = (FT_Face) faceHandle;
	jint result = LLVG_SUCCESS;
	*rendered = false;

	// Select palette
	if (0 != FT_Palette_Select(face, 0, &palette)){
		palette = NULL;
	}

	float scale = __get_scale(size, face);
	float letterSpacingScaled = letterSpacing / scale;
	float radiusScaled = radius / scale;

	// baselineposition
	short baselineposition = face->ascender;

	jfloat* vgLocalMatrix = MAP_VGLITE_MATRIX(vgGlobalMatrix);
	LLVG_MATRIX_IMPL_scale(vgLocalMatrix, scale, scale);

	vg_lite_matrix_t localGlyphMatrix;
	jfloat* vglocalGlyphMatrix = MAP_VGLITE_MATRIX(&localGlyphMatrix);

	__set_renderer(drawer, &localGlyphMatrix, color, gradient, blend);

	// Layout variables
	int glyph_index ; // current glyph index

	int length = (int)SNI_getArrayLength(text);

	if (0 < length) {

		int glyph_offset_y;
		int glyph_advance_x;
		int glyph_advance_y;
		int glyph_offset_x;
		int advance_x = 0;
		int advance_Y = 0;
		int previous_glyph_index = 0; // previous glyph index for kerning

		MICROVG_HELPER_layout_configure(faceHandle, text, length);

		while((LLVG_SUCCESS == result) && (MICROVG_HELPER_layout_load_glyph(&glyph_index, &glyph_advance_x, &glyph_advance_y, &glyph_offset_x, &glyph_offset_y))) {
			// At that point the current glyph has been loaded by Freetype

			int charWidth = glyph_advance_x;

			if (0 == previous_glyph_index){
				// first glyph: remove the first blank line
				if( 0 == face->glyph->metrics.width) {
					advance_x -= charWidth;
				}
				else {
					advance_x -= face->glyph->metrics.horiBearingX;
				}
			}

			LLVG_MATRIX_IMPL_copy(vglocalGlyphMatrix, vgLocalMatrix);

			if(0.f == radius) {
				LLVG_MATRIX_IMPL_translate(vglocalGlyphMatrix, advance_x + glyph_offset_x, baselineposition + advance_Y  + glyph_offset_y);
			} else {
				float sign = (DIRECTION_CLOCK_WISE != direction) ? -1.f : 1.f;

				// Space characters joining bboxes at baseline
				float angleDegrees = 90 + __get_angle(advance_x + glyph_offset_x, radiusScaled)	+ __get_angle(charWidth / 2, radiusScaled);

				// Rotate to angle
				LLVG_MATRIX_IMPL_rotate(vglocalGlyphMatrix, sign * angleDegrees);

				// Translate left to center of bbox
				// Translate baseline over circle
				LLVG_MATRIX_IMPL_translate(vglocalGlyphMatrix, -charWidth / 2, -sign * radiusScaled);
			}

			// Draw the glyph
			FT_Error error = __render_glyph(face, glyph_index, INT_TO_FT_COLOR(color));
			if (FT_ERR( Ok ) != error) {
				MEJ_LOG_ERROR_MICROVG("Error while rendering glyphid %d: 0x%x, refer to fterrdef.h\n",glyph_index, error);
				result = (FT_ERR( Out_Of_Memory ) == error) ? LLVG_OUT_OF_MEMORY : LLVG_DATA_INVALID;
				continue;
			}

			// at least one glyph has been drawn
			*rendered = true;

			// Compute advance to next glyph
			advance_x += charWidth;
			advance_x += (int)letterSpacingScaled;

			advance_Y += glyph_advance_y;

			previous_glyph_index = glyph_index;
		}
	}

	return result;
}

#endif // defined VG_FEATURE_FONT && (VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR)

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------
