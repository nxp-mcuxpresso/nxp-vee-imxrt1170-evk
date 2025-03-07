/*
 * C
 *
 * Copyright 2023-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief Redirection of all MicroVG library drawing functions. This file is required
 * (must be compiled in the C project) but should not be modified (except when the VEE
 * port uses more than 3 destination formats, see below).
 *
 * This file follows strictly the same concept as ui_drawing.c but for MicroVG
 * library.
 *
 * @author MicroEJ Developer Team
 * @version 6.0.1
 * @see ui_drawing.c
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "vg_drawing.h"
#include "vg_drawing_stub.h"
#include "bsp_util.h"

// --------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------

#if !defined(UI_GC_SUPPORTED_FORMATS) || (UI_GC_SUPPORTED_FORMATS <= 1)

/*
 * The function VG_DRAWING_DEFAULT_xxx() are directly called by LLVG_PAINTER_impl.c
 * Another file can override each weak function independently to use a GPU.
 */

#define VG_DRAWING_DEFAULT_drawPath VG_DRAWING_drawPath
#define VG_DRAWING_DEFAULT_drawGradient VG_DRAWING_drawGradient
#define VG_DRAWING_DEFAULT_drawString VG_DRAWING_drawString
#define VG_DRAWING_DEFAULT_drawStringGradient VG_DRAWING_drawStringGradient
#define VG_DRAWING_DEFAULT_drawStringOnCircle VG_DRAWING_drawStringOnCircle
#define VG_DRAWING_DEFAULT_drawStringOnCircleGradient VG_DRAWING_drawStringOnCircleGradient
#define VG_DRAWING_DEFAULT_drawImage VG_DRAWING_drawImage

#else // !defined(UI_GC_SUPPORTED_FORMATS) || (UI_GC_SUPPORTED_FORMATS <= 1)

/*
 * The functions VG_DRAWING_DEFAULT_xxx() are indirectly called through some tables.
 * The functions have got the identifier "0" as suffix. Another file can override
 * each weak function independently to use a GPU.
 */

#define VG_DRAWING_DEFAULT_drawPath VG_DRAWING_drawPath_0
#define VG_DRAWING_DEFAULT_drawGradient VG_DRAWING_drawGradient_0
#define VG_DRAWING_DEFAULT_drawString VG_DRAWING_drawString_0
#define VG_DRAWING_DEFAULT_drawStringGradient VG_DRAWING_drawStringGradient_0
#define VG_DRAWING_DEFAULT_drawStringOnCircle VG_DRAWING_drawStringOnCircle_0
#define VG_DRAWING_DEFAULT_drawStringOnCircleGradient VG_DRAWING_drawStringOnCircleGradient_0
#define VG_DRAWING_DEFAULT_drawImage VG_DRAWING_drawImage_0

#endif // !defined(UI_GC_SUPPORTED_FORMATS) || (UI_GC_SUPPORTED_FORMATS <= 1)

// --------------------------------------------------------------------------------
// Extern functions
// --------------------------------------------------------------------------------

#if defined(UI_GC_SUPPORTED_FORMATS) && (UI_GC_SUPPORTED_FORMATS > 1)

/*
 * @brief Set of drawing functions according to the index of the destination format in
 * the drawing tables ("0", "1" or "2").
 *
 * These functions must be declared in other H files.
 */

#if (UI_GC_SUPPORTED_FORMATS > 3)
#error "Increase the number of following functions and update the next tables"
#endif

extern DRAWING_Status VG_DRAWING_drawPath_0(MICROUI_GraphicsContext *gc, jbyte *path, jfloat *matrix, jint fillRule,
                                            jint blend, jint color);
extern DRAWING_Status VG_DRAWING_drawGradient_0(MICROUI_GraphicsContext *gc, jbyte *path, jfloat *matrix, jint fillRule,
                                                jint alpha, jint blend, jint *gradient, jfloat *gradientMatrix);
extern DRAWING_Status VG_DRAWING_drawString_0(MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle, jfloat size,
                                              jfloat *matrix, jint alpha, jint blend, jfloat letterSpacing);
extern DRAWING_Status VG_DRAWING_drawStringGradient_0(MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle,
                                                      jfloat size, jfloat *matrix, jint alpha, jint blend,
                                                      jfloat letterSpacing, jint *gradientData, jfloat *gradientMatrix);
extern DRAWING_Status VG_DRAWING_drawStringOnCircle_0(MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle,
                                                      jfloat size, jfloat *matrix, jint alpha, jint blend,
                                                      jfloat letterSpacing, jfloat radius, jint direction);
extern DRAWING_Status VG_DRAWING_drawStringOnCircleGradient_0(MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle,
                                                              jfloat size, jfloat *matrix, jint alpha, jint blend,
                                                              jfloat letterSpacing, jfloat radius, jint direction,
                                                              jint *gradientData, jfloat *gradientMatrix);
extern DRAWING_Status VG_DRAWING_drawImage_0(MICROUI_GraphicsContext *gc, void *image, jfloat *matrix, jint alpha,
                                             jlong elapsed, const float color_matrix[], jint *errno);

extern DRAWING_Status VG_DRAWING_drawPath_1(MICROUI_GraphicsContext *gc, jbyte *path, jfloat *matrix, jint fillRule,
                                            jint blend, jint color);
extern DRAWING_Status VG_DRAWING_drawGradient_1(MICROUI_GraphicsContext *gc, jbyte *path, jfloat *matrix, jint fillRule,
                                                jint alpha, jint blend, jint *gradient, jfloat *gradientMatrix);
extern DRAWING_Status VG_DRAWING_drawString_1(MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle, jfloat size,
                                              jfloat *matrix, jint alpha, jint blend, jfloat letterSpacing);
extern DRAWING_Status VG_DRAWING_drawStringGradient_1(MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle,
                                                      jfloat size, jfloat *matrix, jint alpha, jint blend,
                                                      jfloat letterSpacing, jint *gradientData, jfloat *gradientMatrix);
extern DRAWING_Status VG_DRAWING_drawStringOnCircle_1(MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle,
                                                      jfloat size, jfloat *matrix, jint alpha, jint blend,
                                                      jfloat letterSpacing, jfloat radius, jint direction);
extern DRAWING_Status VG_DRAWING_drawStringOnCircleGradient_1(MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle,
                                                              jfloat size, jfloat *matrix, jint alpha, jint blend,
                                                              jfloat letterSpacing, jfloat radius, jint direction,
                                                              jint *gradientData, jfloat *gradientMatrix);
extern DRAWING_Status VG_DRAWING_drawImage_1(MICROUI_GraphicsContext *gc, void *image, jfloat *matrix, jint alpha,
                                             jlong elapsed, const float color_matrix[], jint *errno);

#if (UI_GC_SUPPORTED_FORMATS > 2)
extern DRAWING_Status VG_DRAWING_drawPath_2(MICROUI_GraphicsContext *gc, jbyte *path, jfloat *matrix, jint fillRule,
                                            jint blend, jint color);
extern DRAWING_Status VG_DRAWING_drawGradient_2(MICROUI_GraphicsContext *gc, jbyte *path, jfloat *matrix, jint fillRule,
                                                jint alpha, jint blend, jint *gradient, jfloat *gradientMatrix);
extern DRAWING_Status VG_DRAWING_drawString_2(MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle, jfloat size,
                                              jfloat *matrix, jint alpha, jint blend, jfloat letterSpacing);
extern DRAWING_Status VG_DRAWING_drawStringGradient_2(MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle,
                                                      jfloat size, jfloat *matrix, jint alpha, jint blend,
                                                      jfloat letterSpacing, jint *gradientData, jfloat *gradientMatrix);
extern DRAWING_Status VG_DRAWING_drawStringOnCircle_2(MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle,
                                                      jfloat size, jfloat *matrix, jint alpha, jint blend,
                                                      jfloat letterSpacing, jfloat radius, jint direction);
extern DRAWING_Status VG_DRAWING_drawStringOnCircleGradient_2(MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle,
                                                              jfloat size, jfloat *matrix, jint alpha, jint blend,
                                                              jfloat letterSpacing, jfloat radius, jint direction,
                                                              jint *gradientData, jfloat *gradientMatrix);
extern DRAWING_Status VG_DRAWING_drawImage_2(MICROUI_GraphicsContext *gc, void *image, jfloat *matrix, jint alpha,
                                             jlong elapsed, const float color_matrix[], jint *errno);
#endif // (UI_GC_SUPPORTED_FORMATS > 2)

#endif // defined(UI_GC_SUPPORTED_FORMATS) && (UI_GC_SUPPORTED_FORMATS > 1)

// --------------------------------------------------------------------------------
// Typedef of drawing functions
// --------------------------------------------------------------------------------

#if defined(UI_GC_SUPPORTED_FORMATS) && (UI_GC_SUPPORTED_FORMATS > 1)

/*
 * @brief Typedef used by next tables. See the functions comments in vg_drawing.h
 */

typedef DRAWING_Status (* VG_DRAWING_drawPath_t) (MICROUI_GraphicsContext *gc, jbyte *path, jfloat *matrix,
                                                  jint fillRule, jint blend, jint color);
typedef DRAWING_Status (* VG_DRAWING_drawGradient_t) (MICROUI_GraphicsContext *gc, jbyte *path, jfloat *matrix,
                                                      jint fillRule, jint alpha, jint blend, jint *gradient,
                                                      jfloat *gradientMatrix);
typedef DRAWING_Status (* VG_DRAWING_drawString_t) (MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle,
                                                    jfloat size, jfloat *matrix, jint alpha, jint blend,
                                                    jfloat letterSpacing);
typedef DRAWING_Status (* VG_DRAWING_drawStringGradient_t) (MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle,
                                                            jfloat size, jfloat *matrix, jint alpha, jint blend,
                                                            jfloat letterSpacing, jint *gradientData,
                                                            jfloat *gradientMatrix);
typedef DRAWING_Status (* VG_DRAWING_drawStringOnCircle_t) (MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle,
                                                            jfloat size, jfloat *matrix, jint alpha, jint blend,
                                                            jfloat letterSpacing, jfloat radius, jint direction);
typedef DRAWING_Status (* VG_DRAWING_drawStringOnCircleGradient_t) (MICROUI_GraphicsContext *gc, jchar *text,
                                                                    jint faceHandle, jfloat size, jfloat *matrix,
                                                                    jint alpha, jint blend, jfloat letterSpacing,
                                                                    jfloat radius, jint direction, jint *gradientData,
                                                                    jfloat *gradientMatrix);
typedef DRAWING_Status (* VG_DRAWING_drawImage_t) (MICROUI_GraphicsContext *gc, void *image, jfloat *matrix, jint alpha,
                                                   jlong elapsed, const float color_matrix[], jint *errno);

#endif // #if defined(UI_GC_SUPPORTED_FORMATS) && (UI_GC_SUPPORTED_FORMATS > 1)

// --------------------------------------------------------------------------------
// Tables according to the destination format.
// --------------------------------------------------------------------------------

#if defined(UI_GC_SUPPORTED_FORMATS) && (UI_GC_SUPPORTED_FORMATS > 1)

static const VG_DRAWING_drawPath_t VG_DRAWER_drawPath[] = {
	&VG_DRAWING_drawPath_0,
	&VG_DRAWING_drawPath_1,
#if (UI_GC_SUPPORTED_FORMATS > 2)
	&VG_DRAWING_drawPath_2,
#endif
};

static const VG_DRAWING_drawGradient_t VG_DRAWER_drawGradient[] = {
	&VG_DRAWING_drawGradient_0,
	&VG_DRAWING_drawGradient_1,
#if (UI_GC_SUPPORTED_FORMATS > 2)
	&VG_DRAWING_drawGradient_2,
#endif
};

static const VG_DRAWING_drawString_t VG_DRAWER_drawString[] = {
	&VG_DRAWING_drawString_0,
	&VG_DRAWING_drawString_1,
#if (UI_GC_SUPPORTED_FORMATS > 2)
	&VG_DRAWING_drawString_2,
#endif
};

static const VG_DRAWING_drawStringGradient_t VG_DRAWER_drawStringGradient[] = {
	&VG_DRAWING_drawStringGradient_0,
	&VG_DRAWING_drawStringGradient_1,
#if (UI_GC_SUPPORTED_FORMATS > 2)
	&VG_DRAWING_drawStringGradient_2,
#endif
};

static const VG_DRAWING_drawStringOnCircle_t VG_DRAWER_drawStringOnCircle[] = {
	&VG_DRAWING_drawStringOnCircle_0,
	&VG_DRAWING_drawStringOnCircle_1,
#if (UI_GC_SUPPORTED_FORMATS > 2)
	&VG_DRAWING_drawStringOnCircle_2,
#endif
};

static const VG_DRAWING_drawStringOnCircleGradient_t VG_DRAWER_drawStringOnCircleGradient[] = {
	&VG_DRAWING_drawStringOnCircleGradient_0,
	&VG_DRAWING_drawStringOnCircleGradient_1,
#if (UI_GC_SUPPORTED_FORMATS > 2)
	&VG_DRAWING_drawStringOnCircleGradient_2,
#endif
};

static const VG_DRAWING_drawImage_t VG_DRAWER_drawImage[] = {
	&VG_DRAWING_drawImage_0,
	&VG_DRAWING_drawImage_1,
#if (UI_GC_SUPPORTED_FORMATS > 2)
	&VG_DRAWING_drawImage_2,
#endif
};

#endif /* if defined(UI_GC_SUPPORTED_FORMATS) && (UI_GC_SUPPORTED_FORMATS > 1) */

// --------------------------------------------------------------------------------
// vg_drawing.h functions
// --------------------------------------------------------------------------------

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT void VG_DRAWING_initialize() {
	// Nothing to do
}

// --------------------------------------------------------------------------------
// vg_drawing.h functions
// (the function names differ according to the available number of destination formats)
// --------------------------------------------------------------------------------

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_DEFAULT_drawPath(MICROUI_GraphicsContext *gc, jbyte *path,
                                                                 jfloat *matrix, jint fillRule, jint blend,
                                                                 jint color) {
	return VG_DRAWING_STUB_drawPath(gc, path, matrix, fillRule, blend, color);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_DEFAULT_drawGradient(MICROUI_GraphicsContext *gc, jbyte *path,
                                                                     jfloat *matrix, jint fillRule, jint alpha,
                                                                     jint blend, jint *gradient,
                                                                     jfloat *gradientMatrix) {
	return VG_DRAWING_STUB_drawGradient(gc, path, matrix, fillRule, alpha, blend, gradient, gradientMatrix);
}

BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_DEFAULT_drawString(MICROUI_GraphicsContext *gc, jchar *text,
                                                                   jint faceHandle, jfloat size, jfloat *matrix,
                                                                   jint alpha, jint blend, jfloat letterSpacing) {
	return VG_DRAWING_STUB_drawString(gc, text, faceHandle, size, matrix, alpha, blend, letterSpacing);
}

BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_DEFAULT_drawStringGradient(MICROUI_GraphicsContext *gc, jchar *text,
                                                                           jint faceHandle, jfloat size, jfloat *matrix,
                                                                           jint alpha, jint blend, jfloat letterSpacing,
                                                                           jint *gradientData, jfloat *gradientMatrix) {
	return VG_DRAWING_STUB_drawStringGradient(gc, text, faceHandle, size, matrix, alpha, blend, letterSpacing,
	                                          gradientData, gradientMatrix);
}

BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_DEFAULT_drawStringOnCircle(MICROUI_GraphicsContext *gc, jchar *text,
                                                                           jint faceHandle, jfloat size, jfloat *matrix,
                                                                           jint alpha, jint blend, jfloat letterSpacing,
                                                                           jfloat radius, jint direction) {
	return VG_DRAWING_STUB_drawStringOnCircle(gc, text, faceHandle, size, matrix, alpha, blend, letterSpacing, radius,
	                                          direction);
}

BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_DEFAULT_drawStringOnCircleGradient(MICROUI_GraphicsContext *gc,
                                                                                   jchar *text, jint faceHandle,
                                                                                   jfloat size, jfloat *matrix,
                                                                                   jint alpha, jint blend,
                                                                                   jfloat letterSpacing, jfloat radius,
                                                                                   jint direction, jint *gradientData,
                                                                                   jfloat *gradientMatrix) {
	return VG_DRAWING_STUB_drawStringOnCircleGradient(gc, text, faceHandle, size, matrix, alpha, blend, letterSpacing,
	                                                  radius, direction, gradientData, gradientMatrix);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_DEFAULT_drawImage(MICROUI_GraphicsContext *gc, void *sni_context,
                                                                  jfloat *matrix, jint alpha, jlong elapsed,
                                                                  const float color_matrix[], jint *errno) {
	return VG_DRAWING_STUB_drawImage(gc, sni_context, matrix, alpha, elapsed, color_matrix, errno);
}

#if defined(UI_GC_SUPPORTED_FORMATS) && (UI_GC_SUPPORTED_FORMATS > 1)

/*
 * The VEE port supports several destination formats. All drawing functions use a
 * dedicated table to redirect to the right implementation.
 *
 * The "DEFAULT" functions (see upper) are used as element "0" of the tables (== display
 * buffer format).
 */

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_drawPath(MICROUI_GraphicsContext *gc, jbyte *path, jfloat *matrix,
                                                         jint fillRule, jint blend, jint color) {
	return (*VG_DRAWER_drawPath[gc->drawer])(gc, path, matrix, fillRule, blend, color);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_drawGradient(MICROUI_GraphicsContext *gc, jbyte *path, jfloat *matrix,
                                                             jint fillRule, jint alpha, jint blend, jint *gradient,
                                                             jfloat *gradientMatrix) {
	return (*VG_DRAWER_drawGradient[gc->drawer])(gc, path, matrix, fillRule, alpha, blend, gradient, gradientMatrix);
}

BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_drawString(MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle,
                                                           jfloat size, jfloat *matrix, jint alpha, jint blend,
                                                           jfloat letterSpacing) {
	return (*VG_DRAWER_drawString[gc->drawer])(gc, text, faceHandle, size, matrix, alpha, blend, letterSpacing);
}

BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_drawStringGradient(MICROUI_GraphicsContext *gc, jchar *text,
                                                                   jint faceHandle, jfloat size, jfloat *matrix,
                                                                   jint alpha, jint blend, jfloat letterSpacing,
                                                                   jint *gradientData, jfloat *gradientMatrix) {
	return (*VG_DRAWER_drawStringGradient[gc->drawer])(gc, text, faceHandle, size, matrix, alpha, blend, letterSpacing,
	                                                   gradientData, gradientMatrix);
}

BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_drawStringOnCircle(MICROUI_GraphicsContext *gc, jchar *text,
                                                                   jint faceHandle, jfloat size, jfloat *matrix,
                                                                   jint alpha, jint blend, jfloat letterSpacing,
                                                                   jfloat radius, jint direction) {
	return (*VG_DRAWER_drawStringOnCircle[gc->drawer])(gc, text, faceHandle, size, matrix, alpha, blend, letterSpacing,
	                                                   radius, direction);
}

BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_drawStringOnCircleGradient(MICROUI_GraphicsContext *gc, jchar *text,
                                                                           jint faceHandle, jfloat size, jfloat *matrix,
                                                                           jint alpha, jint blend, jfloat letterSpacing,
                                                                           jfloat radius, jint direction,
                                                                           jint *gradientData, jfloat *gradientMatrix) {
	return (*VG_DRAWER_drawStringOnCircleGradient[gc->drawer])(gc, text, faceHandle, size, matrix, alpha, blend,
	                                                           letterSpacing, radius, direction, gradientData,
	                                                           gradientMatrix);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_drawImage(MICROUI_GraphicsContext *gc, void *image, jfloat *matrix,
                                                          jint alpha, jlong elapsed, const float color_matrix[],
                                                          jint *errno) {
	return (*VG_DRAWER_drawImage[gc->drawer])(gc, image, matrix, alpha, elapsed, color_matrix, errno);
}

/*
 * The next functions are used as elements "1" of the tables. They call STUB functions and should be
 * overridden by the drawer that manages the format "1".
 */

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_drawPath_1(MICROUI_GraphicsContext *gc, jbyte *path, jfloat *matrix,
                                                           jint fillRule, jint blend, jint color) {
	return VG_DRAWING_STUB_drawPath(gc, path, matrix, fillRule, blend, color);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_drawGradient_1(MICROUI_GraphicsContext *gc, jbyte *path, jfloat *matrix,
                                                               jint fillRule, jint alpha, jint blend, jint *gradient,
                                                               jfloat *gradientMatrix) {
	return VG_DRAWING_STUB_drawGradient(gc, path, matrix, fillRule, alpha, blend, gradient, gradientMatrix);
}

BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_drawString_1(MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle,
                                                             jfloat size, jfloat *matrix, jint alpha, jint blend,
                                                             jfloat letterSpacing) {
	return VG_DRAWING_STUB_drawString(gc, text, faceHandle, size, matrix, alpha, blend, letterSpacing);
}

BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_drawStringGradient_1(MICROUI_GraphicsContext *gc, jchar *text,
                                                                     jint faceHandle, jfloat size, jfloat *matrix,
                                                                     jint alpha, jint blend, jfloat letterSpacing,
                                                                     jint *gradientData, jfloat *gradientMatrix) {
	return VG_DRAWING_STUB_drawStringGradient(gc, text, faceHandle, size, matrix, alpha, blend, letterSpacing,
	                                          gradientData, gradientMatrix);
}

BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_drawStringOnCircle_1(MICROUI_GraphicsContext *gc, jchar *text,
                                                                     jint faceHandle, jfloat size, jfloat *matrix,
                                                                     jint alpha, jint blend, jfloat letterSpacing,
                                                                     jfloat radius, jint direction) {
	return VG_DRAWING_STUB_drawStringOnCircle(gc, text, faceHandle, size, matrix, alpha, blend, letterSpacing, radius,
	                                          direction);
}

BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_drawStringOnCircleGradient_1(MICROUI_GraphicsContext *gc, jchar *text,
                                                                             jint faceHandle, jfloat size,
                                                                             jfloat *matrix, jint alpha, jint blend,
                                                                             jfloat letterSpacing, jfloat radius,
                                                                             jint direction, jint *gradientData,
                                                                             jfloat *gradientMatrix) {
	return VG_DRAWING_STUB_drawStringOnCircleGradient(gc, text, faceHandle, size, matrix, alpha, blend, letterSpacing,
	                                                  radius, direction, gradientData, gradientMatrix);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_drawImage_1(MICROUI_GraphicsContext *gc, void *sni_context,
                                                            jfloat *matrix, jint alpha, jlong elapsed,
                                                            const float color_matrix[], jint *errno) {
	return VG_DRAWING_STUB_drawImage(gc, sni_context, matrix, alpha, elapsed, color_matrix, errno);
}

#if (UI_GC_SUPPORTED_FORMATS > 2)

/*
 * The next functions are used as elements "2" of the tables. They call STUB functions and should be
 * overridden by the drawer that manages the format "2".
 */

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_drawPath_2(MICROUI_GraphicsContext *gc, jbyte *path, jfloat *matrix,
                                                           jint fillRule, jint blend, jint color) {
	return VG_DRAWING_STUB_drawPath(gc, path, matrix, fillRule, blend, color);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_drawGradient_2(MICROUI_GraphicsContext *gc, jbyte *path, jfloat *matrix,
                                                               jint fillRule, jint alpha, jint blend, jint *gradient,
                                                               jfloat *gradientMatrix) {
	return VG_DRAWING_STUB_drawGradient(gc, path, matrix, fillRule, alpha, blend, gradient, gradientMatrix);
}

BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_drawString_2(MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle,
                                                             jfloat size, jfloat *matrix, jint alpha, jint blend,
                                                             jfloat letterSpacing) {
	return VG_DRAWING_STUB_drawString(gc, text, faceHandle, size, matrix, alpha, blend, letterSpacing);
}

BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_drawStringGradient_2(MICROUI_GraphicsContext *gc, jchar *text,
                                                                     jint faceHandle, jfloat size, jfloat *matrix,
                                                                     jint alpha, jint blend, jfloat letterSpacing,
                                                                     jint *gradientData, jfloat *gradientMatrix) {
	return VG_DRAWING_STUB_drawStringGradient(gc, text, faceHandle, size, matrix, alpha, blend, letterSpacing,
	                                          gradientData, gradientMatrix);
}

BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_drawStringOnCircle_2(MICROUI_GraphicsContext *gc, jchar *text,
                                                                     jint faceHandle, jfloat size, jfloat *matrix,
                                                                     jint alpha, jint blend, jfloat letterSpacing,
                                                                     jfloat radius, jint direction) {
	return VG_DRAWING_STUB_drawStringOnCircle(gc, text, faceHandle, size, matrix, alpha, blend, letterSpacing, radius,
	                                          direction);
}

BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_drawStringOnCircleGradient_2(MICROUI_GraphicsContext *gc, jchar *text,
                                                                             jint faceHandle, jfloat size,
                                                                             jfloat *matrix, jint alpha, jint blend,
                                                                             jfloat letterSpacing, jfloat radius,
                                                                             jint direction, jint *gradientData,
                                                                             jfloat *gradientMatrix) {
	return VG_DRAWING_STUB_drawStringOnCircleGradient(gc, text, faceHandle, size, matrix, alpha, blend, letterSpacing,
	                                                  radius, direction, gradientData, gradientMatrix);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status VG_DRAWING_drawImage_2(MICROUI_GraphicsContext *gc, void *sni_context,
                                                            jfloat *matrix, jint alpha, jlong elapsed,
                                                            const float color_matrix[], jint *errno) {
	return VG_DRAWING_STUB_drawImage(gc, sni_context, matrix, alpha, elapsed, color_matrix, errno);
}

#endif // (UI_GC_SUPPORTED_FORMATS > 2)

#else // #if defined(UI_GC_SUPPORTED_FORMATS) && (UI_GC_SUPPORTED_FORMATS > 1)

/*
 * The VEE port supports only one destination format: the display buffer format. All
 * drawing functions are redirected to the stub implementation by default. A
 * third party implementation (often on a GPU) should replace each weak function independently.
 */

#endif // #if defined(UI_GC_SUPPORTED_FORMATS) && (UI_GC_SUPPORTED_FORMATS > 1)

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------
