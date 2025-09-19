/*
 * C
 *
 * Copyright 2020-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief MicroEJ MicroVG library low level API: stub implementation of LLVG_FONT_IMPL_xxx().
 * @author MicroEJ Developer Team
 * @version 7.0.1
 */

#include "vg_configuration.h"

#ifndef VG_FEATURE_FONT

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <LLVG_FONT_impl.h>
#include <LLVG_PAINTER_impl.h>

// -----------------------------------------------------------------------------
// LLVG_FONT_impl.h functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
jint LLVG_FONT_IMPL_load_font(jchar *font_name, jboolean complex_layout) {
	(void)font_name;
	(void)complex_layout;
	return 0;
}

// See the header file for the function documentation
jfloat LLVG_FONT_IMPL_string_width(jchar *text, jint faceHandle, jfloat size, jfloat letterSpacing) {
	(void)text;
	(void)faceHandle;
	(void)size;
	(void)letterSpacing;
	return 0;
}

// See the header file for the function documentation
jfloat LLVG_FONT_IMPL_string_height(jchar *text, jint faceHandle, jfloat size) {
	(void)text;
	(void)faceHandle;
	(void)size;
	return 0;
}

// See the header file for the function documentation
jfloat LLVG_FONT_IMPL_get_baseline_position(jint faceHandle, jfloat size) {
	(void)faceHandle;
	(void)size;
	return 0;
}

// See the header file for the function documentation
jfloat LLVG_FONT_IMPL_get_height(jint faceHandle, jfloat size) {
	(void)faceHandle;
	(void)size;
	return 0;
}

// See the header file for the function documentation
void LLVG_FONT_IMPL_dispose(jint faceHandle) {
	(void)faceHandle;
}

// See the header file for the function documentation
bool LLVG_FONT_IMPL_has_complex_layouter(void) {
	return false;
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#endif // VG_FEATURE_FONT
