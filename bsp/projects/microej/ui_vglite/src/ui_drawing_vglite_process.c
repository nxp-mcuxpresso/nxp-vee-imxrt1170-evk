/*
 * C
 *
 * Copyright 2019-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief MicroEJ MicroUI library low level API: implementation of ui_drawing_vglite_process.h.
 * @author MicroEJ Developer Team
 * @version 9.0.0
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <assert.h>

#include "ui_drawing_vglite_process.h"
#include "ui_drawing_vglite_path.h"

// -----------------------------------------------------------------------------
// Types
// -----------------------------------------------------------------------------

/*
 * @brief Variable to store the vglite pathes during the GPU computation.
 *
 * MicroUI design must ensure that only one computation is done at a time.
 */
static union {
	vglite_path_line_t line; // line
	vglite_path_thick_shape_line_t thick_shape_line; // thick shape line
	vglite_path_ellipse_t ellipse[2]; // ellipse (in & out)
	vglite_path_ellipse_arc_t ellipse_arc; // ellipse arc
	vglite_path_rounded_rectangle_t rounded_rectangle[2]; // round rect (in & out)
	vglite_path_thick_ellipse_arc_t thick_ellipse_arc; // anti aliased ellipse arc
} __shape_paths;

// -----------------------------------------------------------------------------
// Global Variables
// -----------------------------------------------------------------------------

/*
 * @brief Unique path used to render the shape (there is only one shape to render at any time).
 */
static vg_lite_path_t shape_vg_path;

/*
 * @brief Variable to target a vglite source buffer (there is only one source at any time)
 */
static vg_lite_buffer_t source_buffer;

// -----------------------------------------------------------------------------
// Internal function definitions
// -----------------------------------------------------------------------------

/*
 * @brief Draws an ellipse arc covering the square specified by its diameter.
 * See DRAWING_impl.h for more information
 *
 * @param[in] gc: the MicroUI GraphicsContext target.
 * @param[in] x: the x coordinate of the upper-left corner of the square where the ellipse arc is drawn
 * @param[in] y: the y coordinate of the upper-left corner of the square where the ellipse arc is drawn
 * @param[in] diameter_w: the horizontal diameter of the ellipse arc to draw
 * @param[in] diameter_h: the vertical diameter of the ellipse arc to draw
 * @param[in] start_angle_deg: the beginning angle of the arc to draw
 * @param[in] arc_angle_deg: the angular extent of the arc from start_angle_deg
 * @param[in] fill: the flag indicating if the ellipse arc will be filled
 * - true: the ellipse arc will be filled
 * - false: the ellipse arc will not be filled
 *
 * @return the drawing status.
 */
static DRAWING_Status __draw_ellipse_arc(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
                                         MICROUI_GraphicsContext *gc,
                                         int x,
                                         int y,
                                         int diameter_w,
                                         int diameter_h,
                                         float start_angle,
                                         float arc_angle,
                                         bool fill);

/*
 * @brief Draws a ellipse covering the square specified by its diameter.
 * See DRAWING_impl.h for more information
 *
 * @param[in] gc: the MicroUI GraphicsContext target.
 * @param[in] x: the x coordinate of the upper-left corner of the square where the ellipse is drawn
 * @param[in] y: the y coordinate of the upper-left corner of the square where the ellipse is drawn
 * @param[in] diameter_out_w: the horizontal outer diameter of the ellipse to draw
 * @param[in] diameter_out_h: the vertical outer diameter of the ellipse to draw
 * @param[in] diameter_in_w: the horizontal inner diameter of the ellipse to draw
 * @param[in] diameter_in_h: the vertical inner diameter of the ellipse to draw
 *
 * @return the drawing status.
 */
static DRAWING_Status __draw_ellipse(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
                                     MICROUI_GraphicsContext *gc,
                                     float x,
                                     float y,
                                     int diameter_out_w,
                                     int diameter_out_h,
                                     int diameter_in_w,
                                     int diameter_in_h);

/*
 * @brief Fills a ellipse covering the square specified by its diameter.
 * See DRAWING_impl.h for more information
 *
 * @param[in] gc: the MicroUI GraphicsContext target.
 * @param[in] x: the x coordinate of the upper-left corner of the square where the ellipse is drawn
 * @param[in] y: the y coordinate of the upper-left corner of the square where the ellipse is drawn
 * @param[in] diameter_w: the horizontal diameter of the ellipse to draw
 * @param[in] diameter_h: the vertical diameter of the ellipse to draw
 *
 * @return the drawing status.
 */
static DRAWING_Status __fill_ellipse(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
                                     MICROUI_GraphicsContext *gc,
                                     float x,
                                     float y,
                                     int diameter_w,
                                     int diameter_h);

/*
 * @brief Draws a line between two points
 * See DRAWING_impl.h for more information
 *
 * @param[in] gc the MicroUI GraphicsContext target.
 * @param[in] xs: the horizontal coordinate of the start of the line
 * @param[in] ys: the vertical coordinate of start of the line
 * @param[in] xe: the horizontal coordinate of the end of the line
 * @param[in] ye: the vertical coordinate of end of the line
 *
 * @return the drawing status.
 */
static DRAWING_Status __draw_line(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
                                  MICROUI_GraphicsContext *gc,
                                  int xs,
                                  int ys,
                                  int xe,
                                  int ye);

/*
 * @brief Draws a thick line between two points
 * See DRAWING_impl.h for more information
 *
 * @param[in] gc the MicroUI GraphicsContext target.
 * @param[in] xs: the horizontal coordinate of the start of the line
 * @param[in] ys: the vertical coordinate of start of the line
 * @param[in] xe: the horizontal coordinate of the end of the line
 * @param[in] ye: the vertical coordinate of end of the line
 * @param[in] thickness: the line thickness.
 * @param[in] start cap representation of start of the line
 * @param[in] end cap representation of end of the line
 *
 * @return the drawing status.
 */
static DRAWING_Status __thick_line(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
                                   MICROUI_GraphicsContext *gc,
                                   int xs,
                                   int ys,
                                   int xe,
                                   int ye,
                                   int thickness,
                                   DRAWING_Cap start,
                                   DRAWING_Cap end);

/*
 * @brief Draws a thick arc covering the square specified by its diameter.
 * See dw_drawing.h for more information
 *
 * @param[in] gc: the MicroUI GraphicsContext target.
 * @param[in] x: the x coordinate of the upper-left corner of the square where the arc is drawn
 * @param[in] y: the y coordinate of the upper-left corner of the square where the arc is drawn
 * @param[in] diameter_w: the horizontal diameter of the ellipse arc to draw
 * @param[in] diameter_h: the vertical diameter of the ellipse arc to draw
 * @param[in] start_angle_deg: the beginning angle of the arc to draw
 * @param[in] arc_angle_deg: the angular extent of the arc from start_angle_deg
 * @param[in] thickness: the arc thickness.
 * @param[in] start: cap representation of start of shape
 * @param[in] end: cap representation of end of shape
 *
 * @return the drawing status.
 */
static DRAWING_Status __draw_thick_shape_ellipse_arc(UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
                                                     MICROUI_GraphicsContext *gc,
                                                     int x,
                                                     int y,
                                                     int diameter_w,
                                                     int diameter_h,
                                                     float start_angle_deg,
                                                     float arc_angle,
                                                     int thickness,
                                                     DRAWING_Cap start,
                                                     DRAWING_Cap end);

/*
 * @brief Rotates an image using the GPU
 *
 * @param[in] gc: Graphics context where to draw
 * @param[in] img: Source image to draw
 * @param[in] src: Source buffer
 * @param[in] x: Horizontal coordinate of the image reference anchor top-left point.
 * @param[in] y: Vertical coordinate of the image reference anchor top-left point.
 * @param[in] xRotation: Horizontal coordinate of the rotation center
 * @param[in] yRotation: Vertical coordinate of the rotation center
 * @param[in] angle: Angle that must be used to rotate the image
 * @param[in] alpha: The opacity level to apply while drawing the rotated image
 * @param[in] filter: Quality of drawing.
 *
 * @return The drawing status.
 */
static DRAWING_Status __rotate_image(UI_DRAWING_VGLITE_PROCESS_blit_rect_t drawer,
                                     MICROUI_GraphicsContext *gc,
                                     const MICROUI_Image *img,
                                     vg_lite_buffer_t *src,
                                     int x,
                                     int y,
                                     int xRotation,
                                     int yRotation,
                                     float angle_deg,
                                     int alpha,
                                     vg_lite_filter_t filter);

/*
 * @brief Scales an image using the GPU
 *
 * @param[in] gc: Graphics context where to draw.
 * @param[in] img: Source image to draw.
 * @param[in] src: Source buffer
 * @param[in] x: Horizontal coordinate of the image reference anchor top-left point.
 * @param[in] y: Vertical coordinate of the image reference anchor top-left point.
 * @param[in] factorX: Horizontal scaling factor.
 * @param[in] yRotation: Vertical scaling factor.
 * @param[in] alpha: The opacity level to apply while drawing the image
 * @param[in] filter: Quality of drawing.
 *
 * @return The drawing status.
 */
static DRAWING_Status __scale_image(UI_DRAWING_VGLITE_PROCESS_blit_rect_t drawer,
                                    MICROUI_GraphicsContext *gc,
                                    const MICROUI_Image *img,
                                    vg_lite_buffer_t *src,
                                    int x,
                                    int y,
                                    float factorX,
                                    float factorY,
                                    int alpha,
                                    vg_lite_filter_t filter);

// -----------------------------------------------------------------------------
// Public functions
// -----------------------------------------------------------------------------

// See the section 'Internal function definitions' for the function documentation
vg_lite_buffer_t * UI_DRAWING_VGLITE_PROCESS_prepare_draw_image(MICROUI_GraphicsContext *gc, MICROUI_Image *img,
                                                                jint x_src, jint y_src, jint width, jint height,
                                                                jint x_dest, jint y_dest, jint alpha,
                                                                vg_lite_color_t *color, vg_lite_matrix_t *matrix,
                                                                uint32_t *blit_rect) {
	vg_lite_buffer_t *ret;

	if (UI_VGLITE_configure_source(&source_buffer, img) && UI_VGLITE_enable_vg_lite_scissor_region(gc, x_dest, y_dest,
	                                                                                               x_dest + width - 1,
	                                                                                               y_dest + height -
	                                                                                               1)) {
		*color = UI_VGLITE_get_vglite_color(gc, img, alpha);

		vg_lite_identity(matrix);
		matrix->m[0][2] = x_dest;
		matrix->m[1][2] = y_dest;

		blit_rect[0] = x_src;
		blit_rect[1] = y_src;
		blit_rect[2] = width;
		blit_rect[3] = height;

		ret = &source_buffer;
	} else {
		ret = NULL;
	}
	return ret;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawLine(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	jint x1, jint y1,
	jint x2, jint y2) {
	DRAWING_Status ret;

	// Check if there is something to draw and clip drawing limits
	// TODO try to crop region (x1 may lower than x2 and y1 may be lower than y2)
	if (UI_VGLITE_enable_vg_lite_scissor_region(
			gc,
			0, 0,
			gc->image.width, gc->image.height)) {
		ret = __draw_line(drawer, gc, x1, y1, x2, y2);
	} else {
		// nothing to draw
		ret = DRAWING_DONE;
	}

	return ret;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawHorizontalLine(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	jint x1, jint x2, jint y) {
	DRAWING_Status ret;

	// Check if there is something to draw and clip drawing limits
	if (UI_VGLITE_enable_vg_lite_scissor_region(gc, x1, y, x2, y)) {
		ret = __draw_line(drawer, gc, x1, y, x2, y);
	} else {
		// nothing to draw
		ret = DRAWING_DONE;
	}

	return ret;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawVerticalLine(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	jint x, jint y1, jint y2) {
	DRAWING_Status ret;

	// Check if there is something to draw and clip drawing limits
	if (UI_VGLITE_enable_vg_lite_scissor_region(gc, x, y1, x, y2)) {
		ret = __draw_line(drawer, gc, x, y1, x, y2);
	} else {
		// nothing to draw
		ret = DRAWING_DONE;
	}

	return ret;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_fillRectangle(
	UI_DRAWING_VGLITE_PROCESS_clear_t drawer,
	MICROUI_GraphicsContext *gc,
	jint x1, jint y1, jint x2, jint y2) {
	DRAWING_Status ret = DRAWING_DONE;

	// Check if there is something to draw and clip drawing limits
	if (UI_VGLITE_enable_vg_lite_scissor_region(gc, x1, y1, x2, y2)) {
		vg_lite_rectangle_t shape_vg_rectangle;
		shape_vg_rectangle.x = x1;
		shape_vg_rectangle.y = y1;
		shape_vg_rectangle.width = (x2 - x1 + 1);
		shape_vg_rectangle.height = (y2 - y1 + 1);

		ret = (*drawer)(gc, &shape_vg_rectangle, gc->foreground_color);
	}

	return ret;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawRoundedRectangle(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	jint x, jint y,
	jint width, jint height,
	jint arc_width, jint arc_height) {
	vg_lite_matrix_t matrix;
	DRAWING_Status ret;

	// Check if there is something to draw and clip drawing limits
	if (UI_VGLITE_enable_vg_lite_scissor_region(gc, x, y, x + width, y + height)) {
		jint l_arc_width = (arc_width > width) ? width : arc_width;
		jint l_arc_height = (arc_height > height) ? height : arc_height;

		vg_lite_identity(&matrix);
		// Compute the rounded rectangle shape path
		shape_vg_path.path = &__shape_paths.rounded_rectangle[0];
		shape_vg_path.path_length = sizeof(__shape_paths.rounded_rectangle);
		int path_offset = 0;
		path_offset = UI_DRAWING_VGLITE_PATH_compute_rounded_rectangle(
			&shape_vg_path, path_offset,
			x, y, width, height, l_arc_width, l_arc_height, false);

		path_offset = UI_DRAWING_VGLITE_PATH_compute_rounded_rectangle(
			&shape_vg_path, path_offset,
			x + 1, y + 1,
			width - 2, height - 2,
			l_arc_width - 1, l_arc_height - 1,
			true);

		if (0 > path_offset) {
			UI_VGLITE_IMPL_error(false, "Error during path computation");
			LLUI_DISPLAY_reportError(gc, DRAWING_LOG_LIBRARY_INCIDENT);
			ret = DRAWING_DONE;
		} else {
			// Draw the point with the GPU
			ret = (*drawer)(
				gc,
				&shape_vg_path,
				VG_LITE_FILL_EVEN_ODD,
				&matrix,
				VG_LITE_BLEND_SRC_OVER,
				gc->foreground_color
				);
		}
	} else {
		ret = DRAWING_DONE;
	}

	return ret;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_fillRoundedRectangle(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	jint x, jint y,
	jint width, jint height,
	jint arc_width, jint arc_height) {
	vg_lite_matrix_t matrix;
	DRAWING_Status ret;

	// Check if there is something to draw and clip drawing limits
	if (UI_VGLITE_enable_vg_lite_scissor_region(gc, x, y, x + width, y + height)) {
		jint l_arc_width = (arc_width > width) ? width : arc_width;
		jint l_arc_height = (arc_height > height) ? height : arc_height;

		vg_lite_identity(&matrix);
		// Compute the rounded rectangle shape path
		shape_vg_path.path = &__shape_paths.rounded_rectangle[0];
		shape_vg_path.path_length = sizeof(__shape_paths.rounded_rectangle);
		int path_offset = UI_DRAWING_VGLITE_PATH_compute_rounded_rectangle(&shape_vg_path, 0,
		                                                                   x, y, width, height, l_arc_width,
		                                                                   l_arc_height, true);

		if (0 > path_offset) {
			UI_VGLITE_IMPL_error(false, "Error during path computation");
			LLUI_DISPLAY_reportError(gc, DRAWING_LOG_LIBRARY_INCIDENT);
			ret = DRAWING_DONE;
		} else {
			// Draw the point with the GPU
			ret = (*drawer)(
				gc,
				&shape_vg_path,
				VG_LITE_FILL_EVEN_ODD,
				&matrix,
				VG_LITE_BLEND_SRC_OVER,
				gc->foreground_color
				);
		}
	} else {
		ret = DRAWING_DONE;
	}

	return ret;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawCircle(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc, jint x, jint y, jint diameter) {
	DRAWING_Status ret;

	ret = __draw_ellipse(
		drawer,
		gc,
		x, y,
		diameter, diameter,
		diameter - 2, diameter - 2);

	return ret;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawCircleArc(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	jint x,
	jint y,
	jint diameter,
	jfloat start_angle,
	jfloat arc_angle) {
	DRAWING_Status ret;

	if (MEJ_ABS(arc_angle) >= 360) {
		ret = UI_DRAWING_VGLITE_PROCESS_drawCircle(drawer, gc, x, y, diameter);
	} else {
		ret = __draw_ellipse_arc(
			drawer,
			gc,
			x - 1, y - 1,
			diameter, diameter,
			start_angle, arc_angle,
			false);
	}
	return ret;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_fillEllipse(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	jint x, jint y,
	jint width, jint height) {
	DRAWING_Status ret;

	ret = __fill_ellipse(drawer, gc, x, y, width, height);

	return ret;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawEllipse(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	jint x, jint y,
	jint width, jint height) {
	DRAWING_Status ret;

	ret = __draw_ellipse(drawer, gc, x, y, width, height, width - 2, height - 2);

	return ret;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_fillCircle(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc, jint x, jint y, jint diameter) {
	DRAWING_Status ret;

	ret = __fill_ellipse(drawer, gc, x, y, diameter, diameter);

	return ret;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawEllipseArc(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	jint x,
	jint y,
	jint width,
	jint height,
	jfloat start_angle,
	jfloat arc_angle) {
	DRAWING_Status ret;

	if (MEJ_ABS(arc_angle) >= 360) {
		ret = UI_DRAWING_VGLITE_PROCESS_drawEllipse(drawer, gc, x, y, width, height);
	} else {
		ret = __draw_ellipse_arc(
			drawer,
			gc,
			x - 1, y - 1,
			width, height,
			start_angle, arc_angle,
			false);
	}

	return ret;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_fillCircleArc(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	jint x, jint y,
	jint diameter,
	jfloat start_angle, jfloat arc_angle) {
	DRAWING_Status ret;

	if (MEJ_ABS(arc_angle) >= 360) {
		ret = UI_DRAWING_VGLITE_PROCESS_fillCircle(drawer, gc, x, y, diameter);
	} else {
		ret = __draw_ellipse_arc(
			drawer,
			gc,
			x, y,
			diameter, diameter,
			start_angle, arc_angle,
			true);
	}

	return ret;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_fillEllipseArc(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	jint x, jint y,
	jint width, jint height,
	jfloat start_angle, jfloat arc_angle) {
	DRAWING_Status ret;

	if (MEJ_ABS(arc_angle) >= 360) {
		ret = UI_DRAWING_VGLITE_PROCESS_fillEllipse(
			drawer,
			gc,
			x, y,
			width, height);
	} else {
		ret = __draw_ellipse_arc(
			drawer,
			gc,
			x, y,
			width, height,
			start_angle, arc_angle,
			true);
	}

	return ret;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawThickFadedPoint(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	jint x,
	jint y,
	jint thickness) {
	float dx = x + 0.5f;
	float dy = y + 0.5f;
	float ft = thickness;
	ft /= 2.f;
	return __fill_ellipse(drawer, gc, dx - ft, dy - ft, thickness, thickness);
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawThickFadedLine(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	jint x1,
	jint y1,
	jint x2,
	jint y2,
	jint thickness,
	DRAWING_Cap start,
	DRAWING_Cap end) {
	return __thick_line(
		drawer,
		gc,
		x1,
		y1,
		x2,
		y2,
		thickness,
		start,
		end);
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawThickFadedCircle(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	jint x, jint y,
	jint diameter, jint thickness) {
	int diameter_out;
	int diameter_in;

	diameter_out = diameter + thickness;
	diameter_in = diameter - thickness;
	float dx = x;
	float dy = y;
	float ft = thickness;
	ft /= 2.f;

	return __draw_ellipse(
		drawer,
		gc,
		dx - ft, dy - ft,
		diameter_out, diameter_out,
		diameter_in, diameter_in);
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawThickFadedCircleArc(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	jint x,
	jint y,
	jint diameter,
	jfloat start_angle_deg,
	jfloat arc_angle,
	jint thickness,
	DRAWING_Cap start,
	DRAWING_Cap end) {
	return __draw_thick_shape_ellipse_arc(
		drawer,
		gc,
		x, y,
		diameter, diameter,
		start_angle_deg, arc_angle,
		thickness,
		start, end);
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawThickFadedEllipse(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	jint x, jint y,
	jint width, jint height,
	jint thickness) {
	int diameter_out_w;
	int diameter_out_h;
	int diameter_in_w;
	int diameter_in_h;

	diameter_out_w = width + thickness;
	diameter_out_h = height + thickness;
	diameter_in_w = width - thickness;
	diameter_in_h = height - thickness;

	float dx = x;
	float dy = y;
	float ft = thickness;
	ft /= 2.f;

	return __draw_ellipse(
		drawer,
		gc,
		dx - ft, dy - ft,
		diameter_out_w, diameter_out_h,
		diameter_in_w, diameter_in_h);
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawThickLine(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	jint x1,
	jint y1,
	jint x2,
	jint y2,
	jint thickness) {
	DRAWING_Status ret;

	ret = __thick_line(
		drawer,
		gc,
		x1,
		y1,
		x2,
		y2,
		thickness,
		DRAWING_ENDOFLINE_NONE,
		DRAWING_ENDOFLINE_NONE);
	return ret;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawThickCircle(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	jint x,
	jint y,
	jint diameter,
	jint thickness) {
	int diameter_out;
	int diameter_in;
	DRAWING_Status ret;

	diameter_out = diameter + thickness;
	diameter_in = diameter - thickness;
	float dx = x;
	float dy = y;
	float ft = thickness;
	ft /= 2.f;

	ret = __draw_ellipse(
		drawer,
		gc,
		dx - ft, dy - ft,
		diameter_out, diameter_out,
		diameter_in, diameter_in);

	return ret;
}

DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawThickEllipse(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	jint x, jint y,
	jint width, jint height,
	jint thickness) {
	int diameter_out_w;
	int diameter_out_h;
	int diameter_in_w;
	int diameter_in_h;
	DRAWING_Status ret;

	diameter_out_w = width + thickness;
	diameter_out_h = height + thickness;
	diameter_in_w = width - thickness;
	diameter_in_h = height - thickness;

	float dx = x;
	float dy = y;
	float ft = thickness;
	ft /= 2.f;

	ret = __draw_ellipse(
		drawer,
		gc,
		dx - ft, dy - ft,
		diameter_out_w, diameter_out_h,
		diameter_in_w, diameter_in_h);

	return ret;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawThickCircleArc(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	jint x,
	jint y,
	jint diameter,
	jfloat start_angle_deg,
	jfloat arc_angle,
	jint thickness) {
	DRAWING_Status ret;

	ret = __draw_thick_shape_ellipse_arc(
		drawer,
		gc,
		x, y,
		diameter, diameter,
		start_angle_deg, arc_angle,
		thickness,
		DRAWING_ENDOFLINE_NONE,
		DRAWING_ENDOFLINE_NONE);
	return ret;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawFlippedImage(
	UI_DRAWING_VGLITE_PROCESS_blit_rect_t drawer,
	MICROUI_GraphicsContext *gc,
	MICROUI_Image *img,
	jint region_x,
	jint region_y,
	jint width,
	jint height,
	jint x,
	jint y,
	DRAWING_Flip transformation,
	jint alpha, bool *is_gpu_compatible) {
	// the next algorithm does not support another origin than (0,0) but MicroUI never uses this condition
	assert(0 == region_x);
	assert(0 == region_y);

	DRAWING_Status ret;
	*is_gpu_compatible = UI_VGLITE_configure_source(&source_buffer, img);

	if (*is_gpu_compatible) {
		uint32_t dest_width = 0u;
		uint32_t dest_height = 0u;

		switch (transformation) {
		case DRAWING_FLIP_NONE:
		case DRAWING_FLIP_180:
		case DRAWING_FLIP_MIRROR:
		case DRAWING_FLIP_MIRROR_180:
			dest_width = img->width;
			dest_height = img->height;
			break;

		case DRAWING_FLIP_90:
		case DRAWING_FLIP_270:
		case DRAWING_FLIP_MIRROR_90:
		case DRAWING_FLIP_MIRROR_270:
			dest_width = img->height;
			dest_height = img->width;
			break;

		default:
			UI_VGLITE_IMPL_error(false, "Unexpected transformation: %d\n", transformation);
			break;
		}

		// Check if there is something to draw and clip drawing limits
		if ((dest_width > 0u) && (dest_height > 0u) && UI_VGLITE_enable_vg_lite_scissor_region(
				gc,
				x, y,
				(x + dest_width - 1), (y + dest_height - 1))) {
			// Exclude bottom right region
			source_buffer.width = width;
			source_buffer.height = height;

			vg_lite_matrix_t matrix;
			vg_lite_identity(&matrix);
			vg_lite_translate(x + (dest_width / 2u), y + (dest_height / 2u), &matrix);

			switch (transformation) {
			default:
			case DRAWING_FLIP_NONE:
				// nothing to do
				break;

			case DRAWING_FLIP_90:
				vg_lite_rotate(-90, &matrix);
				break;

			case DRAWING_FLIP_180:
				vg_lite_rotate(-180, &matrix);
				break;

			case DRAWING_FLIP_270:
				vg_lite_rotate(-270, &matrix);
				break;

			case DRAWING_FLIP_MIRROR:
				vg_lite_scale(-1, 1, &matrix);
				break;

			case DRAWING_FLIP_MIRROR_90:
				vg_lite_rotate(90, &matrix);
				vg_lite_scale(1, -1, &matrix);
				break;

			case DRAWING_FLIP_MIRROR_180:
				vg_lite_rotate(180, &matrix);
				vg_lite_scale(-1, 1, &matrix);
				break;

			case DRAWING_FLIP_MIRROR_270:
				vg_lite_rotate(270, &matrix);
				vg_lite_scale(1, -1, &matrix);
				break;
			}

			vg_lite_translate(-img->width / 2, -img->height / 2, &matrix);

			uint32_t blit_rect[4];
			blit_rect[0] = 0;
			blit_rect[1] = 0;
			blit_rect[2] = img->width;
			blit_rect[3] = img->height;

			ret = (*drawer)(
				gc,
				&source_buffer,
				blit_rect,
				&matrix, VG_LITE_BLEND_SRC_OVER,
				UI_VGLITE_get_vglite_color(gc, img, alpha),
				VG_LITE_FILTER_BI_LINEAR);
		} else {
			// no error: the drawing is just "out of clip"
			ret = DRAWING_DONE;
		}
	} else {
		// no error: the drawing is just "not performed"
		ret = DRAWING_DONE;
	}

	return ret;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawRotatedImageNearestNeighbor(
	UI_DRAWING_VGLITE_PROCESS_blit_rect_t drawer,
	MICROUI_GraphicsContext *gc,
	MICROUI_Image *img,
	jint x,
	jint y,
	jint xRotation,
	jint yRotation,
	jfloat angle,
	jint alpha, bool *is_gpu_compatible) {
	DRAWING_Status ret;
	*is_gpu_compatible = UI_VGLITE_configure_source(&source_buffer, img);

	if (*is_gpu_compatible) {
		// The GCNanoLiteV limitation: Same rendering around 90 degrees (89.0, 89.1 ... 90.9, 91)
		// Render is acceptable since vglite 3.0.4-rev4
		ret = __rotate_image(drawer, gc, img, &source_buffer, x, y, xRotation, yRotation, angle, alpha,
		                     VG_LITE_FILTER_POINT);
	} else {
		// no error: the drawing is just "not performed"
		ret = DRAWING_DONE;
	}

	return ret;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawRotatedImageBilinear(
	UI_DRAWING_VGLITE_PROCESS_blit_rect_t drawer,
	MICROUI_GraphicsContext *gc,
	MICROUI_Image *img,
	jint x,
	jint y,
	jint xRotation,
	jint yRotation,
	jfloat angle,
	jint alpha, bool *is_gpu_compatible) {
	DRAWING_Status ret;
	*is_gpu_compatible = UI_VGLITE_configure_source(&source_buffer, img);

	if (*is_gpu_compatible) {
		// The GCNanoLiteV limitation: Same rendering around 90 degrees (89.0, 89.1 ... 90.9, 91)
		// Render is acceptable since vglite 3.0.4-rev4
		ret = __rotate_image(drawer, gc, img, &source_buffer, x, y, xRotation, yRotation, angle, alpha,
		                     VG_LITE_FILTER_BI_LINEAR);
	} else {
		// no error: the drawing is just "not performed"
		ret = DRAWING_DONE;
	}

	return ret;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawScaledImageNearestNeighbor(
	UI_DRAWING_VGLITE_PROCESS_blit_rect_t drawer,
	MICROUI_GraphicsContext *gc,
	MICROUI_Image *img,
	jint x,
	jint y,
	jfloat factorX,
	jfloat factorY,
	jint alpha, bool *is_gpu_compatible) {
	DRAWING_Status ret;
	*is_gpu_compatible = UI_VGLITE_configure_source(&source_buffer, img);

	if (*is_gpu_compatible) {
		ret = __scale_image(drawer, gc, img, &source_buffer, x, y, factorX, factorY, alpha, VG_LITE_FILTER_POINT);
	} else {
		// no error: the drawing is just "not performed"
		ret = DRAWING_DONE;
	}

	return ret;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_VGLITE_PROCESS_drawScaledImageBilinear(
	UI_DRAWING_VGLITE_PROCESS_blit_rect_t drawer,
	MICROUI_GraphicsContext *gc,
	MICROUI_Image *img,
	jint x,
	jint y,
	jfloat factorX,
	jfloat factorY,
	jint alpha, bool *is_gpu_compatible) {
	DRAWING_Status ret;
	*is_gpu_compatible = UI_VGLITE_configure_source(&source_buffer, img);

	if (*is_gpu_compatible) {
		// when the factor is 1x1, the filter "point" draws something closer than draw_image
		vg_lite_filter_t filter = ((1.0f == factorX) &&
		                           (1.0f == factorY)) ? VG_LITE_FILTER_POINT : VG_LITE_FILTER_BI_LINEAR;
		ret = __scale_image(drawer, gc, img, &source_buffer, x, y, factorX, factorY, alpha, filter);
	} else {
		// no error: the drawing is just "not performed"
		ret = DRAWING_DONE;
	}

	return ret;
}

// -----------------------------------------------------------------------------
// Internal functions
// -----------------------------------------------------------------------------

// See the section 'Internal function definitions' for the function documentation
static DRAWING_Status __draw_ellipse_arc(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	int x,
	int y,
	int diameter_w,
	int diameter_h,
	float start_angle_deg,
	float arc_angle_deg,
	bool fill) {
	vg_lite_matrix_t matrix;
	DRAWING_Status ret;

	// Check if there is something to draw and clip drawing limits
	// +/- 1: consider 1 pixel of antialiasing
	if (UI_VGLITE_enable_vg_lite_scissor_region(gc, x - 1, y - 1, x + diameter_w, y + diameter_h)) {
		vg_lite_identity(&matrix);

		float radius_out_w = (float)diameter_w;
		float radius_out_h = (float)diameter_h;
		radius_out_w /= 2.f;
		radius_out_h /= 2.f;
		float radius_in_w = radius_out_w - 1.f;
		float radius_in_h = radius_out_h - 1.f;

		vg_lite_translate((float)x + radius_out_w, (float)y + radius_out_h, &matrix);

		radius_out_w *= DRAWING_SCALE_FACTOR;
		radius_out_h *= DRAWING_SCALE_FACTOR;
		radius_in_w *= DRAWING_SCALE_FACTOR;
		radius_in_h *= DRAWING_SCALE_FACTOR;

		vg_lite_scale(DRAWING_SCALE_DIV, DRAWING_SCALE_DIV, &matrix);

		shape_vg_path.path = &__shape_paths.ellipse_arc;
		shape_vg_path.path_length = sizeof(__shape_paths.ellipse_arc);
		int path_offset = UI_DRAWING_VGLITE_PATH_compute_ellipse_arc(
			&shape_vg_path,
			(int)radius_out_w, (int)radius_out_h,
			(int)radius_in_w, (int)radius_in_h,

			// 0 degrees:
			// - MicroUI: 3 o'clock.
			// - RT595 VGLite implementation : 0 o'clock.
			90.f - start_angle_deg,

			-arc_angle_deg, fill);

		if (0 > path_offset) {
			UI_VGLITE_IMPL_error(false, "Error during path computation");
			LLUI_DISPLAY_reportError(gc, DRAWING_LOG_LIBRARY_INCIDENT);
			ret = DRAWING_DONE;
		} else {
			// Draw the point with the GPU
			ret = (*drawer)(
				gc,
				&shape_vg_path,
				VG_LITE_FILL_EVEN_ODD,
				&matrix,
				VG_LITE_BLEND_SRC_OVER,
				gc->foreground_color
				);
		}
	} else {
		ret = DRAWING_DONE;
	}
	return ret;
}

// See the section 'Internal function definitions' for the function documentation
static DRAWING_Status __draw_ellipse(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	float x, float y,
	int diameter_out_w, int diameter_out_h,
	int diameter_in_w, int diameter_in_h) {
	vg_lite_matrix_t matrix;
	DRAWING_Status ret;

	// Check if there is something to draw and clip drawing limits
	// +/- 1: consider 1 pixel of antialiasing
	if (UI_VGLITE_enable_vg_lite_scissor_region(gc, (int)(x - 1.0f), (int)(y - 1.0f), (int)(x + (float)diameter_out_w),
	                                            (int)(y + (float)diameter_out_h))) {
		vg_lite_identity(&matrix);

		float radius_w = (float)diameter_out_w;
		float radius_h = (float)diameter_out_h;

		radius_w /= 2.f;
		radius_h /= 2.f;

		vg_lite_translate(x + radius_w, y + radius_h, &matrix);

		radius_w *= DRAWING_SCALE_FACTOR;
		radius_h *= DRAWING_SCALE_FACTOR;

		vg_lite_scale(DRAWING_SCALE_DIV, DRAWING_SCALE_DIV, &matrix);
		// Compute the rounded rectangle shape path
		shape_vg_path.path = &__shape_paths.ellipse[0];
		shape_vg_path.path_length = sizeof(__shape_paths.ellipse);
		int path_offset = UI_DRAWING_VGLITE_PATH_compute_ellipse(
			&shape_vg_path, 0,
			(int)radius_w, (int)radius_h,
			false);

		radius_w = (float)diameter_in_w;
		radius_h = (float)diameter_in_h;

		radius_w /= 2.f;
		radius_h /= 2.f;

		radius_w *= DRAWING_SCALE_FACTOR;
		radius_h *= DRAWING_SCALE_FACTOR;

		path_offset = UI_DRAWING_VGLITE_PATH_compute_ellipse(
			&shape_vg_path,
			path_offset,
			(int)radius_w, (int)radius_h,
			true);

		if (0 > path_offset) {
			UI_VGLITE_IMPL_error(false, "Error during path computation");
			LLUI_DISPLAY_reportError(gc, DRAWING_LOG_LIBRARY_INCIDENT);
			ret = DRAWING_DONE;
		} else {
			// Draw the point with the GPU
			ret = (*drawer)(
				gc,
				&shape_vg_path,
				VG_LITE_FILL_EVEN_ODD,
				&matrix,
				VG_LITE_BLEND_SRC_OVER,
				gc->foreground_color
				);
		}
	} else {
		ret = DRAWING_DONE;
	}
	return ret;
}

// See the section 'Internal function definitions' for the function documentation
static DRAWING_Status __fill_ellipse(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	float x, float y,
	int diameter_w, int diameter_h) {
	vg_lite_matrix_t matrix;
	DRAWING_Status ret;

	// Check if there is something to draw and clip drawing limits
	// +/- 1: consider 1 pixel of antialiasing
	if (UI_VGLITE_enable_vg_lite_scissor_region(gc,
	                                            (int)(x - 1.0f),
	                                            (int)(y - 1.0f),
	                                            (int)(x + (float)diameter_w),
	                                            (int)(y + (float)diameter_h))) {
		float radius_w = (float)diameter_w;
		float radius_h = (float)diameter_h;
		radius_w /= 2.f;
		radius_h /= 2.f;

		vg_lite_identity(&matrix);
		vg_lite_translate(x + radius_w, y + radius_h, &matrix);

		radius_w *= DRAWING_SCALE_FACTOR;
		radius_h *= DRAWING_SCALE_FACTOR;

		vg_lite_scale(DRAWING_SCALE_DIV, DRAWING_SCALE_DIV, &matrix);

		// Compute the thick shape path
		shape_vg_path.path = &__shape_paths.ellipse;
		shape_vg_path.path_length = sizeof(__shape_paths.ellipse);
		(void)UI_DRAWING_VGLITE_PATH_compute_filled_ellipse(&shape_vg_path, (int)radius_w, (int)radius_h, &matrix);

		// Draw the point with the GPU
		ret = (*drawer)(
			gc,
			&shape_vg_path,
			VG_LITE_FILL_NON_ZERO,
			&matrix,
			VG_LITE_BLEND_SRC_OVER,
			gc->foreground_color
			);
	} else {
		ret = DRAWING_DONE;
	}
	return ret;
}

// See the section 'Internal function definitions' for the function documentation
static DRAWING_Status __draw_line(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	int xs, int ys,
	int xe, int ye) {
	// Translate to the line starting point
	vg_lite_matrix_t matrix;
	vg_lite_identity(&matrix);
	vg_lite_translate(xs, ys, &matrix);

	// Compute the thick shape path
	shape_vg_path.path = &__shape_paths.line;
	shape_vg_path.path_length = sizeof(__shape_paths.line);
	(void)UI_DRAWING_VGLITE_PATH_compute_line(&shape_vg_path, xe - xs, ye - ys);

	// Draw the line with the GPU
	return (*drawer)(
		gc,
		&shape_vg_path,
		VG_LITE_FILL_NON_ZERO,
		&matrix,
		VG_LITE_BLEND_SRC_OVER,
		gc->foreground_color
		);
}

// See the section 'Internal function definitions' for the function documentation
static DRAWING_Status __thick_line(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	int xs,
	int ys,
	int xe,
	int ye,
	int thickness,
	DRAWING_Cap start,
	DRAWING_Cap end) {
	vg_lite_matrix_t matrix;
	DRAWING_Status ret;

	int min_x = MEJ_MIN(xs, xe);
	int min_y = MEJ_MIN(ys, ye);
	int max_x = MEJ_MAX(xs, xe);
	int max_y = MEJ_MAX(ys, ye);

	// Check if there is something to draw and clip drawing limits
	if (UI_VGLITE_enable_vg_lite_scissor_region(
			gc,
			min_x - (thickness / 2),
			min_y - (thickness / 2),
			max_x + (thickness / 2),
			max_y + (thickness / 2))) {
		// Translate to the line starting point
		vg_lite_identity(&matrix);
		vg_lite_translate(xs, ys, &matrix);

		vg_lite_scale(DRAWING_SCALE_DIV, DRAWING_SCALE_DIV, &matrix);

		shape_vg_path.path = &__shape_paths.thick_shape_line;
		shape_vg_path.path_length = sizeof(__shape_paths.thick_shape_line);

		int caps = 0;
		caps |= MEJ_VGLITE_PATH_SET_CAPS_START(start);
		caps |= MEJ_VGLITE_PATH_SET_CAPS_END(end);

		(void)UI_DRAWING_VGLITE_PATH_compute_thick_shape_line(
			&shape_vg_path,
			(int)(DRAWING_SCALE_FACTOR * (xe - xs)),
			(int)(DRAWING_SCALE_FACTOR * (ye - ys)),
			(int)(DRAWING_SCALE_FACTOR * thickness),
			caps,
			&matrix);

		ret = (*drawer)(
			gc,
			&shape_vg_path,
			VG_LITE_FILL_NON_ZERO,
			&matrix,
			VG_LITE_BLEND_SRC_OVER,
			gc->foreground_color);
	} else {
		ret = DRAWING_DONE;
	}
	return ret;
}

// See the section 'Internal function definitions' for the function documentation
static DRAWING_Status __draw_thick_shape_ellipse_arc(
	UI_DRAWING_VGLITE_PROCESS_draw_path_t drawer,
	MICROUI_GraphicsContext *gc,
	int x,
	int y,
	int diameter_w,
	int diameter_h,
	float start_angle_deg,
	float arc_angle,
	int thickness,
	DRAWING_Cap start,
	DRAWING_Cap end) {
	DRAWING_Status ret;

	// Check if there is something to draw and clip drawing limits
	if (UI_VGLITE_enable_vg_lite_scissor_region(
			gc,
			x - thickness,
			y - thickness,
			x + diameter_w + (2 * thickness) - 1,
			y + diameter_h + (2 * thickness) - 1)) {
		float translate_x = (float)diameter_w;
		float translate_y = (float)diameter_h;
		translate_x /= 2.f;
		translate_y /= 2.f;

		vg_lite_matrix_t matrix;
		vg_lite_identity(&matrix);
		vg_lite_translate((float)x + translate_x, (float)y + translate_y, &matrix);
		vg_lite_rotate(-start_angle_deg, &matrix);
		vg_lite_scale(DRAWING_SCALE_DIV, DRAWING_SCALE_DIV, &matrix);

		int caps = 0;
		caps |= MEJ_VGLITE_PATH_SET_CAPS_START(start);
		caps |= MEJ_VGLITE_PATH_SET_CAPS_END(end);

		// Compute the thick shape path
		shape_vg_path.path = &__shape_paths.thick_ellipse_arc;
		shape_vg_path.path_length = sizeof(__shape_paths.thick_ellipse_arc);

		(void)UI_DRAWING_VGLITE_PATH_compute_thick_shape_ellipse_arc(
			&shape_vg_path,
			(int)(DRAWING_SCALE_FACTOR * diameter_w),
			(int)(DRAWING_SCALE_FACTOR * diameter_h),
			(int)(DRAWING_SCALE_FACTOR * thickness),
			0,
			arc_angle,
			caps);

		ret = (*drawer)(
			gc,
			&shape_vg_path,
			VG_LITE_FILL_NON_ZERO,
			&matrix,
			VG_LITE_BLEND_SRC_OVER,
			gc->foreground_color);
	} else {
		ret = DRAWING_DONE;
	}
	return ret;
}

// See the section 'Internal function definitions' for the function documentation
static DRAWING_Status __rotate_image(
	UI_DRAWING_VGLITE_PROCESS_blit_rect_t drawer,
	MICROUI_GraphicsContext *gc,
	const MICROUI_Image *img,
	vg_lite_buffer_t *src,
	int x,
	int y,
	int xRotation,
	int yRotation,
	float angle_deg,
	int alpha,
	vg_lite_filter_t filter) {
	DRAWING_Status ret;

	// Check if there is something to draw and clip drawing limits
	// TODO try to crop region
	if (UI_VGLITE_enable_vg_lite_scissor_region(
			gc,
			0, 0,
			gc->image.width, gc->image.height)) {
		vg_lite_matrix_t matrix;
		vg_lite_identity(&matrix);
		vg_lite_translate(xRotation, yRotation, &matrix);
		vg_lite_rotate(-angle_deg, &matrix);
		vg_lite_translate(x - xRotation, y - yRotation, &matrix);

		uint32_t blit_rect[4];
		blit_rect[0] = 0;
		blit_rect[1] = 0;
		blit_rect[2] = img->width;
		blit_rect[3] = img->height;

		ret = (*drawer)(
			gc,
			src,
			blit_rect,
			&matrix, VG_LITE_BLEND_SRC_OVER,
			UI_VGLITE_get_vglite_color(gc, img, alpha),
			filter);
	} else {
		ret = DRAWING_DONE;
	}
	return ret;
}

// See the section 'Internal function definitions' for the function documentation
static DRAWING_Status __scale_image(
	UI_DRAWING_VGLITE_PROCESS_blit_rect_t drawer,
	MICROUI_GraphicsContext *gc,
	const MICROUI_Image *img,
	vg_lite_buffer_t *src,
	int x,
	int y,
	float factorX,
	float factorY,
	int alpha,
	vg_lite_filter_t filter) {
	DRAWING_Status ret;

	// Check if there is something to draw and clip drawing limits
	if (UI_VGLITE_enable_vg_lite_scissor_region(
			gc, x, y,
			(int)(x + (factorX * img->width)),
			(int)(y + (factorY * img->height)))) {
		vg_lite_matrix_t matrix;
		vg_lite_identity(&matrix);
		vg_lite_translate(x, y, &matrix);
		vg_lite_scale(factorX, factorY, &matrix);

		uint32_t blit_rect[4];
		blit_rect[0] = 0;
		blit_rect[1] = 0;
		blit_rect[2] = img->width;
		blit_rect[3] = img->height;

		ret = (*drawer)(
			gc,
			src,
			blit_rect,
			&matrix, VG_LITE_BLEND_SRC_OVER,
			UI_VGLITE_get_vglite_color(gc, img, alpha),
			filter);
	} else {
		ret = DRAWING_DONE;
	}
	return ret;
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------
