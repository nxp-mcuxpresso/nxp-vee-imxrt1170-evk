/*
 * C
 *
 * Copyright 2022-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief MicroVG library low level API: stubbed implementation of LLVG_PATH_impl.h
 * and LLVG_PATH_PAINTER_impl.h.
 *
 * @author MicroEJ Developer Team
 * @version 7.0.1
 */

#include "vg_configuration.h"

#ifndef VG_FEATURE_PATH

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <LLVG_PATH_impl.h>

// -----------------------------------------------------------------------------
// LLVG_PATH_impl.h functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
jint LLVG_PATH_IMPL_initializePath(jbyte *jpath, jint length) {
	(void)jpath;
	(void)length;
	return LLVG_SUCCESS;
}

// See the header file for the function documentation
jint LLVG_PATH_IMPL_appendPathCommand1(jbyte *jpath, jint length, jint cmd, jfloat x, jfloat y) {
	(void)jpath;
	(void)length;
	(void)cmd;
	(void)x;
	(void)y;
	return LLVG_SUCCESS;
}

// See the header file for the function documentation
jint LLVG_PATH_IMPL_appendPathCommand2(jbyte *jpath, jint length, jint cmd, jfloat x1, jfloat y1, jfloat x2,
                                       jfloat y2) {
	(void)jpath;
	(void)length;
	(void)cmd;
	(void)x1;
	(void)y1;
	(void)x2;
	(void)y2;
	return LLVG_SUCCESS;
}

// See the header file for the function documentation
jint LLVG_PATH_IMPL_appendPathCommand3(jbyte *jpath, jint length, jint cmd, jfloat x1, jfloat y1, jfloat x2, jfloat y2,
                                       jfloat x3, jfloat y3) {
	(void)jpath;
	(void)length;
	(void)cmd;
	(void)x1;
	(void)y1;
	(void)x2;
	(void)y2;
	(void)x3;
	(void)y3;
	return LLVG_SUCCESS;
}

// See the header file for the function documentation
void LLVG_PATH_IMPL_reopenPath(jbyte *jpath) {
	// nothing to do
	(void)jpath;
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#endif // VG_FEATURE_PATH
