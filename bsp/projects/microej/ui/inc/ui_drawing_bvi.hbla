/* 
 * Copyright 2023-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#if !defined UI_DRAWING_BVI_H
#define UI_DRAWING_BVI_H
#ifdef __cplusplus
extern "C" {
#endif

/*
 * @file
 * @brief Implementation of a set of ui_drawing.h drawing functions (MicroUI and
 * Drawing libraries). These are implementations over a BufferedVectorImage and
 * the destination buffer format is the BufferedVectorImage format. When the drawing
 * cannot be performed for any reason, the stub implementation is used instead.
 * @author MicroEJ Developer Team
 * @version 5.0.0
 */

#include "microvg_configuration.h"
#if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE

// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------

#include <LLVG_BVI_impl.h>

#include "ui_drawing.h"

// --------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------

/*
 * @brief Redirects all ui_drawing.h functions to the BufferedVectorImage functions
 */

#define UI_DRAWING_BVI_is_drawer CONCAT(UI_DRAWING_is_drawer_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_adjustNewImageCharacteristics CONCAT(UI_DRAWING_adjustNewImageCharacteristics_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_initializeNewImage CONCAT(UI_DRAWING_initializeNewImage_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_freeImageResources CONCAT(UI_DRAWING_freeImageResources_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)

#define UI_DRAWING_BVI_drawLine CONCAT(UI_DRAWING_drawLine_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_drawHorizontalLine CONCAT(UI_DRAWING_drawHorizontalLine_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_drawVerticalLine CONCAT(UI_DRAWING_drawVerticalLine_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_fillRectangle CONCAT(UI_DRAWING_fillRectangle_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_drawRoundedRectangle CONCAT(UI_DRAWING_drawRoundedRectangle_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_fillRoundedRectangle CONCAT(UI_DRAWING_fillRoundedRectangle_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_drawCircleArc CONCAT(UI_DRAWING_drawCircleArc_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_drawEllipseArc CONCAT(UI_DRAWING_drawEllipseArc_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_fillCircleArc CONCAT(UI_DRAWING_fillCircleArc_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_fillEllipseArc CONCAT(UI_DRAWING_fillEllipseArc_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_drawEllipse CONCAT(UI_DRAWING_drawEllipse_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_fillEllipse CONCAT(UI_DRAWING_fillEllipse_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_drawCircle CONCAT(UI_DRAWING_drawCircle_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_fillCircle CONCAT(UI_DRAWING_fillCircle_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_drawThickFadedPoint CONCAT(UI_DRAWING_drawThickFadedPoint_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_drawThickFadedLine CONCAT(UI_DRAWING_drawThickFadedLine_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_drawThickFadedCircle CONCAT(UI_DRAWING_drawThickFadedCircle_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_drawThickFadedCircleArc CONCAT(UI_DRAWING_drawThickFadedCircleArc_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_drawThickFadedEllipse CONCAT(UI_DRAWING_drawThickFadedEllipse_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_drawThickLine CONCAT(UI_DRAWING_drawThickLine_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_drawThickCircle CONCAT(UI_DRAWING_drawThickCircle_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_drawThickEllipse CONCAT(UI_DRAWING_drawThickEllipse_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)
#define UI_DRAWING_BVI_drawThickCircleArc CONCAT(UI_DRAWING_drawThickCircleArc_, UI_DRAWING_IDENTIFIER_BVI_FORMAT)

// --------------------------------------------------------------------------------
// ui_drawing.h API
// (the function names differ according to the available number of destination formats)
// --------------------------------------------------------------------------------

/*
 * @brief Implementation of adjustNewImageCharacteristics for a BufferedVectorImage. See ui_drawing.h
 */
void UI_DRAWING_BVI_adjustNewImageCharacteristics(jbyte image_format, uint32_t width, uint32_t height, uint32_t* data_size, uint32_t* data_alignment);

/*
 * @brief Implementation of initializeNewImage for a BufferedVectorImage. See ui_drawing.h
 */
void UI_DRAWING_BVI_initializeNewImage(MICROUI_Image* image);

/*
 * @brief Implementation of freeImageResources for a BufferedVectorImage. See ui_drawing.h
 */
void UI_DRAWING_BVI_freeImageResources(MICROUI_Image* image) ;
  
/*
 * @brief Implementation of drawLine over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_drawLine(MICROUI_GraphicsContext* gc, jint startX, jint startY, jint endX, jint endY);

/*
 * @brief Implementation of drawHorizontalLine over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_drawHorizontalLine(MICROUI_GraphicsContext* gc, jint x1, jint x2, jint y);

/*
 * @brief Implementation of drawVerticalLine over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_drawVerticalLine(MICROUI_GraphicsContext* gc, jint x, jint y1, jint y2);

/*
 * @brief Implementation of fillRectangle over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_fillRectangle(MICROUI_GraphicsContext* gc, jint x1, jint y1, jint x2, jint y2);

/*
 * @brief Implementation of drawRoundedRectangle over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_drawRoundedRectangle(MICROUI_GraphicsContext* gc, jint x, jint y, jint width, jint height, jint cornerEllipseWidth, jint cornerEllipseHeight);

/*
 * @brief Implementation of fillRoundedRectangle over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_fillRoundedRectangle(MICROUI_GraphicsContext* gc, jint x, jint y, jint width, jint height, jint cornerEllipseWidth, jint cornerEllipseHeight);

/*
 * @brief Implementation of drawCircleArc over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_drawCircleArc(MICROUI_GraphicsContext* gc, jint x, jint y, jint diameter, jfloat startAngle, jfloat arcAngle);

/*
 * @brief Implementation of drawEllipseArc over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_drawEllipseArc(MICROUI_GraphicsContext* gc, jint x, jint y, jint width, jint height, jfloat startAngle, jfloat arcAngle);

/*
 * @brief Implementation of fillCircleArc over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_fillCircleArc(MICROUI_GraphicsContext* gc, jint x, jint y, jint diameter, jfloat startAngle, jfloat arcAngle);

/*
 * @brief Implementation of fillEllipseArc over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_fillEllipseArc(MICROUI_GraphicsContext* gc, jint x, jint y, jint width, jint height, jfloat startAngle, jfloat arcAngle);

/*
 * @brief Implementation of drawEllipse over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_drawEllipse(MICROUI_GraphicsContext* gc, jint x, jint y, jint width, jint height);

/*
 * @brief Implementation of fillEllipse over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_fillEllipse(MICROUI_GraphicsContext* gc, jint x, jint y, jint width, jint height);

/*
 * @brief Implementation of drawCircle over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_drawCircle(MICROUI_GraphicsContext* gc, jint x, jint y, jint diameter);

/*
 * @brief Implementation of fillCircle over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_fillCircle(MICROUI_GraphicsContext* gc, jint x, jint y, jint diameter);

/*
 * @brief Implementation of drawImage over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_drawImage(MICROUI_GraphicsContext* gc, MICROUI_Image* img, jint regionX, jint regionY, jint width, jint height, jint x, jint y, jint alpha);

/*
 * @brief Implementation of copyImage over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_copyImage(MICROUI_GraphicsContext* gc, MICROUI_Image* img, jint regionX, jint regionY, jint width, jint height, jint x, jint y);

/*
 * @brief Implementation of drawRegion over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_drawRegion(MICROUI_GraphicsContext* gc, jint regionX, jint regionY, jint width, jint height, jint x, jint y, jint alpha);

/*
 * @brief Implementation of drawThickFadedPoint over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_drawThickFadedPoint(MICROUI_GraphicsContext* gc, jint x, jint y, jint thickness, jint fade);

/*
 * @brief Implementation of drawThickFadedLine over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_drawThickFadedLine(MICROUI_GraphicsContext* gc, jint startX, jint startY, jint endX, jint endY, jint thickness, jint fade, DRAWING_Cap startCap, DRAWING_Cap endCap);

/*
 * @brief Implementation of drawThickFadedCircle over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_drawThickFadedCircle(MICROUI_GraphicsContext* gc, jint x, jint y, jint diameter, jint thickness, jint fade);

/*
 * @brief Implementation of drawThickFadedCircleArc over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_drawThickFadedCircleArc(MICROUI_GraphicsContext* gc, jint x, jint y, jint diameter, jfloat startAngle, jfloat arcAngle, jint thickness, jint fade, DRAWING_Cap start, DRAWING_Cap end);

/*
 * @brief Implementation of drawThickFadedEllipse over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_drawThickFadedEllipse(MICROUI_GraphicsContext* gc, jint x, jint y, jint width, jint height, jint thickness, jint fade);

/*
 * @brief Implementation of drawThickLine over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_drawThickLine(MICROUI_GraphicsContext* gc, jint startX, jint startY, jint endX, jint endY, jint thickness);

/*
 * @brief Implementation of drawThickCircle over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_drawThickCircle(MICROUI_GraphicsContext* gc, jint x, jint y, jint diameter, jint thickness);

/*
 * @brief Implementation of drawThickEllipse over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_drawThickEllipse(MICROUI_GraphicsContext* gc, jint x, jint y, jint width, jint height, jint thickness);

/*
 * @brief Implementation of drawThickCircleArc over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_drawThickCircleArc(MICROUI_GraphicsContext* gc, jint x, jint y, jint diameter, jfloat startAngle, jfloat arcAngle, jint thickness);

/*
 * @brief Implementation of drawFlippedImage over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_drawFlippedImage(MICROUI_GraphicsContext* gc, MICROUI_Image* img, jint regionX, jint regionY, jint width, jint height, jint x, jint y, DRAWING_Flip transformation, jint alpha);

/*
 * @brief Implementation of drawRotatedImageNearestNeighbor over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_drawRotatedImageNearestNeighbor(MICROUI_GraphicsContext* gc, MICROUI_Image* img, jint x, jint y, jint rotationX, jint rotationY, jfloat angle, jint alpha);

/*
 * @brief Implementation of drawRotatedImageBilinear over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_drawRotatedImageBilinear(MICROUI_GraphicsContext* gc, MICROUI_Image* img, jint x, jint y, jint rotationX, jint rotationY, jfloat angle, jint alpha);

/*
 * @brief Implementation of drawScaledImageNearestNeighbor over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_drawScaledImageNearestNeighbor(MICROUI_GraphicsContext* gc, MICROUI_Image* img, jint x, jint y, jfloat factorX, jfloat factorY, jint alpha);

/*
 * @brief Implementation of drawScaledImageBilinear over a BufferedVectorImage. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_BVI_drawScaledImageBilinear(MICROUI_GraphicsContext* gc, MICROUI_Image* img, jint x, jint y, jfloat factorX, jfloat factorY, jint alpha);

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------

#endif // #if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE

#ifdef __cplusplus
}
#endif
#endif // UI_DRAWING_BVI_H
