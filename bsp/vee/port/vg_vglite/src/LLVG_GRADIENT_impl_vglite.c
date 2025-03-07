/*
 * C
 *
 * Copyright 2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief MicroVG library low level API over VGLite.
 * @author MicroEJ Developer Team
 * @version 8.0.1
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <LLVG_GRADIENT_impl.h>

#include "vg_configuration.h"
#include "mej_math.h"
#include "vg_helper.h"
#include "vg_vglite_helper.h"

// -----------------------------------------------------------------------------
// Configuration check
// -----------------------------------------------------------------------------

#if defined(VG_FEATURE_GRADIENT) && defined(VG_FEATURE_GRADIENT_FULL) && \
	(VG_FEATURE_GRADIENT == VG_FEATURE_GRADIENT_FULL)

// -----------------------------------------------------------------------------
// Defines
// -----------------------------------------------------------------------------

#define MICROVG_GRADIENT_HEADER_SIZE (sizeof(MICROVG_GRADIENT_HEADER_t) / sizeof(uint32_t))

// -----------------------------------------------------------------------------
// LLVG_GRADIENT_impl.h functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
jint LLVG_GRADIENT_IMPL_initializeGradient(jint *jgradient, jint length, const jint *colors, jint count,
                                           const jfloat *positions, jfloat xStart, jfloat yStart, jfloat xEnd,
                                           jfloat yEnd) {
	MICROVG_GRADIENT_HEADER_t *gradient = (MICROVG_GRADIENT_HEADER_t *)jgradient;
	uint32_t expectedLength = MICROVG_GRADIENT_HEADER_SIZE + (count * 2 /* colors and positions */);
	jint ret;
	if (length >= expectedLength) {
		// fill header
		float angle = RAD_TO_DEG(atan2f(yEnd - yStart, xEnd - xStart));
		float l = sqrtf(powf(xEnd - xStart, 2) + powf(yEnd - yStart, 2));

		gradient->x = xStart;
		gradient->y = yStart;
		gradient->angle = angle;

		gradient->count = (uint32_t)count;
		gradient->length = l;

		gradient->colors_offset = MICROVG_GRADIENT_HEADER_SIZE;
		gradient->positions_offset = MICROVG_GRADIENT_HEADER_SIZE + count;

		// fill colors
		void *colors_addr = (void *)&((uint32_t *)gradient)[gradient->colors_offset];
		(void)memcpy(colors_addr, colors, count * sizeof(uint32_t));

		// fill positions
		uint32_t *positions_addr = &((uint32_t *)gradient)[gradient->positions_offset];
		for (uint32_t p = 0; p < gradient->count; p++) {
			positions_addr[p] = (uint32_t)(positions[p] * (VGLITE_GRADIENT_SIZE - 1));
		}

		ret = LLVG_SUCCESS;
	} else {
		// the given array is too small
		ret = expectedLength;
	}
	return ret;
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#endif /* if defined(VG_FEATURE_GRADIENT) && defined(VG_FEATURE_GRADIENT_FULL) && (VG_FEATURE_GRADIENT ==
        * VG_FEATURE_GRADIENT_FULL) */
