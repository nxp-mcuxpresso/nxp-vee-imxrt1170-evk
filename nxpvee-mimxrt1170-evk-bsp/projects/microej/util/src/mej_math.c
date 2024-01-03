/*
 * C
 *
 * Copyright 2019-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief MicroEJ MicroUI library low level API: implementation over VG-Lite
 * @author MicroEJ Developer Team
 * @version 7.1.0
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "mej_math.h"

// -----------------------------------------------------------------------------
// Macros and Defines
// -----------------------------------------------------------------------------

/*
 * @brief Enables the use of power quad (if available).
 *
 * If this feature is enabled, mathematic operations use the power quad.
 * If this feature is disabled, mathematic operations use the compiler C library.
 */
#ifndef VG_FEATURE_POWERQUAD
#define VG_FEATURE_POWERQUAD __has_include("fsl_powerquad.h")
#if defined VG_FEATURE_POWERQUAD && VG_FEATURE_POWERQUAD
#include "fsl_powerquad.h"
#else
#include <math.h>
#endif
#endif

// -----------------------------------------------------------------------------
// mej_math.h functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
float32_t mej_tan_f32(float32_t value) {
#if defined VG_FEATURE_POWERQUAD && VG_FEATURE_POWERQUAD
	float32_t cos;
	float32_t sin;
	PQ_CosF32(&value, &cos);
	PQ_SinF32(&value, &sin);
	PQ_DivF32(&sin, &cos, &value);
	return value;
#else
	return tanf(value);
#endif
}

// See the header file for the function documentation
float32_t mej_sqrt_f32(float32_t value) {
#if defined VG_FEATURE_POWERQUAD && VG_FEATURE_POWERQUAD
	PQ_SqrtF32(&value, &value);
	return value;
#else
	return sqrtf(value);
#endif
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------
