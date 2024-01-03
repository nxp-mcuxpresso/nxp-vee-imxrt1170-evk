/* 
 * Copyright 2023 MicroEJ Corp. All rights reserved.
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
 * These are implementations over VG-Lite and the destination buffer format is the
 * format specified in the VEE port. When the drawing cannot be performed by the GPU,
 * the software implementation is used insted.
 * @author MicroEJ Developer Team
 * @version 6.1.0
 * @see ui_drawing_vglite.h
 */

// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------

#include "vg_drawing.h"
#include "microvg_path.h"

#include "vg_lite.h"

// --------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------

#if !defined VG_FEATURE_FONT || (VG_FEATURE_FONT != VG_FEATURE_FONT_FREETYPE_VECTOR)
#error "This drawer requires Freetype".
#endif

#if !defined(LLUI_GC_SUPPORTED_FORMATS) || (LLUI_GC_SUPPORTED_FORMATS <= 1)

/*
 * The fonction VG_DRAWING_VGLITE_xxx() are directly called by LLVG_PAINTER_impl.c
 * This file overrides each function independently to use the VG-Lite GPU.
 */

#define VG_DRAWING_VGLITE_drawPath VG_DRAWING_drawPath
#define VG_DRAWING_VGLITE_drawGradient VG_DRAWING_drawGradient
#define VG_DRAWING_VGLITE_drawString VG_DRAWING_drawString
#define VG_DRAWING_VGLITE_drawStringGradient VG_DRAWING_drawStringGradient
#define VG_DRAWING_VGLITE_drawStringOnCircle VG_DRAWING_drawStringOnCircle
#define VG_DRAWING_VGLITE_drawStringOnCircleGradient VG_DRAWING_drawStringOnCircleGradient
#define VG_DRAWING_VGLITE_drawImage VG_DRAWING_drawImage

#else // !defined(LLUI_GC_SUPPORTED_FORMATS) || (LLUI_GC_SUPPORTED_FORMATS <= 1)

/*
 * The functions VG_DRAWING_VGLITE_xxx() are indirectly called through some tables.
 * The functions have got the identifier "0" as suffix. This file overrides each
 * function independently to use the VG-Lite GPU.
 */

#define VG_DRAWING_VGLITE_drawPath VG_DRAWING_drawPath_0
#define VG_DRAWING_VGLITE_drawGradient VG_DRAWING_drawGradient_0
#define VG_DRAWING_VGLITE_drawString VG_DRAWING_drawString_0
#define VG_DRAWING_VGLITE_drawStringGradient VG_DRAWING_drawStringGradient_0
#define VG_DRAWING_VGLITE_drawStringOnCircle VG_DRAWING_drawStringOnCircle_0
#define VG_DRAWING_VGLITE_drawStringOnCircleGradient VG_DRAWING_drawStringOnCircleGradient_0
#define VG_DRAWING_VGLITE_drawImage VG_DRAWING_drawImage_0

#endif // !defined(LLUI_GC_SUPPORTED_FORMATS) || (LLUI_GC_SUPPORTED_FORMATS <= 1)

// --------------------------------------------------------------------------------
// Typedef
// --------------------------------------------------------------------------------

/*
 * @brief Function to draw a character element: a glyph. A glyph is a vectorial path
 * and the function parameters are similar to vg_lite_draw() and vg_lite_draw_gradient().
 *
 * @param[in] color the color to apply if gradient is NULL
 * @param[in] gradient the gradient to apply or NULL
 *
 * @see vg_lite_draw()
 * @see vg_lite_draw_gradient()
 *
 * @return: the MicroVG error code
 */
typedef jint (* VG_DRAWING_VGLITE_draw_glyph_t) (
		vg_lite_path_t * path,
		vg_lite_fill_t fill_rule,
		vg_lite_matrix_t * matrix,
		vg_lite_blend_t blend,
		vg_lite_color_t color,
		vg_lite_linear_gradient_t* grad);

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
typedef jint (* VG_DRAWING_VGLITE_draw_image_element_t) (
		vg_lite_path_t * path,
		vg_lite_fill_t fill_rule,
		vg_lite_matrix_t * matrix,
		vg_lite_blend_t blend,
		vg_lite_color_t color,
		vg_lite_linear_gradient_t* grad,
		bool is_new_gradient);

// --------------------------------------------------------------------------------
// Public API
// --------------------------------------------------------------------------------

/*
 * @brief Converts a MicroVG matrix in a VG-Lite matrix applying a translation.
 *
 * @param[out] vg_lite_matrix the VG-Lite matrix to set
 * @param[in] x the X translation to apply
 * @param[in] y the Y translation to apply
 * @param[in] matrix the MicroVG transformation to apply
 */
void VG_DRAWING_VGLITE_prepare_matrix(vg_lite_matrix_t* vg_lite_matrix, jfloat x, jfloat y, jfloat* matrix) ;

/*
 * @brief Converts a MicroVG LinearGradient in a VG-Lite gradient according to
 * the drawing parameters. After this call, the gradient is ready to be updated
 * by calling vg_lite_update_grad().
 *
 * This function is similar to MICROVG_VGLITE_HELPER_to_vg_lite_gradient() but
 * the gradientMatrix may be NULL (identity matrix is used instead).
 *
 * @see MICROVG_VGLITE_HELPER_to_vg_lite_gradient()
 */
vg_lite_error_t VG_DRAWING_VGLITE_prepare_gradient(vg_lite_linear_gradient_t* vg_lite_gradient, jint* gradientData, jfloat* gradientMatrix, vg_lite_matrix_t* vg_lite_matrix, int alpha) ;

/*
 * @brief Clears the VG-Lite gradient data: vg_lite_init_grad() allocates a buffer
 * in VG-Lite buffer, this function frees it.
 *
 * @param[in] gradient the VG-Lite gradient to clear
 */
void VG_DRAWING_VGLITE_clear_gradient(vg_lite_linear_gradient_t* gradient) ;

/*
 * @brief Converts a MicroVG path in a VG-Lite path. The same VG-Lite path is returned, by
 * consequence only one path can be converted at any time.
 *
 * @param[in] path the MicroVG path to convert
 *
 * @return the VG-Lite path
 */
vg_lite_path_t* VG_DRAWING_VGLITE_to_vglite_path(MICROVG_PATH_HEADER_t* path);

/*
 * @brief Generic function to prepare the drawing of a string along a line or a circle,
 * with a color or a linear gradient. The implementation does not draw, it calls the
 * drawer function for each glyph.
 *
 * @param[in] drawer the function to draw a glyph.
 * @param[in] text the array of characters to draw.
 * @param[in] faceHandle the font reference handle.
 * @param[in] size the height of the font in pixels.
 * @param[in] vgGlobalMatrix: deformation matrix
 * @param[in] blend the blend mode to use
 * @param[in] color the color to apply if gradient is NULL
 * @param[in] gradient the gradient to apply or NULL
 * @param[in] letterSpacing the extra letter spacing to use
 * @param[in] radius the radius of the circle (0 to draw along a line)
 * @param[in] direction the direction of the text along the circle (0 to draw along a line)
 * @param[out] rendered true if something has been drawn
 *
 * @return LLVG_SUCCESS if something has been drawn, an different value otherwise
 */
jint VG_DRAWING_VGLITE_draw_string(VG_DRAWING_VGLITE_draw_glyph_t drawer, jchar* text, jint faceHandle, jfloat size,  vg_lite_matrix_t* vgGlobalMatrix, vg_lite_blend_t blend, int color, vg_lite_linear_gradient_t *gradient, jfloat letterSpacing, jfloat radius, jint direction, bool* rendered);

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
jint VG_DRAWING_VGLITE_draw_image(VG_DRAWING_VGLITE_draw_image_element_t drawer, void* image, jfloat* drawing_matrix, jint alpha, jlong elapsed, const float color_matrix[], bool* rendered);

// --------------------------------------------------------------------------------
// vg_drawing.h API
// (the function names differ according to the available number of destination formats)
// --------------------------------------------------------------------------------

/*
 * @brief Implementation of drawPath over VG-Lite. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_VGLITE_drawPath(MICROUI_GraphicsContext* gc, jbyte* path, jint x, jint y, jfloat* matrix, jint fillRule, jint blend, jint color);

/*
 * @brief Implementation of drawGradient over VG-Lite. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_VGLITE_drawGradient(MICROUI_GraphicsContext* gc, jbyte* path, jint x, jint y, jfloat* matrix, jint fillRule, jint alpha, jint blend, jint* gradient, jfloat* gradientMatrix);

/*
 * @brief Implementation of drawString over VG-Lite. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_VGLITE_drawString(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jfloat x, jfloat y,  jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing);

/*
 * @brief Implementation of drawStringGradient over VG-Lite. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_VGLITE_drawStringGradient(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jfloat x, jfloat y,  jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing, jint *gradientData, jfloat *gradientMatrix);

/*
 * @brief Implementation of drawStringOnCircle over VG-Lite. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_VGLITE_drawStringOnCircle(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jint x, jint y, jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing, jfloat radius, jint direction);

/*
 * @brief Implementation of drawStringOnCircleGradient over VG-Lite. See vg_drawing.h
 */
DRAWING_Status VG_DRAWING_VGLITE_drawStringOnCircleGradient(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jint x, jint y, jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing, jfloat radius, jint direction, jint *gradientData, jfloat *gradientMatrix);

/*
 * @brief Implementation of drawImage over VG-Lite. See ui_drawing.h
 */
DRAWING_Status VG_DRAWING_VGLITE_drawImage(MICROUI_GraphicsContext* gc, void* image, jfloat *matrix, jint alpha, jlong elapsed, const float color_matrix[], jint* errno);

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
#endif // VG_DRAWING_VGLITE_H
