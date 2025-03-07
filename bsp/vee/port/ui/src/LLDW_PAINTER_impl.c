/*
 * Copyright 2020-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief This file implements all "Drawing" (MicroUI extended library) drawing native functions.
 * @see LLDW_PAINTER_impl.h file comment
 * @author MicroEJ Developer Team
 * @version 14.2.0
 * @since MicroEJ UI Pack 13.0.0
 */

// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------

// implements LLDW_PAINTER_impl functions
#include <LLDW_PAINTER_impl.h>

// use graphical engine functions to synchronize drawings
#include <LLUI_DISPLAY.h>

// calls ui_drawing functions
#include "ui_drawing.h"

// logs the drawings
#include "ui_log.h"

// --------------------------------------------------------------------------------
// Macros and Defines
// --------------------------------------------------------------------------------

// macros to log a drawing
#define LOG_DRAW_START(fn) LLTRACE_record_event_u32(LLUI_EVENT_group, LLUI_EVENT_offset + UI_LOG_DRAW, \
													CONCAT_DEFINES(LOG_DRAW_, fn))
#define LOG_DRAW_END(s) LLTRACE_record_event_end_u32(LLUI_EVENT_group, LLUI_EVENT_offset + UI_LOG_DRAW, (s))

/*
 * LOG_DRAW_EVENT logs identifiers
 */
#define LOG_DRAW_drawThickFadedPoint 100
#define LOG_DRAW_drawThickFadedLine 101
#define LOG_DRAW_drawThickFadedCircle 102
#define LOG_DRAW_drawThickFadedCircleArc 103
#define LOG_DRAW_drawThickFadedEllipse 104
#define LOG_DRAW_drawThickLine 105
#define LOG_DRAW_drawThickCircle 106
#define LOG_DRAW_drawThickEllipse 107
#define LOG_DRAW_drawThickCircleArc 108

#define LOG_DRAW_drawFlippedImage 200
#define LOG_DRAW_drawRotatedImageNearestNeighbor 201
#define LOG_DRAW_drawRotatedImageBilinear 202
#define LOG_DRAW_drawScaledImageNearestNeighbor 203
#define LOG_DRAW_drawScaledImageBilinear 204
#define LOG_DRAW_drawScaledStringBilinear 205
#define LOG_DRAW_drawCharWithRotationBilinear 206
#define LOG_DRAW_drawCharWithRotationNearestNeighbor 207

// --------------------------------------------------------------------------------
// LLDW_PAINTER_impl.h functions
// --------------------------------------------------------------------------------

void LLDW_PAINTER_IMPL_drawThickFadedPoint(MICROUI_GraphicsContext *gc, jint x, jint y, jint thickness, jint fade) {
	if (((thickness > 0) || (fade > 0)) && LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) &
	                                                                   LLDW_PAINTER_IMPL_drawThickFadedPoint)) {
		LOG_DRAW_START(drawThickFadedPoint);
		DRAWING_Status status = UI_DRAWING_drawThickFadedPoint(gc, x, y, thickness, fade);
		LLUI_DISPLAY_setDrawingStatus(status);
		LOG_DRAW_END(status);
	}
}

void LLDW_PAINTER_IMPL_drawThickFadedLine(MICROUI_GraphicsContext *gc, jint startX, jint startY, jint endX, jint endY,
                                          jint thickness, jint fade, DRAWING_Cap startCap, DRAWING_Cap endCap) {
	if (((thickness > 0) || (fade > 0)) && LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) &
	                                                                   LLDW_PAINTER_IMPL_drawThickFadedLine)) {
		LOG_DRAW_START(drawThickFadedLine);
		DRAWING_Status status = UI_DRAWING_drawThickFadedLine(gc, startX, startY, endX, endY, thickness, fade, startCap,
		                                                      endCap);
		LLUI_DISPLAY_setDrawingStatus(status);
		LOG_DRAW_END(status);
	}
}

void LLDW_PAINTER_IMPL_drawThickFadedCircle(MICROUI_GraphicsContext *gc, jint x, jint y, jint diameter, jint thickness,
                                            jint fade) {
	if (((thickness > 0) || (fade > 0)) && (diameter > 0) && LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) &
	                                                                                     LLDW_PAINTER_IMPL_drawThickFadedCircle))
	{
		LOG_DRAW_START(drawThickFadedCircle);
		DRAWING_Status status = UI_DRAWING_drawThickFadedCircle(gc, x, y, diameter, thickness, fade);
		LLUI_DISPLAY_setDrawingStatus(status);
		LOG_DRAW_END(status);
	}
}

void LLDW_PAINTER_IMPL_drawThickFadedCircleArc(MICROUI_GraphicsContext *gc, jint x, jint y, jint diameter,
                                               jfloat startAngle, jfloat arcAngle, jint thickness, jint fade,
                                               DRAWING_Cap start, DRAWING_Cap end) {
	if (((thickness > 0) || (fade > 0)) && (diameter > 0) && ((int32_t)arcAngle != 0) && LLUI_DISPLAY_requestDrawing(gc,
																													 (
																														 SNI_callback)
	                                                                                                                 &
	                                                                                                                 LLDW_PAINTER_IMPL_drawThickFadedCircleArc))
	{
		LOG_DRAW_START(drawThickFadedCircleArc);
		DRAWING_Status status = UI_DRAWING_drawThickFadedCircleArc(gc, x, y, diameter, startAngle, arcAngle, thickness,
		                                                           fade, start, end);
		LLUI_DISPLAY_setDrawingStatus(status);
		LOG_DRAW_END(status);
	}
}

void LLDW_PAINTER_IMPL_drawThickFadedEllipse(MICROUI_GraphicsContext *gc, jint x, jint y, jint width, jint height,
                                             jint thickness, jint fade) {
	if (((thickness > 0) || (fade > 0)) && (width > 0) && (height > 0) && LLUI_DISPLAY_requestDrawing(gc,
	                                                                                                  (SNI_callback) &
	                                                                                                  LLDW_PAINTER_IMPL_drawThickFadedEllipse))
	{
		LOG_DRAW_START(drawThickFadedEllipse);
		DRAWING_Status status = UI_DRAWING_drawThickFadedEllipse(gc, x, y, width, height, thickness, fade);
		LLUI_DISPLAY_setDrawingStatus(status);
		LOG_DRAW_END(status);
	}
}

void LLDW_PAINTER_IMPL_drawThickLine(MICROUI_GraphicsContext *gc, jint startX, jint startY, jint endX, jint endY,
                                     jint thickness) {
	if ((thickness > 0) && LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) & LLDW_PAINTER_IMPL_drawThickLine)) {
		LOG_DRAW_START(drawThickLine);
		DRAWING_Status status = UI_DRAWING_drawThickLine(gc, startX, startY, endX, endY, thickness);
		LLUI_DISPLAY_setDrawingStatus(status);
		LOG_DRAW_END(status);
	}
}

void LLDW_PAINTER_IMPL_drawThickCircle(MICROUI_GraphicsContext *gc, jint x, jint y, jint diameter, jint thickness) {
	if ((thickness > 0) && (diameter > 0) && LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) &
	                                                                     LLDW_PAINTER_IMPL_drawThickCircle)) {
		LOG_DRAW_START(drawThickCircle);
		DRAWING_Status status = UI_DRAWING_drawThickCircle(gc, x, y, diameter, thickness);
		LLUI_DISPLAY_setDrawingStatus(status);
		LOG_DRAW_END(status);
	}
}

void LLDW_PAINTER_IMPL_drawThickEllipse(MICROUI_GraphicsContext *gc, jint x, jint y, jint width, jint height,
                                        jint thickness) {
	if ((thickness > 0) && (width > 0) && (height > 0) && LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) &
	                                                                                  LLDW_PAINTER_IMPL_drawThickEllipse))
	{
		LOG_DRAW_START(drawThickEllipse);
		DRAWING_Status status = UI_DRAWING_drawThickEllipse(gc, x, y, width, height, thickness);
		LLUI_DISPLAY_setDrawingStatus(status);
		LOG_DRAW_END(status);
	}
}

void LLDW_PAINTER_IMPL_drawThickCircleArc(MICROUI_GraphicsContext *gc, jint x, jint y, jint diameter, jfloat startAngle,
                                          jfloat arcAngle, jint thickness) {
	if ((thickness > 0) && (diameter > 0) && ((int32_t)arcAngle != 0) && LLUI_DISPLAY_requestDrawing(gc,
	                                                                                                 (SNI_callback) &
	                                                                                                 LLDW_PAINTER_IMPL_drawThickCircleArc))
	{
		LOG_DRAW_START(drawThickCircleArc);
		DRAWING_Status status = UI_DRAWING_drawThickCircleArc(gc, x, y, diameter, startAngle, arcAngle, thickness);
		LLUI_DISPLAY_setDrawingStatus(status);
		LOG_DRAW_END(status);
	}
}

void LLDW_PAINTER_IMPL_drawFlippedImage(MICROUI_GraphicsContext *gc, MICROUI_Image *img, jint regionX, jint regionY,
                                        jint width, jint height, jint x, jint y, DRAWING_Flip transformation,
                                        jint alpha) {
	if (!LLUI_DISPLAY_isImageClosed(img) && (alpha > 0)
	    && LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) & LLDW_PAINTER_IMPL_drawFlippedImage)) {
		LOG_DRAW_START(drawFlippedImage);
		DRAWING_Status status = UI_DRAWING_drawFlippedImage(gc, img, regionX, regionY, width, height, x, y,
		                                                    transformation, alpha);
		LLUI_DISPLAY_setDrawingStatus(status);
		LOG_DRAW_END(status);
	}
}

void LLDW_PAINTER_IMPL_drawRotatedImageNearestNeighbor(MICROUI_GraphicsContext *gc, MICROUI_Image *img, jint x, jint y,
                                                       jint rotationX, jint rotationY, jfloat angle, jint alpha) {
	if (!LLUI_DISPLAY_isImageClosed(img) && (alpha > 0)
	    && LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) & LLDW_PAINTER_IMPL_drawRotatedImageNearestNeighbor)) {
		LOG_DRAW_START(drawRotatedImageNearestNeighbor);
		DRAWING_Status status = UI_DRAWING_drawRotatedImageNearestNeighbor(gc, img, x, y, rotationX, rotationY, angle,
		                                                                   alpha);
		LLUI_DISPLAY_setDrawingStatus(status);
		LOG_DRAW_END(status);
	}
}

void LLDW_PAINTER_IMPL_drawRotatedImageBilinear(MICROUI_GraphicsContext *gc, MICROUI_Image *img, jint x, jint y,
                                                jint rotationX, jint rotationY, jfloat angle, jint alpha) {
	if (!LLUI_DISPLAY_isImageClosed(img) && (alpha > 0)
	    && LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) & LLDW_PAINTER_IMPL_drawRotatedImageBilinear)) {
		LOG_DRAW_START(drawRotatedImageBilinear);
		DRAWING_Status status = UI_DRAWING_drawRotatedImageBilinear(gc, img, x, y, rotationX, rotationY, angle, alpha);
		LLUI_DISPLAY_setDrawingStatus(status);
		LOG_DRAW_END(status);
	}
}

void LLDW_PAINTER_IMPL_drawScaledImageNearestNeighbor(MICROUI_GraphicsContext *gc, MICROUI_Image *img, jint x, jint y,
                                                      jfloat factorX, jfloat factorY, jint alpha) {
	if (!LLUI_DISPLAY_isImageClosed(img) && (alpha > 0) && (factorX > 0.f) && (factorY > 0.f)
	    && LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) & LLDW_PAINTER_IMPL_drawScaledImageNearestNeighbor)) {
		LOG_DRAW_START(drawScaledImageNearestNeighbor);
		DRAWING_Status status = UI_DRAWING_drawScaledImageNearestNeighbor(gc, img, x, y, factorX, factorY, alpha);
		LLUI_DISPLAY_setDrawingStatus(status);
		LOG_DRAW_END(status);
	}
}

void LLDW_PAINTER_IMPL_drawScaledImageBilinear(MICROUI_GraphicsContext *gc, MICROUI_Image *img, jint x, jint y,
                                               jfloat factorX, jfloat factorY, jint alpha) {
	if (!LLUI_DISPLAY_isImageClosed(img) && (alpha > 0) && (factorX > 0.f) && (factorY > 0.f)
	    && LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) & LLDW_PAINTER_IMPL_drawScaledImageBilinear)) {
		LOG_DRAW_START(drawScaledImageBilinear);
		DRAWING_Status status = UI_DRAWING_drawScaledImageBilinear(gc, img, x, y, factorX, factorY, alpha);
		LLUI_DISPLAY_setDrawingStatus(status);
		LOG_DRAW_END(status);
	}
}

void LLDW_PAINTER_IMPL_drawScaledStringBilinear(MICROUI_GraphicsContext *gc, jchar *chars, jint length,
                                                MICROUI_Font *font, jint x, jint y, jfloat xRatio, jfloat yRatio) {
	if ((length > 0) && (xRatio > 0) && (yRatio > 0)
	    && LLUI_DISPLAY_requestDrawing(gc, (SNI_callback)LLDW_PAINTER_IMPL_drawScaledStringBilinear)) {
		LOG_DRAW_START(drawScaledStringBilinear);
		DRAWING_Status status = UI_DRAWING_drawScaledStringBilinear(gc, chars, length, font, x, y, xRatio, yRatio);
		LLUI_DISPLAY_setDrawingStatus(status);
		LOG_DRAW_END(status);
	}
}

void LLDW_PAINTER_IMPL_drawScaledRenderableStringBilinear(MICROUI_GraphicsContext *gc, jchar *chars, jint length,
                                                          MICROUI_Font *font, jint width,
                                                          MICROUI_RenderableString *renderableString, jint x, jint y,
                                                          jfloat xRatio, jfloat yRatio) {
	if ((length > 0) && (xRatio > 0) && (yRatio > 0)
	    && LLUI_DISPLAY_requestDrawing(gc, (SNI_callback)LLDW_PAINTER_IMPL_drawScaledRenderableStringBilinear)) {
		LOG_DRAW_START(drawScaledStringBilinear);
		DRAWING_Status status = UI_DRAWING_drawScaledRenderableStringBilinear(gc, chars, length, font, width,
		                                                                      renderableString, x, y, xRatio, yRatio);
		LLUI_DISPLAY_setDrawingStatus(status);
		LOG_DRAW_END(status);
	}
}

void LLDW_PAINTER_IMPL_drawCharWithRotationBilinear(MICROUI_GraphicsContext *gc, jchar c, MICROUI_Font *font, jint x,
                                                    jint y, jint xRotation, jint yRotation, jfloat angle, jint alpha) {
	if ((alpha > 0) && LLUI_DISPLAY_requestDrawing(gc, (SNI_callback)LLDW_PAINTER_IMPL_drawCharWithRotationBilinear)) {
		LOG_DRAW_START(drawCharWithRotationBilinear);
		DRAWING_Status status = UI_DRAWING_drawCharWithRotationBilinear(gc, c, font, x, y, xRotation, yRotation, angle,
		                                                                alpha);
		LLUI_DISPLAY_setDrawingStatus(status);
		LOG_DRAW_END(status);
	}
}

void LLDW_PAINTER_IMPL_drawCharWithRotationNearestNeighbor(MICROUI_GraphicsContext *gc, jchar c, MICROUI_Font *font,
                                                           jint x, jint y, jint xRotation, jint yRotation, jfloat angle,
                                                           jint alpha) {
	if ((alpha > 0) &&
	    LLUI_DISPLAY_requestDrawing(gc, (SNI_callback)LLDW_PAINTER_IMPL_drawCharWithRotationNearestNeighbor)) {
		LOG_DRAW_START(drawCharWithRotationNearestNeighbor);
		DRAWING_Status status = UI_DRAWING_drawCharWithRotationNearestNeighbor(gc, c, font, x, y, xRotation, yRotation,
		                                                                       angle, alpha);
		LLUI_DISPLAY_setDrawingStatus(status);
		LOG_DRAW_END(status);
	}
}

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------
