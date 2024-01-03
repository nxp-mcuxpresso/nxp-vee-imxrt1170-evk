/*
 * C
 *
 * Copyright 2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief
 * @brief Implementation of all drawing functions of vg_drawing_stub.h.
 * @author MicroEJ Developer Team
 * @version 3.0.1
 * @see vg_drawing_stub.h
 */

// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------

#include "vg_drawing_stub.h"

// --------------------------------------------------------------------------------
// Private functions
// --------------------------------------------------------------------------------

static inline DRAWING_Status not_implemented(MICROUI_GraphicsContext* gc){
	LLUI_DISPLAY_reportError(gc, DRAWING_LOG_NOT_IMPLEMENTED);
	return DRAWING_DONE;
}

// --------------------------------------------------------------------------------
// vg_drawing_stub.h functions
// --------------------------------------------------------------------------------

// See the header file for the function documentation
DRAWING_Status VG_DRAWING_STUB_drawPath(MICROUI_GraphicsContext* gc, jbyte* path, jint x, jint y, jfloat* matrix, jint fillRule, jint blend, jint color){
	(void)gc;
	(void)path;
	(void)x;
	(void)y;
	(void)matrix;
	(void)fillRule;
	(void)blend;
	(void)color;
	return not_implemented(gc);
}

DRAWING_Status VG_DRAWING_STUB_drawGradient(MICROUI_GraphicsContext* gc, jbyte* path, jint x, jint y, jfloat* matrix, jint fillRule, jint alpha, jint blend, jint* gradient, jfloat* gradientMatrix){
	(void)gc;
	(void)path;
	(void)x;
	(void)y;
	(void)matrix;
	(void)fillRule;
	(void)alpha;
	(void)blend;
	(void)gradient;
	(void)gradientMatrix;
	return not_implemented(gc);
}

DRAWING_Status VG_DRAWING_STUB_drawString(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jfloat x, jfloat y,  jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing){
	(void)gc;
	(void)text;
	(void)faceHandle;
	(void)size;
	(void)x;
	(void)y;
	(void)matrix;
	(void)alpha;
	(void)blend;
	(void)letterSpacing;
	return not_implemented(gc);
}

DRAWING_Status VG_DRAWING_STUB_drawStringGradient(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jfloat x, jfloat y,  jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing, jint *gradientData, jfloat *gradientMatrix){
	(void)gc;
	(void)text;
	(void)faceHandle;
	(void)size;
	(void)x;
	(void)y;
	(void)matrix;
	(void)alpha;
	(void)blend;
	(void)letterSpacing;
	(void)*gradientData;
	(void)*gradientMatrix;
	return not_implemented(gc);
}

DRAWING_Status VG_DRAWING_STUB_drawStringOnCircle(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jint x, jint y, jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing, jfloat radius, jint direction){
	(void)gc;
	(void)text;
	(void)faceHandle;
	(void)size;
	(void)x;
	(void)y;
	(void)matrix;
	(void)alpha;
	(void)blend;
	(void)letterSpacing;
	(void)radius;
	(void)direction;
	return not_implemented(gc);
}

DRAWING_Status VG_DRAWING_STUB_drawStringOnCircleGradient(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jint x, jint y, jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing, jfloat radius, jint direction, jint *gradientData, jfloat *gradientMatrix){
	(void)gc;
	(void)text;
	(void)faceHandle;
	(void)size;
	(void)x;
	(void)y;
	(void)matrix;
	(void)alpha;
	(void)blend;
	(void)letterSpacing;
	(void)radius;
	(void)direction;
	(void)*gradientData;
	(void)*gradientMatrix;
	return not_implemented(gc);
}

DRAWING_Status VG_DRAWING_STUB_drawImage(MICROUI_GraphicsContext* gc, void* image, jfloat *matrix, jint alpha, jlong elapsed, const float color_matrix[], jint* errno){
	(void)gc;
	(void)image;
	(void)*matrix;
	(void)alpha;
	(void)elapsed;
	(void)color_matrix;
	(void)errno;
	return not_implemented(gc);
}

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------
