/*
 * C
 *
 * Copyright 2022-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#if !defined VG_VGLITE_HELPER_H
#define VG_VGLITE_HELPER_H

/**
 * @file
 * @brief MicroVG library low level API. Provides helper defines and functions to
 * manipulates the VGLite objects.
 * @author MicroEJ Developer Team
 * @version 8.0.1
 */

#if defined __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <LLVG_GRADIENT_impl.h>
#include <LLUI_DISPLAY.h>

#include "vg_lite.h"

/*
 * @brief Sets the drawing log flags matching a MicroVG error code.
 *
 * @see LLUI_DISPLAY_reportError
 *
 * @param[in] gc: the graphics context holding the drawing log flags.
 * @param[in] error: the MicroVG error code.
 *
 * @return the drawing status that should be return by the calling drawing function.
 */
static inline DRAWING_Status VG_VGLITE_HELPER_report_error(MICROUI_GraphicsContext *gc, jint error) {
	LLUI_DISPLAY_reportError(gc,
	                         DRAWING_LOG_LIBRARY_INCIDENT | ((LLVG_OUT_OF_MEMORY ==
	                                                          error) ? DRAWING_LOG_OUT_OF_MEMORY : 0));
	return DRAWING_DONE;
}

/*
 * @brief Processes a MicroVG status code and reports the error if needed.
 *
 * @see VG_VGLITE_HELPER_report_error
 *
 * @param[in] gc: the graphics context holding the drawing log flags.
 * @param[in] vg_lite_error: the MicroVG status code.
 */
static inline void VG_VGLITE_HELPER_handle_error(MICROUI_GraphicsContext *gc, jint status) {
	if (LLVG_SUCCESS != status) {
		(void)VG_VGLITE_HELPER_report_error(gc, status);
	}
}

// -----------------------------------------------------------------------------
// Macros and Defines
// -----------------------------------------------------------------------------

/*
 * @brief The vglite matrix can be mapped on a float array
 */
#define MAP_VGLITE_MATRIX(h) ((float *)(((vg_lite_matrix_t *)(h))->m))

/*
 * @brief The vglite gradient matrix can be mapped on a float array
 */
#define MAP_VGLITE_GRADIENT_MATRIX(h) MAP_VGLITE_MATRIX(&(((vg_lite_linear_gradient_t *)(h))->matrix))

/*
 * @brief Constant used to map the MicroVG gradient range (0.0 to 1.0) to the VGLite gradient range (0.0 to 255.0).
 */
#define VGLITE_GRADIENT_SIZE 256

// -----------------------------------------------------------------------------
// Typedef
// -----------------------------------------------------------------------------

/*
 * @brief The user data of the function VG_FREETYPE_draw_glyph_t().
 * @see VG_FREETYPE_draw_glyph_t()
 */
typedef struct {
	/*
	 * @brief Pointer to the VGLite gradient when drawing a string with a linear
	 * gradient or NULL when drawing a string with a color.
	 */
	vg_lite_linear_gradient_t *gradient;

	/*
	 * @brief Blend mode to apply when drawing a string.
	 */
	vg_lite_blend_t blend;

	/*
	 * @brief Boolean set to true as soon as at least one path is sent to the GPU
	 * commands list.
	 */
	bool rendered;
} MICROVG_VGLITE_draw_glyph_data_t;

// -----------------------------------------------------------------------------
// Typedef
// -----------------------------------------------------------------------------

/*
 * @brief Map a jint array that represents a linear gradient
 */
typedef struct {
	uint32_t count;  // number of colors and positions
	float x;
	float y;
	float length;
	float angle;
	uint32_t colors_offset;
	uint32_t positions_offset;
} MICROVG_GRADIENT_HEADER_t;

// -----------------------------------------------------------------------------
// API
// -----------------------------------------------------------------------------

/*
 * @brief Converts a MicroVG blend to a VGLite blend.
 *
 * @param[in] blend: the drawing's blending
 *
 * @return the VGLite blend
 */
vg_lite_blend_t VG_VGLITE_HELPER_get_blend(jint blend);

/*
 * @brief Converts a MicroVG fill rule to a VGLite rule.
 *
 * @param[in] blend: the drawing's fill rule
 *
 * @return the VGLite fill rule
 */
vg_lite_fill_t VG_VGLITE_HELPER_get_fill_rule(jint fill_type);

/*
 * @brief Converts a VGLite blend to a MicroVG blend.
 *
 * @param[in] blend: the drawing's blending
 *
 * @return the MicroVG blend
 */
uint8_t VG_VGLITE_HELPER_get_microvg_blend(vg_lite_blend_t blend);

/*
 * @brief Converts a VGLite fill rule to a MicroVG fill rule.
 *
 * @param[in] blend: the drawing's fill rule
 *
 * @return the MicroVG fill rule
 */
uint8_t VG_VGLITE_HELPER_get_microvg_fill_rule(vg_lite_fill_t fill);

/*
 * @brief Converts a MicroVG LinearGradient in a VGLite gradient according to
 * the drawing parameters. After this call, the gradient is ready to be updated
 * by calling vg_lite_update_grad()
 *
 * @param[in] gradient: the gradient destination (VGLite gradient)
 * @param[in] gradientData: the gradient source
 * @param[in] matrix: the gradient source's matrix
 * @param[in] globalMatrix: the drawing's matrix
 * @param[in] globalAlpha: the drawing's opacity
 *
 * @return a VGLite error code
 */
vg_lite_error_t VG_VGLITE_HELPER_to_vg_lite_gradient(vg_lite_linear_gradient_t *gradient, const jint *gradientData,
                                                     const jfloat *matrix, const jfloat *globalMatrix,
                                                     jint globalAlpha);

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // !defined VG_VGLITE_HELPER_H
