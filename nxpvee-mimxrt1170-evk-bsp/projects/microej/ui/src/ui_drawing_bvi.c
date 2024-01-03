/*
 * C
 *
 * Copyright 2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief Implementation of a set of ui_drawing.h drawing functions (MicroUI and Drawing libraries).
 * These are implementations over VG-Lite and the destination buffer format is the BufferedVectorImage
 * format (VEE Port specific). When the drawing cannot be performed by the GPU, the stub implementation
 * is used insted.
 *
 * Like ui_drawing_vglite.c, this file just configures the destination buffer and redirects all drawings
 * to drawing_vglite.c. The file drawing_vglite.c converts the MicroUI drawing in a path can call a specific
 * function to draw the path; it does not perform the drawing itself and let the caller do it. This
 * drawing function is specified by ui_drawing_bvi.c to target BufferedVectorImage buffer.
 *
 * @author MicroEJ Developer Team
 * @version 6.1.0
 * @see ui_drawing_vglite.c
 */

#include "microvg_configuration.h"
#if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE

// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------

#include <LLUI_DISPLAY.h>

#include "ui_drawing_bvi.h"
#include "bvi_vglite.h"
#include "ui_drawing_stub.h"
#include "ui_drawing_vglite_process.h"
#include "ui_drawing_vglite_path.h"
#include "microvg_vglite_helper.h"

// --------------------------------------------------------------------------------
// Private functions
// --------------------------------------------------------------------------------

static DRAWING_Status _add_path(MICROUI_GraphicsContext* gc, vg_lite_path_t * path, vg_lite_fill_t fill_rule, vg_lite_matrix_t * matrix, vg_lite_blend_t blend, vg_lite_color_t color) {
	jint err = BVI_VGLITE_add_draw_path(MAP_BVI_ON_GC(gc), path, fill_rule, matrix, blend, color);
	MICROVG_VGLITE_handle_error(gc, err);
	return DRAWING_DONE;
}

static DRAWING_Status _add_clear(MICROUI_GraphicsContext* gc, vg_lite_rectangle_t * rect, vg_lite_color_t color) {

	// Compute the rectangle shape path
	vg_lite_path_t shape_vg_path;
	vglite_path_rectangle_t rectangle_path;
	shape_vg_path.path = &rectangle_path;
	shape_vg_path.path_length = sizeof(rectangle_path);
	int path_offset = UI_DRAWING_VGLITE_PATH_compute_rectangle(&shape_vg_path, 0, rect->x, rect->y, rect->width, rect->height, true);

	if (0 < path_offset) {
		vg_lite_matrix_t matrix;
		vg_lite_identity(&matrix);
		jint err = BVI_VGLITE_add_draw_path(MAP_BVI_ON_GC(gc), &shape_vg_path, VG_LITE_FILL_EVEN_ODD, &matrix, VG_LITE_BLEND_SRC_OVER, color);
		MICROVG_VGLITE_handle_error(gc, err);
	}
	else {
		LLUI_DISPLAY_reportError(gc, DRAWING_LOG_LIBRARY_INCIDENT);
	}
	return DRAWING_DONE;
}

// --------------------------------------------------------------------------------
// ui_drawing.h functions
// (the function names differ according to the index of destination format)
// --------------------------------------------------------------------------------

// See the header file for the function documentation
bool UI_DRAWING_BVI_is_drawer(jbyte image_format) {
	return BVI_FORMAT == (MICROUI_ImageFormat)image_format;
}

// See the header file for the function documentation
void UI_DRAWING_BVI_adjustNewImageCharacteristics(jbyte image_format, uint32_t width, uint32_t height, uint32_t* data_size, uint32_t* data_alignment) {
	(void)image_format;
	BVI_VGLITE_adjust_new_image_characteristics(width, height, data_size, data_alignment);
}

// See the header file for the function documentation
void UI_DRAWING_BVI_initializeNewImage(MICROUI_Image* image) {
	BVI_VGLITE_initialize_new_image(image);
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_BVI_drawLine(MICROUI_GraphicsContext* gc, jint startX, jint startY, jint endX, jint endY){
	return UI_DRAWING_VGLITE_PROCESS_drawLine(&_add_path, gc, startX, startY, endX, endY);
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_BVI_drawHorizontalLine(MICROUI_GraphicsContext* gc, jint x1, jint x2, jint y){
	return UI_DRAWING_VGLITE_PROCESS_drawHorizontalLine(&_add_path, gc, x1, x2, y);
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_BVI_drawVerticalLine(MICROUI_GraphicsContext* gc, jint x, jint y1, jint y2){
	return UI_DRAWING_VGLITE_PROCESS_drawVerticalLine(&_add_path, gc, x, y1, y2);
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_BVI_fillRectangle(MICROUI_GraphicsContext* gc, jint x1, jint y1, jint x2, jint y2){
	return UI_DRAWING_VGLITE_PROCESS_fillRectangle(&_add_clear, gc, x1, y1, x2, y2);
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_BVI_drawRoundedRectangle(MICROUI_GraphicsContext* gc, jint x, jint y, jint width, jint height, jint cornerEllipseWidth, jint cornerEllipseHeight){
	return UI_DRAWING_VGLITE_PROCESS_drawRoundedRectangle(&_add_path, gc, x, y, width, height, cornerEllipseWidth, cornerEllipseHeight);
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_BVI_fillRoundedRectangle(MICROUI_GraphicsContext* gc, jint x, jint y, jint width, jint height, jint cornerEllipseWidth, jint cornerEllipseHeight){
	return UI_DRAWING_VGLITE_PROCESS_fillRoundedRectangle(&_add_path, gc, x, y, width, height, cornerEllipseWidth, cornerEllipseHeight);
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_BVI_drawCircleArc(MICROUI_GraphicsContext* gc, jint x, jint y, jint diameter, jfloat startAngle, jfloat arcAngle){
	return UI_DRAWING_VGLITE_PROCESS_drawCircleArc(&_add_path, gc, x, y, diameter, startAngle, arcAngle);
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_BVI_fillCircleArc(MICROUI_GraphicsContext* gc, jint x, jint y, jint diameter, jfloat startAngle, jfloat arcAngle){
	return UI_DRAWING_VGLITE_PROCESS_fillCircleArc(&_add_path, gc, x, y, diameter, startAngle, arcAngle);
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_BVI_drawEllipseArc(MICROUI_GraphicsContext* gc, jint x, jint y, jint width, jint height, jfloat startAngle, jfloat arcAngle){
	return UI_DRAWING_VGLITE_PROCESS_drawEllipseArc(&_add_path, gc, x, y, width, height, startAngle, arcAngle);
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_BVI_fillEllipseArc(MICROUI_GraphicsContext* gc, jint x, jint y, jint width, jint height, jfloat startAngle, jfloat arcAngle){
	return UI_DRAWING_VGLITE_PROCESS_fillEllipseArc(&_add_path, gc, x, y, width, height, startAngle, arcAngle);
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_BVI_drawCircle(MICROUI_GraphicsContext* gc, jint x, jint y, jint diameter){
	return UI_DRAWING_VGLITE_PROCESS_drawCircle(&_add_path, gc, x, y, diameter);
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_BVI_fillCircle(MICROUI_GraphicsContext* gc, jint x, jint y, jint diameter){
	return UI_DRAWING_VGLITE_PROCESS_fillCircle(&_add_path, gc, x, y, diameter);
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_BVI_drawEllipse(MICROUI_GraphicsContext* gc, jint x, jint y, jint width, jint height){
	return UI_DRAWING_VGLITE_PROCESS_drawEllipse(&_add_path, gc, x, y, width, height);
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_BVI_fillEllipse(MICROUI_GraphicsContext* gc, jint x, jint y, jint width, jint height){
	return UI_DRAWING_VGLITE_PROCESS_fillEllipse(&_add_path, gc, x, y, width, height);
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_BVI_drawThickFadedPoint(MICROUI_GraphicsContext* gc, jint x, jint y, jint thickness, jint fade){
	DRAWING_Status status;
	if (!UI_DRAWING_VGLITE_IS_COMPATIBLE_FADE(fade)) {
		status = UI_DRAWING_STUB_drawThickFadedPoint(gc, x, y, thickness, fade);
	}
	else {
		status = UI_DRAWING_VGLITE_PROCESS_drawThickFadedPoint(&_add_path, gc, x, y, UI_DRAWING_VGLITE_GET_THICKNESS(thickness, fade));
	}
	return status;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_BVI_drawThickFadedLine(MICROUI_GraphicsContext* gc, jint startX, jint startY, jint endX, jint endY, jint thickness, jint fade, DRAWING_Cap startCap, DRAWING_Cap endCap){
	DRAWING_Status status;
	if (!UI_DRAWING_VGLITE_IS_COMPATIBLE_FADE(fade)) {
		status = UI_DRAWING_STUB_drawThickFadedLine(gc, startX, startY, endX, endY, thickness, fade, startCap, endCap);
	}
	else {
		status = UI_DRAWING_VGLITE_PROCESS_drawThickFadedLine(&_add_path, gc, startX, startY, endX, endY, UI_DRAWING_VGLITE_GET_THICKNESS(thickness, fade), startCap, endCap);
	}

	return status;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_BVI_drawThickFadedCircle(MICROUI_GraphicsContext* gc, jint x, jint y, jint diameter, jint thickness, jint fade){
	DRAWING_Status status;
	if (!UI_DRAWING_VGLITE_IS_COMPATIBLE_FADE(fade)) {
		status = UI_DRAWING_STUB_drawThickFadedCircle(gc, x, y, diameter, thickness, fade);
	}
	else {
		status = UI_DRAWING_VGLITE_PROCESS_drawThickFadedCircle(&_add_path, gc, x, y, diameter, UI_DRAWING_VGLITE_GET_THICKNESS(thickness, fade));
	}

	return status;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_BVI_drawThickFadedCircleArc(MICROUI_GraphicsContext* gc, jint x, jint y, jint diameter, jfloat startAngle, jfloat arcAngle, jint thickness, jint fade, DRAWING_Cap start, DRAWING_Cap end){
	DRAWING_Status status;
	if (!UI_DRAWING_VGLITE_IS_COMPATIBLE_FADE(fade)) {
		status = UI_DRAWING_STUB_drawThickFadedCircleArc(gc, x, y, diameter, startAngle, arcAngle, thickness, fade, start, end);
	}
	else {
		status = UI_DRAWING_VGLITE_PROCESS_drawThickFadedCircleArc(&_add_path, gc, x, y, diameter, startAngle, arcAngle, UI_DRAWING_VGLITE_GET_THICKNESS(thickness, fade), start, end);
	}

	return status;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_BVI_drawThickFadedEllipse(MICROUI_GraphicsContext* gc, jint x, jint y, jint width, jint height, jint thickness, jint fade){
	DRAWING_Status status;
	if (!UI_DRAWING_VGLITE_IS_COMPATIBLE_FADE(fade)) {
		status = UI_DRAWING_STUB_drawThickFadedEllipse(gc, x, y, width, height, thickness, fade);
	}
	else {
		status = UI_DRAWING_VGLITE_PROCESS_drawThickFadedEllipse(&_add_path, gc, x, y, width, height, UI_DRAWING_VGLITE_GET_THICKNESS(thickness, fade));
	}

	return status;
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_BVI_drawThickLine(MICROUI_GraphicsContext* gc, jint startX, jint startY, jint endX, jint endY, jint thickness){
	return UI_DRAWING_VGLITE_PROCESS_drawThickLine(&_add_path, gc, startX, startY, endX, endY, thickness);
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_BVI_drawThickCircle(MICROUI_GraphicsContext* gc, jint x, jint y, jint diameter, jint thickness){
	return UI_DRAWING_VGLITE_PROCESS_drawThickCircle(&_add_path, gc, x, y, diameter, thickness);
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_BVI_drawThickEllipse(MICROUI_GraphicsContext* gc, jint x, jint y, jint width, jint height, jint thickness){
	return UI_DRAWING_VGLITE_PROCESS_drawThickEllipse(&_add_path, gc, x, y, width, height, thickness);
}

// See the header file for the function documentation
DRAWING_Status UI_DRAWING_BVI_drawThickCircleArc(MICROUI_GraphicsContext* gc, jint x, jint y, jint diameter, jfloat startAngle, jfloat arcAngle, jint thickness){
	return UI_DRAWING_VGLITE_PROCESS_drawThickCircleArc(&_add_path, gc, x, y, diameter, startAngle, arcAngle, thickness);
}

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------

#endif // #if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE
