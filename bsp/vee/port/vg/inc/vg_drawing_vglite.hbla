/*
 * Copyright 2023-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#if !defined VG_DRAWING_VGLITE_H
#define VG_DRAWING_VGLITE_H
#ifdef __cplusplus
extern "C" {
#endif

/*
 * @file
 * @brief Implementation of a set of vg_drawing.h drawing functions (library MicroVG).
 * These are implementations over VGLite and the destination buffer format is the
 * format specified in the VEE port.
 * @author MicroEJ Developer Team
 * @version 8.0.1
 * @see ui_drawing_vglite.h
 */

// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------

#include "vg_drawing.h"
#include "vg_path.h"
#include "vg_freetype.h"

#include "vg_lite.h"

// --------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------

#if !defined VG_FEATURE_FONT || (VG_FEATURE_FONT != VG_FEATURE_FONT_FREETYPE_VECTOR)
#error "This drawer requires Freetype".
#endif

#if !defined(UI_GC_SUPPORTED_FORMATS) || (UI_GC_SUPPORTED_FORMATS <= 1)

/*
 * The fonction VG_DRAWING_VGLITE_xxx() are directly called by LLVG_PAINTER_impl.c
 * This file overrides each function independently to use the VGLite GPU.
 */

#define VG_DRAWING_VGLITE_drawPath VG_DRAWING_drawPath
#define VG_DRAWING_VGLITE_drawGradient VG_DRAWING_drawGradient
#define VG_DRAWING_VGLITE_drawString VG_DRAWING_drawString
#define VG_DRAWING_VGLITE_drawStringGradient VG_DRAWING_drawStringGradient
#define VG_DRAWING_VGLITE_drawStringOnCircle VG_DRAWING_drawStringOnCircle
#define VG_DRAWING_VGLITE_drawStringOnCircleGradient VG_DRAWING_drawStringOnCircleGradient
#define VG_DRAWING_VGLITE_drawImage VG_DRAWING_drawImage

#else // !defined(UI_GC_SUPPORTED_FORMATS) || (UI_GC_SUPPORTED_FORMATS <= 1)

/*
 * The functions VG_DRAWING_VGLITE_xxx() are indirectly called through some tables.
 * The functions have got the identifier "0" as suffix. This file overrides each
 * function independently to use the VGLite GPU.
 */

#define VG_DRAWING_VGLITE_drawPath VG_DRAWING_drawPath_0
#define VG_DRAWING_VGLITE_drawGradient VG_DRAWING_drawGradient_0
#define VG_DRAWING_VGLITE_drawString VG_DRAWING_drawString_0
#define VG_DRAWING_VGLITE_drawStringGradient VG_DRAWING_drawStringGradient_0
#define VG_DRAWING_VGLITE_drawStringOnCircle VG_DRAWING_drawStringOnCircle_0
#define VG_DRAWING_VGLITE_drawStringOnCircleGradient VG_DRAWING_drawStringOnCircleGradient_0
#define VG_DRAWING_VGLITE_drawImage VG_DRAWING_drawImage_0

#endif // !defined(UI_GC_SUPPORTED_FORMATS) || (UI_GC_SUPPORTED_FORMATS <= 1)

// --------------------------------------------------------------------------------
// Typedef
// --------------------------------------------------------------------------------

/*
 * @brief Function to draw an image element. An image element is a vectorial path
 * and the function parameters are similar to vg_lite_draw() and vg_lite_draw_gradient().
 *
 * @param[in] color the color to apply if gradient is NULL
 * @param[in] gradient the gradient to apply or NULL
 * @param[in] is_new_gradient is true when the given gradient is different than the one
 * of the previous call (with a non-null gradient).
 *
 * @see vg_lite_draw()
 *
 * @return: the MicroVG error code
 */
typedef jint (* VG_DRAWING_VGLITE_draw_image_element_t) (const vg_lite_path_t *path,
                                                         vg_lite_fill_t fill_rule,
                                                         const vg_lite_matrix_t *matrix,
                                                         vg_lite_blend_t blend,
                                                         vg_lite_color_t color,
                                                         vg_lite_linear_gradient_t *shared_gradient,
                                                         bool is_new_gradient);

// --------------------------------------------------------------------------------
// Global variables
// --------------------------------------------------------------------------------

/*
 * @brief Working variable used to render paths
 */
extern vg_lite_path_t render_path;

// --------------------------------------------------------------------------------
// Public API
// --------------------------------------------------------------------------------

/*
 * @brief Converts a MicroVG LinearGradient in a VGLite gradient according to
 * the drawing parameters. After this call, the gradient is ready to be updated
 * by calling vg_lite_update_grad().
 *
 * This function is similar to VG_VGLITE_HELPER_to_vg_lite_gradient() but
 * the gradientMatrix may be NULL (identity matrix is used instead).
 *
 * @see VG_VGLITE_HELPER_to_vg_lite_gradient()
 */
vg_lite_error_t VG_DRAWING_VGLITE_prepare_gradient(vg_lite_linear_gradient_t *vg_lite_gradient,
                                                   const jint *gradient_data, const jfloat *gradient_matrix,
                                                   const jfloat *global_matrix, int alpha);

/*
 * @brief Clears the VGLite gradient data: vg_lite_init_grad() allocates a buffer
 * in VGLite buffer, this function frees it.
 *
 * @param[in] gradient the VGLite gradient to clear
 */
void VG_DRAWING_VGLITE_clear_gradient(vg_lite_linear_gradient_t *gradient);

/*
 * @brief Converts a MicroVG path in a VGLite path. The same VGLite path is returned, by
 * consequence only one path can be converted at any time.
 *
 * @param[in] path the MicroVG path to convert
 *
 * @return the VGLite path
 */
vg_lite_path_t * VG_DRAWING_VGLITE_to_vglite_path(VG_PATH_HEADER_t *path);

/*
 * @brief Generic function to prepare the drawing of an image. The implementation does
 * not draw, it calls the drawer function for each image element.
 *
 * @param[in] drawer the function to draw an image element.
 * @param[in] image the destination.
 * @param[in] matrix the transformation matrix to apply
 * @param[in] alpha the global opacity rendering value
 * @param[in] elapsedTime the elapsed time since the beginning of the animation, in milliseconds
 * @param[in] colorMatrix the color matrix used to transform colors, may be null (means no color deformation)
 * @param[out] rendered true if something has been drawn
 *
 * @return LLVG_SUCCESS on a success, a different value otherwise
 */
jint VG_DRAWING_VGLITE_draw_image(VG_DRAWING_VGLITE_draw_image_element_t drawer, const MICROVG_Image *image,
                                  const jfloat *drawing_matrix, jint alpha, jlong elapsed, const float color_matrix[],
                                  bool *rendered);

// --------------------------------------------------------------------------------
// vg_drawing.h API
// (the function names differ according to the available number of destination formats)
// --------------------------------------------------------------------------------

/*
 * @brief Implementation of drawPath over VGLite. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_VGLITE_drawPath(MICROUI_GraphicsContext *gc, const jbyte *path, const jfloat *matrix,
                                          jint fillRule, jint blend, jint color);

/*
 * @brief Implementation of drawGradient over VGLite. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_VGLITE_drawGradient(MICROUI_GraphicsContext *gc, const jbyte *path, const jfloat *matrix,
                                              jint fillRule, jint alpha, jint blend, const jint *gradient,
                                              const jfloat *gradientMatrix);

/*
 * @brief Implementation of drawString over VGLite. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_VGLITE_drawString(MICROUI_GraphicsContext *gc, const jchar *text, jint faceHandle,
                                            jfloat size, const jfloat *matrix, jint alpha, jint blend,
                                            jfloat letterSpacing);

/*
 * @brief Implementation of drawStringGradient over VGLite. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_VGLITE_drawStringGradient(MICROUI_GraphicsContext *gc, const jchar *text, jint faceHandle,
                                                    jfloat size, const jfloat *matrix, jint alpha, jint blend,
                                                    jfloat letterSpacing, const jint *gradientData,
                                                    const jfloat *gradientMatrix);

/*
 * @brief Implementation of drawStringOnCircle over VGLite. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_VGLITE_drawStringOnCircle(MICROUI_GraphicsContext *gc, const jchar *text, jint faceHandle,
                                                    jfloat size, const jfloat *matrix, jint alpha, jint blend,
                                                    jfloat letterSpacing, jfloat radius, jint direction);

/*
 * @brief Implementation of drawStringOnCircleGradient over VGLite. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_VGLITE_drawStringOnCircleGradient(MICROUI_GraphicsContext *gc, const jchar *text,
                                                            jint faceHandle, jfloat size, const jfloat *matrix,
                                                            jint alpha, jint blend, jfloat letterSpacing, jfloat radius,
                                                            jint direction, const jint *gradientData,
                                                            const jfloat *gradientMatrix);

/*
 * @brief Implementation of drawImage over VGLite. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_VGLITE_drawImage(MICROUI_GraphicsContext *gc, const void *image, const jfloat *matrix,
                                           jint alpha, jlong elapsed, const float color_matrix[], jint *errno);

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
#endif // VG_DRAWING_VGLITE_H
