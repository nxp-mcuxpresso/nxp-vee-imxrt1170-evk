/* 
 * Copyright 2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#if !defined VG_DRAWING_H
#define VG_DRAWING_H
#ifdef __cplusplus
extern "C" {
#endif


/*
 * @brief Provides drawing functions called by MicroVG native drawing functions.
 *
 * This header file follows strictly the same concept as ui_drawing.h but for MicroVG
 * library.
 *
 * @see ui_drawing.h
 */

// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------

#include <LLVG_PAINTER_impl.h>

#include "ui_drawing.h"

// --------------------------------------------------------------------------------
// API
// --------------------------------------------------------------------------------

/*
 * @brief Gets the image size. The image structure format is VEE Port specific.
 *
 * @param[in] image: the image
 * @param[out] width: the image width
 * @param[out] height: the image height
 */
void VG_DRAWING_get_image_size(void* image, float* width, float* height);

/*
 * @brief Draws a path filled with the specified color (not the one of the Graphics Context).
 *
 * @param[in] gc: the destination
 * @param[in] path: the path's data
 * @param[in] x: the destination X coordinate
 * @param[in] y: the destination Y coordinate
 * @param[in] matrix: the deformation to apply
 * @param[in] fillRule: the fill type
 * @param[in] blend: the blend mode
 * @param[in] color: the color to apply (with an opacity)
 *
 * @return the drawing status.
 */
DRAWING_Status VG_DRAWING_drawPath(MICROUI_GraphicsContext* gc, jbyte* path, jint x, jint y, jfloat* matrix, jint fillRule, jint blend, jint color);

/*
 * @brief Draws a path filled with a linear gradient.
 *
 * @param[in] gc: the destination
 * @param[in] path: the path's data
 * @param[in] x: the destination X coordinate
 * @param[in] y: the destination Y coordinate
 * @param[in] matrix: the deformation to apply
 * @param[in] fillRule: the fill type
 * @param[in] alpha: the global opacity to apply
 * @param[in] blend: the blend mode
 * @param[in] gradient: the gradient to apply
 * @param[in] gradientMatrix: the gradient deformation, may be null (means identity)
 *
 * @return the drawing status.
 */
DRAWING_Status VG_DRAWING_drawGradient(MICROUI_GraphicsContext* gc, jbyte* path, jint x, jint y, jfloat* matrix, jint fillRule, jint alpha, jint blend, jint* gradient, jfloat* gradientMatrix);

/*
 * @brief  Draws a string with the color of the Graphics Context.
 *
 * The top/left position of the first drawn character is placed at
 * coordinate x/y.
 *
 * @param[in] gc the MicroUI GraphicsContext target.
 * @param[in] text the array of characters to draw.
 * @param[in] faceHandle the font reference handle.
 * @param[in] size the height of the font in pixels.
 * @param[in] x the horizontal coordinate of the top/left of the first drawn character.
 * @param[in] y the vertical coordinate of the top/left of the first drawn character.
 * @param[in] matrix: deformation matrix, may be null (means identity)
 * @param[in] alpha the opacity level to apply to the character.
 * @param[in] blend the blend mode to use
 * @param[in] letterSpacing the extra letter spacing to use
 *
 * @return the drawing status.
 */
DRAWING_Status VG_DRAWING_drawString(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jfloat x, jfloat y,  jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing);

/*
 * @brief Draws a string with a linear gradient.
 *
 * @param[in] gc the MicroUI GraphicsContext target.
 * @param[in] text the array of characters to draw.
 * @param[in] faceHandle the font reference handle.
 * @param[in] size the height of the font in pixels.
 * @param[in] x the horizontal coordinate of the top/left of the first drawn character.
 * @param[in] y the vertical coordinate of the top/left of the first drawn character.
 * @param[in] matrix: deformation matrix, may be null (means identity)
 * @param[in] alpha the opacity level to apply to the character.
 * @param[in] blend the blend mode to use
 * @param[in] letterSpacing the extra letter spacing to use
 * @param[in] gradientData the gradient to apply
 * @param[in] gradientMatrix the gradient deformation, may be null (means identity)
 *
 * @return the drawing status.
 */
DRAWING_Status VG_DRAWING_drawStringGradient(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jfloat x, jfloat y,  jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing, jint *gradientData, jfloat *gradientMatrix);

/*
 * @brief Draws a string along a circle, with the color of the Graphics Context.
 *
 * @param[in] gc the MicroUI GraphicsContext target.
 * @param[in] text the array of characters to draw.
 * @param[in] faceHandle the font reference handle.
 * @param[in] size the height of the font in pixels.
 * @param[in] x the horizontal coordinate of the top/left of the first drawn character.
 * @param[in] y the vertical coordinate of the top/left of the first drawn character.
 * @param[in] matrix: deformation matrix, may be null (means identity)
 * @param[in] alpha the opacity level to apply to the character.
 * @param[in] blend the blend mode to use
 * @param[in] letterSpacing the extra letter spacing to use
 * @param[in] radius the radius of the circle
 * @param[in] direction the direction of the text along the circle
 *
 * @return the drawing status.
 */
DRAWING_Status VG_DRAWING_drawStringOnCircle(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jint x, jint y, jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing, jfloat radius, jint direction);

/*
 * @brief Draws a string along a circle, with a linear gradient.
 *
 * @param[in] gc the MicroUI GraphicsContext target.
 * @param[in] text the array of characters to draw.
 * @param[in] faceHandle the font reference handle.
 * @param[in] size the height of the font in pixels.
 * @param[in] x the horizontal coordinate of the top/left of the first drawn character.
 * @param[in] y the vertical coordinate of the top/left of the first drawn character.
 * @param[in] matrix: deformation matrix, may be null (means identity)
 * @param[in] alpha the opacity level to apply to the character.
 * @param[in] blend the blend mode to use
 * @param[in] letterSpacing the extra letter spacing to use
 * @param[in] radius the radius of the circle
 * @param[in] direction the direction of the text along the circle
 * @param[in] gradientData the gradient to apply
 * @param[in] gradientMatrix the gradient deformation, may be null (means identity)
 *
 * @return the drawing status.
 */
DRAWING_Status VG_DRAWING_drawStringOnCircleGradient(MICROUI_GraphicsContext* gc, jchar* text, jint faceHandle, jfloat size,  jint x, jint y, jfloat* matrix, jint alpha, jint blend, jfloat letterSpacing, jfloat radius, jint direction, jint *gradientData, jfloat *gradientMatrix);

/*
 * @brief Draws an image with transformation and opacity. Optionally apply an animation and / or a color filtering.
 *
 * @param[in] gc the graphics context to draw on
 * @param[in] image the image to draw
 * @param[in] matrix the transformation matrix to apply
 * @param[in] alpha the global opacity rendering value
 * @param[in] elapsedTime the elapsed time since the beginning of the animation, in milliseconds
 * @param[in] colorMatrix the color matrix used to transform colors, may be null (means no color deformation)
 * @param[out] errno the error code to return to MicroVG.
 *
 * @return the drawing status.
 */
DRAWING_Status VG_DRAWING_drawImage(MICROUI_GraphicsContext* gc, void* image, jfloat *matrix, jint alpha, jlong elapsed, const float color_matrix[], jint* errno) ;

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
#endif // VG_DRAWING_H
