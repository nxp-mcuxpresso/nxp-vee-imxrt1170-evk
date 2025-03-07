/*
 * C
 *
 * Copyright 2020-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief MicroEJ MicroVG library low level API: implementation over FreeType.
 * @author MicroEJ Developer Team
 * @version 6.0.1
 */

#if !defined VG_FREETYPE_H
#define VG_FREETYPE_H

#if defined __cplusplus
extern "C" {
#endif

#include "vg_configuration.h"

#if defined VG_FEATURE_FONT &&                                                              \
	(defined VG_FEATURE_FONT_FREETYPE_VECTOR || defined VG_FEATURE_FONT_FREETYPE_BITMAP) && \
	(VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR || VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_BITMAP)

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "vg_path.h"

// --------------------------------------------------------------------------------
// Typedef
// --------------------------------------------------------------------------------

/*
 * @brief Function to draw a character element: a glyph. A glyph is a vectorial path.
 *
 * This function is called by VG_FREETYPE_draw_string(). Two implementations
 * are required: one that draws the path with a color and one that draws the path with
 * a gradient.
 *
 * The list of the drawing parameters is reduced to the elements that the Freetype engine
 * changes for each glyph: the glyph's path, the transformation to apply on the glyph,
 * the glyph color (useful for colored glyphs like emoji), the glyph's outine.
 *
 * All others elements required by the drawer (if any) have to be listed in the dedicated
 * structure user_data.
 *
 * @return: the MicroVG error code
 */
typedef jint (* VG_FREETYPE_draw_glyph_t) (
	/*
	 * @brief The path of the glyph.
	 */
	VG_PATH_HEADER_t *path,

	/*
	 * @brief The deformation to apply on the path.
	 */
	jfloat *matrix,

	/*
	 * @brief The specific glyph color (example: emoji) or the current destination
	 * color. Useless if the string is drawn with a gradient.
	 */
	uint32_t color,

	/*
	 * @brief The glyph outline: true: EVEN_ODD or false: NON_ZERO.
	 */
	bool fill_rule_even_odd,

	/*
	 * @brief The custom drawer data (may be null). For instance this structure can
	 * hold the gradient if the GPU's drawing function requires it. In case of
	 * the gradient is set before calling VG_FREETYPE_draw_string(), the
	 * gradient doesn't need to be stored in this structure.
	 */
	void *user_data);

// --------------------------------------------------------------------------------
// API
// --------------------------------------------------------------------------------

/*
 * @brief Initializes the lowlevel font library.
 */
void VG_FREETYPE_initialize(void);

/*
 * @brief Draws a string using the Freetype engine along a line or a circle, with a
 * color or a linear gradient. The implementation does not draw, it calls the
 * drawer function for each glyph: VG_FREETYPE_draw_glyph_t.
 *
 * @param[in] drawer the function to draw a glyph.
 * @param[in] text the array of characters to draw.
 * @param[in] face_handle the font reference handle.
 * @param[in] size the height of the font in pixels.
 * @param[in] matrix the transformation to apply.
 * @param[in] color the 32-bit color to apply (useless when drawing with a gradient).
 * @param[in] letter_spacing the extra letter spacing to use.
 * @param[in] radius the radius of the circle (0 to draw along a line).
 * @param[in] direction the direction of the text along the circle (0 to draw along a line).
 * @param[in] user_data the data used by the drawer; may be null.
 *
 * @return LLVG_SUCCESS if something has been drawn, an different value otherwise
 */
jint VG_FREETYPE_draw_string(VG_FREETYPE_draw_glyph_t drawer, const jchar *text, jint face_handle, jfloat size,
                             const jfloat *matrix, uint32_t color, jfloat letter_spacing, jfloat radius, jint direction,
                             void *user_data);

// -----------------------------------------------------------------------------
// Implementation
// -----------------------------------------------------------------------------

/*
 * @brief Converts an ARGB8888 color to a format compatible with the GPU. The default
 * implementation does nothing and just returns the original color.
 */
jint VG_FREETYPE_IMPL_convert_color(jint color);

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------
#endif \
    // defined VG_FEATURE_FONT && \
    // (defined VG_FEATURE_FONT_FREETYPE_VECTOR || defined VG_FEATURE_FONT_FREETYPE_BITMAP) && \
    // (VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR || VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_BITMAP)

#ifdef __cplusplus
}
#endif

#endif // !defined VG_FREETYPE_H
