/*
 * Copyright 2023-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#if !defined VG_DRAWING_STUB_H
#define VG_DRAWING_STUB_H
#ifdef __cplusplus
extern "C" {
#endif

/*
 * @file
 * @brief Implementation of all drawing functions of vg_drawing.h (library MicroVG).
 * These are stubbed implementations, there is no error but the drawings are not
 * performed. Useful to stub a drawing on a custom destination (custom GraphicsContext
 * format not supported by the Graphics Engine).
 * @author MicroEJ Developer Team
 * @version 7.0.1
 * @see ui_drawing_stub.h
 */

// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------

#include "vg_drawing.h"

// --------------------------------------------------------------------------------
// Public API
// --------------------------------------------------------------------------------

/*
 * @brief Stubbed implementation of drawPath. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_STUB_drawPath(MICROUI_GraphicsContext *gc, jbyte *path, jfloat *matrix, jint fillRule,
                                        jint blend, jint color);

/*
 * @brief Stubbed implementation of drawGradient. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_STUB_drawGradient(MICROUI_GraphicsContext *gc, jbyte *path, jfloat *matrix, jint fillRule,
                                            jint alpha, jint blend, jint *gradient, jfloat *gradientMatrix);

/*
 * @brief Stubbed implementation of drawString. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_STUB_drawString(MICROUI_GraphicsContext *gc, jchar *text, jint length, jint faceHandle,
                                          jfloat size, jfloat *matrix, jint alpha, jint blend, jfloat letterSpacing);

/*
 * @brief Stubbed implementation of drawStringGradient. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_STUB_drawStringGradient(MICROUI_GraphicsContext *gc, jchar *text, jint length,
                                                  jint faceHandle, jfloat size, jfloat *matrix, jint alpha, jint blend,
                                                  jfloat letterSpacing, jint *gradientData, jfloat *gradientMatrix);

/*
 * @brief Stubbed implementation of drawStringOnCircle. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_STUB_drawStringOnCircle(MICROUI_GraphicsContext *gc, jchar *text, jint length,
                                                  jint faceHandle, jfloat size, jfloat *matrix, jint alpha, jint blend,
                                                  jfloat letterSpacing, jfloat radius, jint direction);

/*
 * @brief Stubbed implementation of drawStringOnCircleGradient. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_STUB_drawStringOnCircleGradient(MICROUI_GraphicsContext *gc, jchar *text, jint length,
                                                          jint faceHandle, jfloat size, jfloat *matrix, jint alpha,
                                                          jint blend, jfloat letterSpacing, jfloat radius,
                                                          jint direction, jint *gradientData, jfloat *gradientMatrix);

/*
 * @brief Stubbed implementation of drawImage. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_STUB_drawImage(MICROUI_GraphicsContext *gc, void *image, jfloat *matrix, jint alpha,
                                         jlong elapsed, const float color_matrix[], jint *errno);

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
#endif // VG_DRAWING_STUB_H
