/*
 * Copyright 2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#if !defined UI_FONT_DRAWING_H
#define UI_FONT_DRAWING_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief Provides font drawing functions called by ui_drawing.c.
 *
 * The default weak functions of ui_drawing.c call the ui_font_drawing.h functions to draw
 * the font. According to the kind of font (standard or custom), the implementation
 * of ui_font_drawing.h calls the right font manager functions.
 *
 * When the font is a standard font, the implementation calls the Graphics Engine
 * software algorithms.
 *
 * When the font is a custom font, the associated font manager is mandatory (the
 * Graphics Engine is not able to decode this custom font). The implementation
 * dispatches the font drawing to this font manager (the custom font manager is
 * retrieved thanks to the custom font format: MICROUI_FONT_FORMAT_CUSTOM_0 to
 * MICROUI_FONT_FORMAT_CUSTOM_7).
 *
 * Several cases:
 *
 * 1- The font manager is available and it is able to draw in the given Graphics
 * Context: it has to decode the font and draw into the Graphics Context.
 *
 * 2- The font manager is available and it is not able to draw in the given Graphics
 * Context (unknown ouput format) but it is able to draw the font into the Graphics
 * Context by using the ui_drawing.h functions.
 *
 * 3- The font manager is available, it is not able to draw in the given Graphics
 * Context (unknown ouput format) and it is not able to call standard ui_drawing.h
 * functions to draw the font: it has to call the stub implementation.
 *
 * @author MicroEJ Developer Team
 * @version 14.2.0
 */

// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------

#include "ui_drawing.h"

// --------------------------------------------------------------------------------
// Macros and Defines
// --------------------------------------------------------------------------------

/**
 * @brief The suffixes to apply at the end of the implementation functions of ui_font_drawing.c's
 * extern functions that handle a font format between MICROUI_FONT_FORMAT_CUSTOM_0
 * and MICROUI_FONT_FORMAT_CUSTOM_7.
 */
#define MICROUI_FONT_FORMAT_CUSTOM_0_FUNCTIONS_SUFFIX 0
#define MICROUI_FONT_FORMAT_CUSTOM_1_FUNCTIONS_SUFFIX 1
#define MICROUI_FONT_FORMAT_CUSTOM_2_FUNCTIONS_SUFFIX 2
#define MICROUI_FONT_FORMAT_CUSTOM_3_FUNCTIONS_SUFFIX 3
#define MICROUI_FONT_FORMAT_CUSTOM_4_FUNCTIONS_SUFFIX 4
#define MICROUI_FONT_FORMAT_CUSTOM_5_FUNCTIONS_SUFFIX 5
#define MICROUI_FONT_FORMAT_CUSTOM_6_FUNCTIONS_SUFFIX 6
#define MICROUI_FONT_FORMAT_CUSTOM_7_FUNCTIONS_SUFFIX 7

/**
 * @brief Macro to get the right implementation functions of ui_font_drawing.c's
 * extern functions according to the custom font format.
 */
#define GET_CUSTOM_FONT_FUNCTIONS_SUFFIX(format) CONCAT(format, _FUNCTIONS_SUFFIX)

// --------------------------------------------------------------------------------
// API
// --------------------------------------------------------------------------------

/*
 * @brief Computes the rendered width of a string.
 * @see UI_DRAWING_stringWidth
 */
jint UI_FONT_DRAWING_stringWidth(jchar *chars, jint length, MICROUI_Font *font);

/**
 * @brief Computes the rendered width of a renderable string and fills its SNI context.
 * @see UI_DRAWING_initializeRenderableStringSNIContext
 */
jint UI_FONT_DRAWING_initializeRenderableStringSNIContext(jchar *chars, jint length, MICROUI_Font *font,
                                                          MICROUI_RenderableString *renderableString);

/*
 * @brief Draws a string.
 * @see UI_DRAWING_drawString
 */
DRAWING_Status UI_FONT_DRAWING_drawString(MICROUI_GraphicsContext *gc, jchar *chars, jint length, MICROUI_Font *font,
                                          jint x, jint y);

/**
 * @brief Draws a renderable string.
 * @see UI_DRAWING_drawRenderableString
 */
DRAWING_Status UI_FONT_DRAWING_drawRenderableString(MICROUI_GraphicsContext *gc, jchar *chars, jint length,
                                                    MICROUI_Font *font, jint width,
                                                    MICROUI_RenderableString *renderableString, jint x, jint y);

/*
 * @brief Draws a string applying a scaling.
 * @see UI_DRAWING_drawScaledStringBilinear
 */
DRAWING_Status UI_FONT_DRAWING_drawScaledStringBilinear(MICROUI_GraphicsContext *gc, jchar *chars, jint length,
                                                        MICROUI_Font *font, jint x, jint y, jfloat xRatio,
                                                        jfloat yRatio);

/*
 * @brief Draws a renderable string applying a scaling.
 * @see UI_DRAWING_drawScaledRenderableStringBilinear
 */
DRAWING_Status UI_FONT_DRAWING_drawScaledRenderableStringBilinear(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                  jint length, MICROUI_Font *font, jint width,
                                                                  MICROUI_RenderableString *renderableString, jint x,
                                                                  jint y, jfloat xRatio, jfloat yRatio);

/*
 * @brief Draws a character applying a rotation and an alpha value.
 * @see UI_DRAWING_drawCharWithRotationBilinear
 */
DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationBilinear(MICROUI_GraphicsContext *gc, jchar c, MICROUI_Font *font,
                                                            jint x, jint y, jint xRotation, jint yRotation,
                                                            jfloat angle, jint alpha);

/*
 * @brief Draws a character applying a rotation and an alpha value.
 * @see UI_DRAWING_drawCharWithRotationNearestNeighbor
 */
DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor(MICROUI_GraphicsContext *gc, jchar c,
                                                                   MICROUI_Font *font, jint x, jint y, jint xRotation,
                                                                   jint yRotation, jfloat angle, jint alpha);

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // UI_FONT_DRAWING_H
