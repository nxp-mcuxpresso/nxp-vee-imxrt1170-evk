/* 
 * Copyright 2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#if !defined UI_DRAWING_VGLITE_PROCESS_H
#define UI_DRAWING_VGLITE_PROCESS_H
#ifdef __cplusplus
extern "C" {
#endif

/*
 * @file
 * @brief MicroEJ MicroUI library low level API: implementation over VG-Lite. Provides
 * a set of drawing functions over the VG-Lite library. First the MicroUI parameters
 * are converted to VG-Lite objects and then a drawing function is called to render the
 * drawing.
 * @author MicroEJ Developer Team
 * @version 7.1.0
 */

// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------

#include "ui_drawing.h"
#include "ui_vglite.h"

#include "vg_lite.h"

// --------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------

/*
 * @brief A fade higher than 1 is not compatible with VG-Lite
 */
#define UI_DRAWING_VGLITE_IS_COMPATIBLE_FADE(f) ((f) <= 1)

/*
 * @brief Gets the thickness according to a thickness (t) and a fade (f).
 */
#define UI_DRAWING_VGLITE_GET_THICKNESS(t, f) ((t) + (f))

// --------------------------------------------------------------------------------
// Typedef
// --------------------------------------------------------------------------------

/*
 * @brief Function to draw a path.
 *
 * @see vg_lite_draw()
 */
typedef DRAWING_Status (* UI_DRAWING_VGLITE_PROCESS_draw_path_t) (
		MICROUI_GraphicsContext* gc,
		vg_lite_path_t * path,
		vg_lite_fill_t fill_rule,
		vg_lite_matrix_t * matrix,
		vg_lite_blend_t blend,
		vg_lite_color_t color);

/*
 * @brief Function to blit a part of the source.
 *
 * @see vg_lite_blit_rect()
 */
typedef DRAWING_Status (* UI_DRAWING_VGLITE_PROCESS_blit_rect_t)(
		MICROUI_GraphicsContext* gc,
		vg_lite_buffer_t *source,
		uint32_t         *rect,
		vg_lite_matrix_t *matrix,
		vg_lite_blend_t   blend,
		vg_lite_color_t   color,
		vg_lite_filter_t  filter);

/*
 * @brief Function to clear the source.
 *
 * @see vg_lite_clear()
 */
typedef DRAWING_Status (* UI_DRAWING_VGLITE_PROCESS_clear_t)(
		MICROUI_GraphicsContext* gc,
		vg_lite_rectangle_t * rect,
		vg_lite_color_t color);

// --------------------------------------------------------------------------------
// Public API
// --------------------------------------------------------------------------------

/*
 * @brief Tells if the source is GPU compatible. If yes, configures the given matrix,
 * blit rectangle and color and returns a VG-Lite source buffer to draw a raster image.
 * Returns NULL if the image cannot be drawn (image format not supported by the GPU)
 * or if the image region to draw is fully outside the clip.
 *
 * @return a VG-Lite source buffer if the GPU can be used to draw the image.
 */
vg_lite_buffer_t* UI_DRAWING_VGLITE_PROCESS_prepare_draw_image(MICROUI_GraphicsContext* gc, MICROUI_Image* img, jint x_src, jint y_src, jint width, jint height, jint x_dest, jint y_dest, jint alpha, vg_lite_color_t* color, vg_lite_matrix_t* matrix, uint32_t* blit_rect);

/*
 * @brief Implementation of drawLine over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawLine(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer, MICROUI_GraphicsContext* gc, jint startX, jint startY, jint endX, jint endY);

/*
 * @brief Implementation of drawHorizontalLine over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawHorizontalLine(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer, MICROUI_GraphicsContext* gc, jint x1, jint x2, jint y);

/*
 * @brief Implementation of drawVerticalLine over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawVerticalLine(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer, MICROUI_GraphicsContext* gc, jint x, jint y1, jint y2);

/*
 * @brief Implementation of fillRectangle over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_fillRectangle(UI_DRAWING_VGLITE_PROCESS_clear_t drawer, MICROUI_GraphicsContext* gc, jint x1, jint y1, jint x2, jint y2);

/*
 * @brief Implementation of drawRoundedRectangle over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawRoundedRectangle(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer, MICROUI_GraphicsContext* gc, jint x, jint y, jint width, jint height, jint cornerEllipseWidth, jint cornerEllipseHeight);

/*
 * @brief Implementation of fillRoundedRectangle over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_fillRoundedRectangle(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer, MICROUI_GraphicsContext* gc, jint x, jint y, jint width, jint height, jint cornerEllipseWidth, jint cornerEllipseHeight);

/*
 * @brief Implementation of drawCircleArc over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawCircleArc(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer, MICROUI_GraphicsContext* gc, jint x, jint y, jint diameter, jfloat startAngle, jfloat arcAngle);

/*
 * @brief Implementation of drawEllipseArc over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawEllipseArc(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer, MICROUI_GraphicsContext* gc, jint x, jint y, jint width, jint height, jfloat startAngle, jfloat arcAngle);

/*
 * @brief Implementation of fillCircleArc over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_fillCircleArc(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer, MICROUI_GraphicsContext* gc, jint x, jint y, jint diameter, jfloat startAngle, jfloat arcAngle);

/*
 * @brief Implementation of fillEllipseArc over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_fillEllipseArc(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer, MICROUI_GraphicsContext* gc, jint x, jint y, jint width, jint height, jfloat startAngle, jfloat arcAngle);

/*
 * @brief Implementation of drawEllipse over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawEllipse(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer, MICROUI_GraphicsContext* gc, jint x, jint y, jint width, jint height);

/*
 * @brief Implementation of fillEllipse over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_fillEllipse(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer, MICROUI_GraphicsContext* gc, jint x, jint y, jint width, jint height);

/*
 * @brief Implementation of drawCircle over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawCircle(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer, MICROUI_GraphicsContext* gc, jint x, jint y, jint diameter);

/*
 * @brief Implementation of fillCircle over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_fillCircle(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer, MICROUI_GraphicsContext* gc, jint x, jint y, jint diameter);

/*
 * @brief Implementation of drawThickFadedPoint over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawThickFadedPoint(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer, MICROUI_GraphicsContext* gc, jint x, jint y, jint thickness);

/*
 * @brief Implementation of drawThickFadedLine over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawThickFadedLine(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer, MICROUI_GraphicsContext* gc, jint startX, jint startY, jint endX, jint endY, jint thickness, DRAWING_Cap startCap, DRAWING_Cap endCap);

/*
 * @brief Implementation of drawThickFadedCircle over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawThickFadedCircle(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer, MICROUI_GraphicsContext* gc, jint x, jint y, jint diameter, jint thickness);

/*
 * @brief Implementation of drawThickFadedCircleArc over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawThickFadedCircleArc(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer, MICROUI_GraphicsContext* gc, jint x, jint y, jint diameter, jfloat startAngle, jfloat arcAngle, jint thickness, DRAWING_Cap start, DRAWING_Cap end);

/*
 * @brief Implementation of drawThickFadedEllipse over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawThickFadedEllipse(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer, MICROUI_GraphicsContext* gc, jint x, jint y, jint width, jint height, jint thickness);

/*
 * @brief Implementation of drawThickLine over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawThickLine(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer, MICROUI_GraphicsContext* gc, jint startX, jint startY, jint endX, jint endY, jint thickness);

/*
 * @brief Implementation of drawThickCircle over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawThickCircle(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer, MICROUI_GraphicsContext* gc, jint x, jint y, jint diameter, jint thickness);

/*
 * @brief Implementation of drawThickEllipse over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawThickEllipse(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer, MICROUI_GraphicsContext* gc, jint x, jint y, jint width, jint height, jint thickness);

/*
 * @brief Implementation of drawThickCircleArc over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawThickCircleArc(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer, MICROUI_GraphicsContext* gc, jint x, jint y, jint diameter, jfloat startAngle, jfloat arcAngle, jint thickness);

/*
 * @brief Implementation of drawFlippedImage over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawFlippedImage(UI_DRAWING_VGLITE_PROCESS_blit_rect_t drawer, MICROUI_GraphicsContext* gc, MICROUI_Image* img, jint regionX, jint regionY, jint width, jint height, jint x, jint y, DRAWING_Flip transformation, jint alpha, bool* is_gpu_compatible);

/*
 * @brief Implementation of drawRotatedImageNearestNeighbor over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawRotatedImageNearestNeighbor(UI_DRAWING_VGLITE_PROCESS_blit_rect_t drawer, MICROUI_GraphicsContext* gc, MICROUI_Image* img, jint x, jint y, jint rotationX, jint rotationY, jfloat angle, jint alpha, bool* is_gpu_compatible);

/*
 * @brief Implementation of drawRotatedImageBilinear over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawRotatedImageBilinear(UI_DRAWING_VGLITE_PROCESS_blit_rect_t drawer, MICROUI_GraphicsContext* gc, MICROUI_Image* img, jint x, jint y, jint rotationX, jint rotationY, jfloat angle, jint alpha, bool* is_gpu_compatible);

/*
 * @brief Implementation of drawScaledImageNearestNeighbor over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawScaledImageNearestNeighbor(UI_DRAWING_VGLITE_PROCESS_blit_rect_t drawer, MICROUI_GraphicsContext* gc, MICROUI_Image* img, jint x, jint y, jfloat factorX, jfloat factorY, jint alpha, bool* is_gpu_compatible);

/*
 * @brief Implementation of drawScaledImageBilinear over VG-Lite. See ui_drawing.h
 */
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawScaledImageBilinear(UI_DRAWING_VGLITE_PROCESS_blit_rect_t drawer, MICROUI_GraphicsContext* gc, MICROUI_Image* img, jint x, jint y, jfloat factorX, jfloat factorY, jint alpha, bool* is_gpu_compatible);

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
#endif // UI_DRAWING_VGLITE_PROCESS_H
