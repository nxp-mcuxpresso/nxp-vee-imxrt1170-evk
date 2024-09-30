/*
 * Copyright 2023-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#if !defined UI_DRAWING_VGLITE_H
#define UI_DRAWING_VGLITE_H
#ifdef __cplusplus
extern "C" {
#endif

/*
 * @file
 * @brief Implementation of a set of ui_drawing.h drawing functions (MicroUI and Drawing libraries).
 * These are implementations over VGLite and the destination buffer format is the format specified
 * in the VEE port. When the drawing cannot be performed by the GPU, the software implementation
 * is used instead.
 * @author MicroEJ Developer Team
 * @version 9.0.0
 */

// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------

#include "ui_drawing.h"

#include "vg_lite.h"

// --------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------

#if !defined(LLUI_GC_SUPPORTED_FORMATS) || (LLUI_GC_SUPPORTED_FORMATS <= 1)

/*
 * The functions UI_DRAWING_VGLITE_xxx() are directly called by LLUI_PAINTER_impl.c
 * and LLDW_PAINTER_impl.c. This file overrides each function independently to use the
 * VGLite GPU.
 */

#define UI_DRAWING_VGLITE_writePixel UI_DRAWING_writePixel
#define UI_DRAWING_VGLITE_drawLine UI_DRAWING_drawLine
#define UI_DRAWING_VGLITE_drawHorizontalLine UI_DRAWING_drawHorizontalLine
#define UI_DRAWING_VGLITE_drawVerticalLine UI_DRAWING_drawVerticalLine
#define UI_DRAWING_VGLITE_drawRectangle UI_DRAWING_drawRectangle
#define UI_DRAWING_VGLITE_fillRectangle UI_DRAWING_fillRectangle
#define UI_DRAWING_VGLITE_drawRoundedRectangle UI_DRAWING_drawRoundedRectangle
#define UI_DRAWING_VGLITE_fillRoundedRectangle UI_DRAWING_fillRoundedRectangle
#define UI_DRAWING_VGLITE_drawCircleArc UI_DRAWING_drawCircleArc
#define UI_DRAWING_VGLITE_drawEllipseArc UI_DRAWING_drawEllipseArc
#define UI_DRAWING_VGLITE_fillCircleArc UI_DRAWING_fillCircleArc
#define UI_DRAWING_VGLITE_fillEllipseArc UI_DRAWING_fillEllipseArc
#define UI_DRAWING_VGLITE_drawEllipse UI_DRAWING_drawEllipse
#define UI_DRAWING_VGLITE_fillEllipse UI_DRAWING_fillEllipse
#define UI_DRAWING_VGLITE_drawCircle UI_DRAWING_drawCircle
#define UI_DRAWING_VGLITE_fillCircle UI_DRAWING_fillCircle
#define UI_DRAWING_VGLITE_drawImage UI_DRAWING_drawImage
#define UI_DRAWING_VGLITE_copyImage UI_DRAWING_copyImage
#define UI_DRAWING_VGLITE_drawRegion UI_DRAWING_drawRegion

#define UI_DRAWING_VGLITE_drawThickFadedPoint UI_DRAWING_drawThickFadedPoint
#define UI_DRAWING_VGLITE_drawThickFadedLine UI_DRAWING_drawThickFadedLine
#define UI_DRAWING_VGLITE_drawThickFadedCircle UI_DRAWING_drawThickFadedCircle
#define UI_DRAWING_VGLITE_drawThickFadedCircleArc UI_DRAWING_drawThickFadedCircleArc
#define UI_DRAWING_VGLITE_drawThickFadedEllipse UI_DRAWING_drawThickFadedEllipse
#define UI_DRAWING_VGLITE_drawThickLine UI_DRAWING_drawThickLine
#define UI_DRAWING_VGLITE_drawThickCircle UI_DRAWING_drawThickCircle
#define UI_DRAWING_VGLITE_drawThickEllipse UI_DRAWING_drawThickEllipse
#define UI_DRAWING_VGLITE_drawThickCircleArc UI_DRAWING_drawThickCircleArc
#define UI_DRAWING_VGLITE_drawFlippedImage UI_DRAWING_drawFlippedImage
#define UI_DRAWING_VGLITE_drawRotatedImageNearestNeighbor UI_DRAWING_drawRotatedImageNearestNeighbor
#define UI_DRAWING_VGLITE_drawRotatedImageBilinear UI_DRAWING_drawRotatedImageBilinear
#define UI_DRAWING_VGLITE_drawScaledImageNearestNeighbor UI_DRAWING_drawScaledImageNearestNeighbor
#define UI_DRAWING_VGLITE_drawScaledImageBilinear UI_DRAWING_drawScaledImageBilinear

#else // !defined(LLUI_GC_SUPPORTED_FORMATS) || (LLUI_GC_SUPPORTED_FORMATS <= 1)

/*
 * The functions UI_DRAWING_VGLITE_xxx() are indirectly called through some tables.
 * The functions have got the identifier "0" as suffix. This file overrides each function
 * independently to use the VGLite GPU.
 */

#define UI_DRAWING_VGLITE_writePixel UI_DRAWING_writePixel_0
#define UI_DRAWING_VGLITE_drawLine UI_DRAWING_drawLine_0
#define UI_DRAWING_VGLITE_drawHorizontalLine UI_DRAWING_drawHorizontalLine_0
#define UI_DRAWING_VGLITE_drawVerticalLine UI_DRAWING_drawVerticalLine_0
#define UI_DRAWING_VGLITE_drawRectangle UI_DRAWING_drawRectangle_0
#define UI_DRAWING_VGLITE_fillRectangle UI_DRAWING_fillRectangle_0
#define UI_DRAWING_VGLITE_drawRoundedRectangle UI_DRAWING_drawRoundedRectangle_0
#define UI_DRAWING_VGLITE_fillRoundedRectangle UI_DRAWING_fillRoundedRectangle_0
#define UI_DRAWING_VGLITE_drawCircleArc UI_DRAWING_drawCircleArc_0
#define UI_DRAWING_VGLITE_drawEllipseArc UI_DRAWING_drawEllipseArc_0
#define UI_DRAWING_VGLITE_fillCircleArc UI_DRAWING_fillCircleArc_0
#define UI_DRAWING_VGLITE_fillEllipseArc UI_DRAWING_fillEllipseArc_0
#define UI_DRAWING_VGLITE_drawEllipse UI_DRAWING_drawEllipse_0
#define UI_DRAWING_VGLITE_fillEllipse UI_DRAWING_fillEllipse_0
#define UI_DRAWING_VGLITE_drawCircle UI_DRAWING_drawCircle_0
#define UI_DRAWING_VGLITE_fillCircle UI_DRAWING_fillCircle_0
#define UI_DRAWING_VGLITE_drawImage UI_DRAWING_drawImage_0
#define UI_DRAWING_VGLITE_copyImage UI_DRAWING_copyImage_0
#define UI_DRAWING_VGLITE_drawRegion UI_DRAWING_drawRegion_0

#define UI_DRAWING_VGLITE_drawThickFadedPoint UI_DRAWING_drawThickFadedPoint_0
#define UI_DRAWING_VGLITE_drawThickFadedLine UI_DRAWING_drawThickFadedLine_0
#define UI_DRAWING_VGLITE_drawThickFadedCircle UI_DRAWING_drawThickFadedCircle_0
#define UI_DRAWING_VGLITE_drawThickFadedCircleArc UI_DRAWING_drawThickFadedCircleArc_0
#define UI_DRAWING_VGLITE_drawThickFadedEllipse UI_DRAWING_drawThickFadedEllipse_0
#define UI_DRAWING_VGLITE_drawThickLine UI_DRAWING_drawThickLine_0
#define UI_DRAWING_VGLITE_drawThickCircle UI_DRAWING_drawThickCircle_0
#define UI_DRAWING_VGLITE_drawThickEllipse UI_DRAWING_drawThickEllipse_0
#define UI_DRAWING_VGLITE_drawThickCircleArc UI_DRAWING_drawThickCircleArc_0
#define UI_DRAWING_VGLITE_drawFlippedImage UI_DRAWING_drawFlippedImage_0
#define UI_DRAWING_VGLITE_drawRotatedImageNearestNeighbor UI_DRAWING_drawRotatedImageNearestNeighbor_0
#define UI_DRAWING_VGLITE_drawRotatedImageBilinear UI_DRAWING_drawRotatedImageBilinear_0
#define UI_DRAWING_VGLITE_drawScaledImageNearestNeighbor UI_DRAWING_drawScaledImageNearestNeighbor_0
#define UI_DRAWING_VGLITE_drawScaledImageBilinear UI_DRAWING_drawScaledImageBilinear_0

#endif // !defined(LLUI_GC_SUPPORTED_FORMATS) || (LLUI_GC_SUPPORTED_FORMATS <= 1)

// --------------------------------------------------------------------------------
// ui_drawing.h API
// (the function names differ according to the available number of destination formats)
// --------------------------------------------------------------------------------

/*
 * @brief Implementation of drawLine over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_drawLine(MICROUI_GraphicsContext *gc, jint startX, jint startY, jint endX, jint endY);

/*
 * @brief Implementation of drawHorizontalLine over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_drawHorizontalLine(MICROUI_GraphicsContext *gc, jint x1, jint x2, jint y);

/*
 * @brief Implementation of drawVerticalLine over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_drawVerticalLine(MICROUI_GraphicsContext *gc, jint x, jint y1, jint y2);

/*
 * @brief Implementation of fillRectangle over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_fillRectangle(MICROUI_GraphicsContext *gc, jint x1, jint y1, jint x2, jint y2);

/*
 * @brief Implementation of drawRoundedRectangle over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_drawRoundedRectangle(MICROUI_GraphicsContext *gc, jint x, jint y, jint width,
                                                      jint height, jint cornerEllipseWidth, jint cornerEllipseHeight);

/*
 * @brief Implementation of fillRoundedRectangle over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_fillRoundedRectangle(MICROUI_GraphicsContext *gc, jint x, jint y, jint width,
                                                      jint height, jint cornerEllipseWidth, jint cornerEllipseHeight);

/*
 * @brief Implementation of drawCircleArc over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_drawCircleArc(MICROUI_GraphicsContext *gc, jint x, jint y, jint diameter,
                                               jfloat startAngle, jfloat arcAngle);

/*
 * @brief Implementation of drawEllipseArc over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_drawEllipseArc(MICROUI_GraphicsContext *gc, jint x, jint y, jint width, jint height,
                                                jfloat startAngle, jfloat arcAngle);

/*
 * @brief Implementation of fillCircleArc over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_fillCircleArc(MICROUI_GraphicsContext *gc, jint x, jint y, jint diameter,
                                               jfloat startAngle, jfloat arcAngle);

/*
 * @brief Implementation of fillEllipseArc over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_fillEllipseArc(MICROUI_GraphicsContext *gc, jint x, jint y, jint width, jint height,
                                                jfloat startAngle, jfloat arcAngle);

/*
 * @brief Implementation of drawEllipse over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_drawEllipse(MICROUI_GraphicsContext *gc, jint x, jint y, jint width, jint height);

/*
 * @brief Implementation of fillEllipse over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_fillEllipse(MICROUI_GraphicsContext *gc, jint x, jint y, jint width, jint height);

/*
 * @brief Implementation of drawCircle over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_drawCircle(MICROUI_GraphicsContext *gc, jint x, jint y, jint diameter);

/*
 * @brief Implementation of fillCircle over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_fillCircle(MICROUI_GraphicsContext *gc, jint x, jint y, jint diameter);

/*
 * @brief Implementation of drawImage over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_drawImage(MICROUI_GraphicsContext *gc, MICROUI_Image *img, jint regionX, jint regionY,
                                           jint width, jint height, jint x, jint y, jint alpha);

/*
 * @brief Implementation of copyImage over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_copyImage(MICROUI_GraphicsContext *gc, MICROUI_Image *img, jint regionX, jint regionY,
                                           jint width, jint height, jint x, jint y);

/*
 * @brief Implementation of drawRegion over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_drawRegion(MICROUI_GraphicsContext *gc, jint regionX, jint regionY, jint width,
                                            jint height, jint x, jint y, jint alpha);

/*
 * @brief Implementation of drawThickFadedPoint over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_drawThickFadedPoint(MICROUI_GraphicsContext *gc, jint x, jint y, jint thickness,
                                                     jint fade);

/*
 * @brief Implementation of drawThickFadedLine over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_drawThickFadedLine(MICROUI_GraphicsContext *gc, jint startX, jint startY, jint endX,
                                                    jint endY, jint thickness, jint fade, DRAWING_Cap startCap,
                                                    DRAWING_Cap endCap);

/*
 * @brief Implementation of drawThickFadedCircle over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_drawThickFadedCircle(MICROUI_GraphicsContext *gc, jint x, jint y, jint diameter,
                                                      jint thickness, jint fade);

/*
 * @brief Implementation of drawThickFadedCircleArc over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_drawThickFadedCircleArc(MICROUI_GraphicsContext *gc, jint x, jint y, jint diameter,
                                                         jfloat startAngle, jfloat arcAngle, jint thickness, jint fade,
                                                         DRAWING_Cap start, DRAWING_Cap end);

/*
 * @brief Implementation of drawThickFadedEllipse over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_drawThickFadedEllipse(MICROUI_GraphicsContext *gc, jint x, jint y, jint width,
                                                       jint height, jint thickness, jint fade);

/*
 * @brief Implementation of drawThickLine over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_drawThickLine(MICROUI_GraphicsContext *gc, jint startX, jint startY, jint endX,
                                               jint endY, jint thickness);

/*
 * @brief Implementation of drawThickCircle over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_drawThickCircle(MICROUI_GraphicsContext *gc, jint x, jint y, jint diameter,
                                                 jint thickness);

/*
 * @brief Implementation of drawThickEllipse over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_drawThickEllipse(MICROUI_GraphicsContext *gc, jint x, jint y, jint width, jint height,
                                                  jint thickness);

/*
 * @brief Implementation of drawThickCircleArc over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_drawThickCircleArc(MICROUI_GraphicsContext *gc, jint x, jint y, jint diameter,
                                                    jfloat startAngle, jfloat arcAngle, jint thickness);

/*
 * @brief Implementation of drawFlippedImage over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_drawFlippedImage(MICROUI_GraphicsContext *gc, MICROUI_Image *img, jint regionX,
                                                  jint regionY, jint width, jint height, jint x, jint y,
                                                  DRAWING_Flip transformation, jint alpha);

/*
 * @brief Implementation of drawRotatedImageNearestNeighbor over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_drawRotatedImageNearestNeighbor(MICROUI_GraphicsContext *gc, MICROUI_Image *img,
                                                                 jint x, jint y, jint rotationX, jint rotationY,
                                                                 jfloat angle, jint alpha);

/*
 * @brief Implementation of drawRotatedImageBilinear over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_drawRotatedImageBilinear(MICROUI_GraphicsContext *gc, MICROUI_Image *img, jint x,
                                                          jint y, jint rotationX, jint rotationY, jfloat angle,
                                                          jint alpha);

/*
 * @brief Implementation of drawScaledImageNearestNeighbor over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_drawScaledImageNearestNeighbor(MICROUI_GraphicsContext *gc, MICROUI_Image *img, jint x,
                                                                jint y, jfloat factorX, jfloat factorY, jint alpha);

/*
 * @brief Implementation of drawScaledImageBilinear over VGLite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_drawScaledImageBilinear(MICROUI_GraphicsContext *gc, MICROUI_Image *img, jint x,
                                                         jint y, jfloat factorX, jfloat factorY, jint alpha);

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
#endif // UI_DRAWING_VGLITE_H
