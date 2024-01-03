/*
 * C
 *
 * Copyright 2021-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief MicroEJ MicroVG library low level API: basic implementation
 * of matrix APIs.
 * @author MicroEJ Developer Team
 * @version 3.0.1
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <math.h>
#include <string.h>

#include <LLVG_MATRIX_impl.h>

#include "microvg_helper.h"

// -----------------------------------------------------------------------------
// LLVG_MATRIX_impl.h functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
void LLVG_MATRIX_IMPL_identity(jfloat* matrix) {
	LLVG_MATRIX_IMPL_setTranslate(matrix, 0, 0);
}

// See the header file for the function documentation
void LLVG_MATRIX_IMPL_copy(jfloat* dest, jfloat* src) {
	(void)memcpy((void*)dest, (void*)src, sizeof(float) * LLVG_MATRIX_SIZE);
}

// See the header file for the function documentation
void LLVG_MATRIX_IMPL_multiply(jfloat* dest, jfloat* a, jfloat* b) {

	dest[0] = (a[0] * b[0]) + (a[1] * b[3]) + (a[2] * b[6]);
	dest[1] = (a[0] * b[1]) + (a[1] * b[4]) + (a[2] * b[7]);
	dest[2] = (a[0] * b[2]) + (a[1] * b[5]) + (a[2] * b[8]);

	dest[3] = (a[3] * b[0]) + (a[4] * b[3]) + (a[5] * b[6]);
	dest[4] = (a[3] * b[1]) + (a[4] * b[4]) + (a[5] * b[7]);
	dest[5] = (a[3] * b[2]) + (a[4] * b[5]) + (a[5] * b[8]);

	dest[6] = (a[6] * b[0]) + (a[7] * b[3]) + (a[8] * b[6]);
	dest[7] = (a[6] * b[1]) + (a[7] * b[4]) + (a[8] * b[7]);
	dest[8] = (a[6] * b[2]) + (a[7] * b[5]) + (a[8] * b[8]);
}

// See the header file for the function documentation
void LLVG_MATRIX_IMPL_setTranslate(jfloat* matrix, jfloat x, jfloat y) {
	matrix[0] = 1.0f;
	matrix[1] = 0.0f;
	matrix[2] = x;
	matrix[3] = 0.0f;
	matrix[4] = 1.0f;
	matrix[5] = y;
	matrix[6] = 0.0f;
	matrix[7] = 0.0f;
	matrix[8] = 1.0f;
}

// See the header file for the function documentation
void LLVG_MATRIX_IMPL_setScale(jfloat* matrix, jfloat sx, jfloat sy) {
	LLVG_MATRIX_IMPL_identity(matrix);
	LLVG_MATRIX_IMPL_scale(matrix, sx, sy);
}

// See the header file for the function documentation
void LLVG_MATRIX_IMPL_setRotate(jfloat* matrix, jfloat degrees) {
	LLVG_MATRIX_IMPL_identity(matrix);
	LLVG_MATRIX_IMPL_rotate(matrix, degrees);
}

// See the header file for the function documentation
void LLVG_MATRIX_IMPL_setConcat(jfloat* dest, jfloat* a, jfloat* b) {
	if(dest == a) {
		// cppcheck-suppress [misra-c2012-18.8] the size is a define
		float temp[LLVG_MATRIX_SIZE];
		LLVG_MATRIX_IMPL_copy(temp, a);
		LLVG_MATRIX_IMPL_multiply(dest, temp, b);
	}
	else if (dest == b) {
		// cppcheck-suppress [misra-c2012-18.8] the size is a define
		float temp[LLVG_MATRIX_SIZE];
		LLVG_MATRIX_IMPL_copy(temp, b);
		LLVG_MATRIX_IMPL_multiply(dest, a, temp);
	}
	else {
		LLVG_MATRIX_IMPL_multiply(dest, a, b);
	}
}

// See the header file for the function documentation
void LLVG_MATRIX_IMPL_translate(jfloat* matrix, jfloat x, jfloat y) {
	matrix[2] = (matrix[0] * x) + (matrix[1] * y) + matrix[2];
	matrix[5] = (matrix[3] * x) + (matrix[4] * y) + matrix[5];
	matrix[8] = (matrix[6] * x) + (matrix[7] * y) + matrix[8];
}

// See the header file for the function documentation
void LLVG_MATRIX_IMPL_scale(jfloat* matrix, jfloat scaleX, jfloat scaleY) {
	matrix[0] *= scaleX;
	matrix[1] *= scaleY;
	matrix[3] *= scaleX;
	matrix[4] *= scaleY;
	matrix[6] *= scaleX;
	matrix[7] *= scaleY;
}

// See the header file for the function documentation
void LLVG_MATRIX_IMPL_rotate(jfloat* matrix, jfloat angleDegrees) {

	float angleRadians = DEG_TO_RAD(angleDegrees);

	// computes cosine and sine values.
	float cosAngle = cosf(angleRadians);
	float sinAngle = sinf(angleRadians);

	float tmp;

	tmp = (cosAngle * matrix[0]) + (sinAngle * matrix[1]);
	matrix[1] = (cosAngle * matrix[1]) - (sinAngle * matrix[0]);
	matrix[0] = tmp;

	tmp = (cosAngle * matrix[3]) + (sinAngle * matrix[4]);
	matrix[4] = (cosAngle * matrix[4]) - (sinAngle * matrix[3]);
	matrix[3] = tmp;

	tmp = (cosAngle * matrix[6]) + (sinAngle * matrix[7]);
	matrix[7] = (cosAngle * matrix[7]) - (sinAngle * matrix[6]);
	matrix[6] = tmp;
}

// See the header file for the function documentation
void LLVG_MATRIX_IMPL_concatenate(jfloat* matrix, jfloat* other) {
	// cppcheck-suppress [misra-c2012-18.8] the size is a define
	float temp[LLVG_MATRIX_SIZE];
	LLVG_MATRIX_IMPL_copy(temp, matrix);
	LLVG_MATRIX_IMPL_multiply(matrix, temp, other);
}

// See the header file for the function documentation
void LLVG_MATRIX_IMPL_postTranslate(jfloat* matrix, jfloat dx, jfloat dy) {
	// cppcheck-suppress [misra-c2012-18.8] the size is a define
	float a[LLVG_MATRIX_SIZE];
	// cppcheck-suppress [misra-c2012-18.8] the size is a define
	float b[LLVG_MATRIX_SIZE];
	LLVG_MATRIX_IMPL_setTranslate(a, dx, dy);
	LLVG_MATRIX_IMPL_copy(b, matrix);
	LLVG_MATRIX_IMPL_multiply(matrix, a, b);
}

// See the header file for the function documentation
void LLVG_MATRIX_IMPL_postScale(jfloat* matrix, jfloat sx, jfloat sy) {
	// cppcheck-suppress [misra-c2012-18.8] the size is a define
	float a[LLVG_MATRIX_SIZE];
	// cppcheck-suppress [misra-c2012-18.8] the size is a define
	float b[LLVG_MATRIX_SIZE];
	LLVG_MATRIX_IMPL_identity(a);
	LLVG_MATRIX_IMPL_scale(a, sx, sy);
	LLVG_MATRIX_IMPL_copy(b, matrix);
	LLVG_MATRIX_IMPL_multiply(matrix, a, b);
}

// See the header file for the function documentation
void LLVG_MATRIX_IMPL_postRotate(jfloat* matrix, jfloat degrees) {
	// cppcheck-suppress [misra-c2012-18.8] the size is a define
	float a[LLVG_MATRIX_SIZE];
	// cppcheck-suppress [misra-c2012-18.8] the size is a define
	float b[LLVG_MATRIX_SIZE];
	LLVG_MATRIX_IMPL_identity(a);
	LLVG_MATRIX_IMPL_rotate(a, degrees);
	LLVG_MATRIX_IMPL_copy(b, matrix);
	LLVG_MATRIX_IMPL_multiply(matrix, a, b);
}

// See the header file for the function documentation
void LLVG_MATRIX_IMPL_postConcat(jfloat* matrix, jfloat* other) {
	// cppcheck-suppress [misra-c2012-18.8] the size is a define
	float a[LLVG_MATRIX_SIZE];
	LLVG_MATRIX_IMPL_copy(a, matrix);
	LLVG_MATRIX_IMPL_multiply(matrix, other, a);
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------
