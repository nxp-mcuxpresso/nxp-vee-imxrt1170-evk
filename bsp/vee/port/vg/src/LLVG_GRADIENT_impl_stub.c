/*
 * C
 *
 * Copyright 2022-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief MicroVG library low level API: stub of the linear gradient implementation.
 *
 * This implementation is only compatible with the configuration VG_FEATURE_GRADIENT_FIRST_COLOR.
 * It only stores the very first gradient's color as current color.
 *
 * @author MicroEJ Developer Team
 * @version 6.0.1
 */

#include "vg_configuration.h"

#if defined(VG_FEATURE_GRADIENT) && defined(VG_FEATURE_GRADIENT_FIRST_COLOR) && \
	(VG_FEATURE_GRADIENT == VG_FEATURE_GRADIENT_FIRST_COLOR)

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <LLVG_GRADIENT_impl.h>

#include "bsp_util.h"

// -----------------------------------------------------------------------------
// LLVG_GRADIENT_impl.h functions [optional]: weak functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT jint LLVG_GRADIENT_IMPL_initializeGradient(jint *gradient, jint length, const jint *colors,
                                                                 jint count,
                                                                 jfloat *positions, jfloat xStart, jfloat yStart,
                                                                 jfloat xEnd, jfloat yEnd) {
	(void)count;
	(void)positions;
	(void)xStart;
	(void)yStart;
	(void)xEnd;
	(void)yEnd;

	jint ret;

	if (length < 1) {
		// the given array is too small
		ret = 1; // return the expected minimal size
	} else {
		// just store the first color
		gradient[0] = colors[0];
		ret = LLVG_SUCCESS;
	}

	return ret;
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#endif // VG_FEATURE_GRADIENT
