/* 
 * Copyright 2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#if !defined VG_DRAWING_BVI_H
#define VG_DRAWING_BVI_H
#ifdef __cplusplus
extern "C" {
#endif

/*
 * @file
 * @brief Implementation of a set of vg_drawing.h drawing functions (library MicroVG).
 * These are implementations over a BufferedVectorImage and the destination buffer format
 * is the BufferedVectorImage format. When the drawing cannot be performed for any reason,
 * the stub implementation is used insted.
 * @author MicroEJ Developer Team
 * @version 3.0.1
 * @see ui_drawing_bvi.h
 */

#include "microvg_configuration.h"
#if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE

// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------

#include <LLVG_BVI_impl.h>

#include "vg_drawing.h"
#include "ui_drawing.h"

// --------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------

/*
 * @brief Redirects all vg_drawing.h functions to the BufferedVectorImage functions
 */
#define VG_DRAWING_BVI_drawPath CONCAT(VG_DRAWING_drawPath_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define VG_DRAWING_BVI_drawGradient CONCAT(VG_DRAWING_drawGradient_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define VG_DRAWING_BVI_drawString CONCAT(VG_DRAWING_drawString_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define VG_DRAWING_BVI_drawStringGradient CONCAT(VG_DRAWING_drawStringGradient_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define VG_DRAWING_BVI_drawStringOnCircle CONCAT(VG_DRAWING_drawStringOnCircle_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define VG_DRAWING_BVI_drawStringOnCircleGradient CONCAT(VG_DRAWING_drawStringOnCircleGradient_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define VG_DRAWING_BVI_drawImage CONCAT(VG_DRAWING_drawImage_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)

// --------------------------------------------------------------------------------
// vg_drawing.h API
// (the function names differ according to the available number of destination formats)
// --------------------------------------------------------------------------------

/*
 * @brief Implementation of drawPath over a BufferedVectorImage. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_BVI_drawPath(MICROUI_GraphicsContext* gc, jbyte* path, jint x, jint y, jfloat* matrix, jint fillRule, jint blend, jint color);

/*
 * @brief Implementation of drawGradient over a BufferedVectorImage. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_BVI_drawGradient(MICROUI_GraphicsContext* gc, jbyte* path, jint x, jint y, jfloat* matrix, jint fillRule, jint alpha, jint blend, jint* gradient, jfloat* gradientMatrix);

/*
 * @brief Implementation of drawString over a BufferedVectorImage. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_BVI_drawString(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jfloat x, jfloat y,  jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing);

/*
 * @brief Implementation of drawStringGradient over a BufferedVectorImage. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_BVI_drawStringGradient(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jfloat x, jfloat y,  jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing, jint *gradientData, jfloat *gradientMatrix);

/*
 * @brief Implementation of drawStringOnCircle over a BufferedVectorImage. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_BVI_drawStringOnCircle(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jint x, jint y, jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing, jfloat radius, jint direction);

/*
 * @brief Implementation of drawStringOnCircleGradient over a BufferedVectorImage. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_BVI_drawStringOnCircleGradient(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jint x, jint y, jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing, jfloat radius, jint direction, jint *gradientData, jfloat *gradientMatrix);

/*
 * @brief Implementation of drawImage over a BufferedVectorImage. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_BVI_drawImage(MICROUI_GraphicsContext* gc, void* image, jfloat *matrix, jint alpha, jlong elapsed, const float color_matrix[], jint* errno);

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------

#endif // #if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE

#ifdef __cplusplus
}
#endif
#endif // VG_DRAWING_BVI_H
