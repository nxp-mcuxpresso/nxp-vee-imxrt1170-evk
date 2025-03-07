/*
 * C
 *
 * Copyright 2019-2024 MicroEJ Corp. This file has been modified and/or created by MicroEJ Corp.
 *
 * This file is part of the FreeType project, and may only be used,
 * modified, and distributed under the terms of the FreeType project
 * license, LICENSE.TXT.  By continuing to use, modify, or distribute
 * this file you indicate that you have read the license and
 * understand and accept it fully.
 */

/**
 * @file
 * @brief Freetype renderer
 * @author MicroEJ Developer Team
 * @version 3.0.0
 */

#ifndef FTVECTOR_H
#define FTVECTOR_H

#include "vg_configuration.h"

#if defined VG_FEATURE_FONT && (VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR)

// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------

#include "vg_freetype.h"

// --------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------

#define FT_PARAM_TAG_DRAWER     FT_MAKE_TAG( 'd', 'r', 'a', 'w' )

// --------------------------------------------------------------------------------
// Typedef
// --------------------------------------------------------------------------------

/*
 * @brief The data of the function VG_FREETYPE_draw_glyph_t
 */
typedef struct {
	VG_FREETYPE_draw_glyph_t drawer;
	jfloat* matrix;
	uint32_t color;
	void* user_data;
} FTVECTOR_draw_glyph_data_t;

#endif // defined VG_FEATURE_FONT && (VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR)

#endif // FTVECTOR_H
