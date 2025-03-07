/*
 * C
 *
 * Copyright 2022-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief MicroEJ MicroVG library low level API: helper to implement library natives
 * methods.
 * @author MicroEJ Developer Team
 * @version 6.0.1
 */

#include "vg_configuration.h"

// -----------------------------------------------------------------------------
// VG Pack Sanity Check
// -----------------------------------------------------------------------------

/*
 * Sanity check between the expected version of the VG Pack used by the VEE Port
 * and the actual version of this file (MicroVG CCO's file).
 *
 * If an error is raised here, it means that a new version of the VG Pack has been
 * installed in the VEE Port or the MicroVG CCO has been updated without any change
 * in the VEE Port. In both cases, the versions must be coherent (see the Release
 * Notes: https://docs.microej.com/en/latest/VEEPortingGuide/vgReleaseNotes.html).
 */

#if (defined(LLVG_MAJOR_VERSION) && (LLVG_MAJOR_VERSION != 1)) || (defined(LLVG_MINOR_VERSION) && \
	(LLVG_MINOR_VERSION < 3))
#error "This CCO is only compatible with VG Pack [1.3.0,2.0.0["
#endif

// -----------------------------------------------------------------------------
// Configuration Sanity Check
// -----------------------------------------------------------------------------

/*
 * Sanity check between the expected version of the configuration and the actual
 * version of the configuration.
 *
 * If an error is raised here, it means that a new version of the CCO has been
 * installed and the configuration vg_configuration.h must be updated based
 * on the one provided by the new CCO version.
 */

#if !defined MICROVG_CONFIGURATION_VERSION
	#error "Undefined MICROVG_CONFIGURATION_VERSION, it must be defined in vg_configuration.h"
#endif

#if defined MICROVG_CONFIGURATION_VERSION && MICROVG_CONFIGURATION_VERSION != 3
	#error "Version of the configuration file vg_configuration.h is not compatible with this implementation."
#endif

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <LLVG_MATRIX_impl.h>

#include "vg_helper.h"
#include "vg_trace.h"
#include "vg_drawing.h"

#if defined VG_FEATURE_PATH
#include "vg_path.h"
#endif

#if defined VG_FEATURE_FONT
#include <freetype/internal/ftobjs.h>
#include "vg_freetype.h"
#endif

#if defined VG_FEATURE_FONT_COMPLEX_LAYOUT
#include "hb.h"
#include "hb-ft.h"
#endif

// -----------------------------------------------------------------------------
// Defines
// -----------------------------------------------------------------------------

#define MIN_HIGH_SURROGATE  ((unsigned short)0xD800)
#define MAX_HIGH_SURROGATE  ((unsigned short)0xDBFF)
#define MIN_LOW_SURROGATE   ((unsigned short)0xDC00)
#define MAX_LOW_SURROGATE   ((unsigned short)0xDFFF)
#define MIN_SUPPLEMENTARY_CODE_POINT 0x010000

#define GET_NEXT_CHARACTER(t, l, o) ((o) >= (l) ? (unsigned short)0 : (t)[o])

#ifdef VG_FEATURE_FONT_COMPLEX_LAYOUT
#define IS_SIMPLE_LAYOUT (!(face->face_flags & FT_FACE_FLAG_COMPLEX_LAYOUT))
#else
#define IS_SIMPLE_LAYOUT true
#endif // VG_FEATURE_FONT_COMPLEX_LAYOUT

// -----------------------------------------------------------------------------
// Public Globals
// -----------------------------------------------------------------------------

/*
 * vg_trace.h logs group identifier
 */
int32_t VG_TRACE_group_id;

// -----------------------------------------------------------------------------
// Private Globals
// -----------------------------------------------------------------------------

static jfloat g_identity_matrix[LLVG_MATRIX_SIZE];

#if defined VG_FEATURE_FONT
static FT_Face face;

// Freetype layout variables
static const unsigned short *current_text;
static unsigned int current_length;
static int current_offset;
static FT_UInt previous_glyph_index; // previous glyph index for kerning
#endif

#if defined VG_FEATURE_FONT_COMPLEX_LAYOUT
// Harfbuzz layout variables
static hb_glyph_info_t *glyph_info;
static hb_glyph_position_t *glyph_pos;
static unsigned int glyph_count;
static int current_glyph;
static hb_buffer_t *buf;
#endif

// -----------------------------------------------------------------------------
// LLVG_impl.h functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
void VG_HELPER_initialize(void) {
	// initializes the logger
	VG_TRACE_group_id = LLTRACE_IMPL_declare_event_group("MicroVG", LOG_MICROVG_EVENTS);

	// configures the matrix used as identity matrix (immutable)
	LLVG_MATRIX_IMPL_identity(g_identity_matrix);

	// Configures the drawing engine
	VG_DRAWING_initialize();

	// configures the path engine
#ifdef VG_FEATURE_PATH
	VG_PATH_initialize();
#endif

	// configures the font engine
#if defined VG_FEATURE_FONT &&                                                              \
	(defined VG_FEATURE_FONT_FREETYPE_VECTOR || defined VG_FEATURE_FONT_FREETYPE_BITMAP) && \
	(VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR || VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_BITMAP)

	VG_FREETYPE_initialize();
#endif
}

// See the header file for the function documentation
int VG_HELPER_get_utf(const unsigned short *textCharRam, int length, int *offset) {
	unsigned short highPart = GET_NEXT_CHARACTER(textCharRam, length, *offset);
	int ret = 0; // means "error" (see doc)

	if ((highPart >= MIN_HIGH_SURROGATE) && (highPart <= MAX_HIGH_SURROGATE)) {
		if (*offset < (length - 1)) {
			unsigned short lowPart = GET_NEXT_CHARACTER(textCharRam, length, *(offset) + 1);

			if ((lowPart >= MIN_LOW_SURROGATE) && (lowPart <= MAX_LOW_SURROGATE)) {
				*offset += 2;

				ret = 0;
				ret += ((int)highPart - (int)MIN_HIGH_SURROGATE);
				ret <<= (int)10;
				ret += ((int)lowPart - (int)MIN_LOW_SURROGATE);
				ret += (int)MIN_SUPPLEMENTARY_CODE_POINT;
			}
			// else: invalid surrogate pair
		}
		// else: missing second part of surrogate pair
	} else {
		*offset += 1;

		// standard character
		ret = 0x0000FFFF & (int)highPart;
	}

	return ret;
}

#if defined VG_FEATURE_FONT
// See the header file for the function documentation
void VG_HELPER_layout_configure(int faceHandle, const unsigned short *text, int length) {
	face = (FT_Face)faceHandle;
	// For Misra rule 2.7
	(void)text;
	(void)length;

	if (IS_SIMPLE_LAYOUT) {
		// Freetype variables initialisation
		current_text = text;
		current_length = length;
		current_offset = 0;
		previous_glyph_index = 0;
	} else {
#if defined VG_FEATURE_FONT_COMPLEX_LAYOUT
		static hb_font_t *hb_font;
		static jint current_faceHandle = 0;
		// load font in Harfbuzz only when faceHandle changes
		if (faceHandle != current_faceHandle) {
			if (0 != current_faceHandle) {
				hb_font_destroy(hb_font);
			}
			// FT_Set_Pixel_Sizes() must be called before hb_ft_font_create() see issue M0092MEJAUI-2643
			FT_Set_Pixel_Sizes(face, 0, face->units_per_EM);  /* set character size */
			hb_font = hb_ft_font_create(face, NULL);
			current_faceHandle = faceHandle;
		}

		buf = hb_buffer_create();
		hb_buffer_add_utf16(buf, (const uint16_t *)text, length, 0, -1);

		hb_buffer_guess_segment_properties(buf);

		hb_shape(hb_font, buf, NULL, 0);

		glyph_info = hb_buffer_get_glyph_infos(buf, &glyph_count);
		glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);

		current_glyph = 0;
#endif // VG_FEATURE_FONT_COMPLEX_LAYOUT
	}
}

#endif /* if defined VG_FEATURE_FONT */

#if defined VG_FEATURE_FONT
// See the header file for the function documentation
bool VG_HELPER_layout_load_glyph(int *glyph_idx, int *x_advance, int *y_advance, int *x_offset, int *y_offset) {
	// Initiate return value with default values
	*glyph_idx = 0;
	*x_advance = 0;
	*y_advance = 0;
	*x_offset = 0;
	*y_offset = 0;

	bool ret = false;

	if (IS_SIMPLE_LAYOUT) {
		// Freetype layout
		FT_ULong next_char = VG_HELPER_get_utf(current_text, current_length, &current_offset);
		if (0 != next_char) {
			FT_UInt glyph_index = FT_Get_Char_Index(face, next_char);

			int error = FT_Load_Glyph(face, glyph_index, FT_LOAD_NO_SCALE);
			if (FT_ERR(Ok) != error) {
				MEJ_LOG_ERROR_MICROVG("Error while loading glyphid %d: 0x%x, refer to fterrdef.h\n", glyph_index,
				                      error);
			}

			*x_advance = face->glyph->advance.x;

			// Compute Kerning
			if (FT_HAS_KERNING(face) && previous_glyph_index && glyph_index) {
				FT_Vector delta;
				FT_Get_Kerning(face, previous_glyph_index, glyph_index, FT_KERNING_UNSCALED, &delta);

				*x_offset = delta.x;
				*x_advance += delta.x;
			}

			previous_glyph_index = glyph_index;

			*glyph_idx = glyph_index;

			ret = true;
		}
	} else {
#if defined VG_FEATURE_FONT_COMPLEX_LAYOUT
		// Harfbuzz layout
		if (((unsigned int)0) != glyph_count) {
			*glyph_idx = glyph_info[current_glyph].codepoint;
			*x_advance = glyph_pos[current_glyph].x_advance / 64;
			*y_advance = glyph_pos[current_glyph].y_advance / 64;
			*x_offset = glyph_pos[current_glyph].x_offset / 64;
			*y_offset = glyph_pos[current_glyph].y_offset / 64;

			glyph_count--;
			current_glyph++;

			// Load glyph
			int error = FT_Load_Glyph(face, *glyph_idx, FT_LOAD_NO_SCALE);
			if (FT_ERR(Ok) != error) {
				MEJ_LOG_ERROR_MICROVG("Error while loading glyphid %d: 0x%x, refer to fterrdef.h\n", *glyph_idx, error);
			}

			ret = true;
		} else {
			hb_buffer_destroy(buf);
			ret = false;
		}
#endif // VG_FEATURE_FONT_COMPLEX_LAYOUT
	}

	return ret;
}

#endif /* if defined VG_FEATURE_FONT */

// See the header file for the function documentation
const jfloat * VG_HELPER_check_matrix(const jfloat *matrix) {
	return (NULL == matrix) ? g_identity_matrix : matrix;
}

// See the header file for the function documentation
uint32_t VG_HELPER_apply_alpha(uint32_t color, uint32_t alpha) {
	uint32_t color_alpha = (((color >> 24) & (uint32_t)0xff) * alpha) / (uint32_t)255;
	return (color & (uint32_t)0xffffff) | (color_alpha << 24);
}

// See the header file for the function documentation
void VG_HELPER_prepare_matrix(jfloat *dest, jfloat x, jfloat y, const jfloat *matrix) {
	const jfloat *local_matrix = VG_HELPER_check_matrix(matrix);

	if ((0 != x) || (0 != y)) {
		// Create translate matrix for initial x,y translation from graphicscontext.
		LLVG_MATRIX_IMPL_setTranslate(dest, x, y);
		LLVG_MATRIX_IMPL_concatenate(dest, local_matrix);
	} else {
		// use original matrix
		LLVG_MATRIX_IMPL_copy(dest, local_matrix);
	}
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------
