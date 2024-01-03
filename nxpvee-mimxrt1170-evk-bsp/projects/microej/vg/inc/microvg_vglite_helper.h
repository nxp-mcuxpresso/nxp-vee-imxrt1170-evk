/*
 * C
 *
 * Copyright 2022-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#if !defined MICROVG_VGLITE_HELPER_H
#define MICROVG_VGLITE_HELPER_H

/**
* @file
* @brief MicroVG library low level API. Provides helper defines and functions to
* manipulates the VG-Lite objects.
* @author MicroEJ Developer Team
* @version 6.1.0
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
static inline DRAWING_Status MICROVG_VGLITE_report_error(MICROUI_GraphicsContext* gc, jint error) {
	LLUI_DISPLAY_reportError(gc, DRAWING_LOG_LIBRARY_INCIDENT | ((LLVG_OUT_OF_MEMORY == error) ? DRAWING_LOG_OUT_OF_MEMORY : 0));
	return DRAWING_DONE;
}

/*
 * @brief Processes a MicroVG status code and reports the error if needed.
 *
 * @see MICROVG_VGLITE_report_error
 *
 * @param[in] gc: the graphics context holding the drawing log flags.
 * @param[in] vg_lite_error: the MicroVG status code.
 */
static inline void MICROVG_VGLITE_handle_error(MICROUI_GraphicsContext* gc, jint status) {
	if (LLVG_SUCCESS != status) {
		(void) MICROVG_VGLITE_report_error(gc, status);
	}
}

// -----------------------------------------------------------------------------
// Macros and Defines
// -----------------------------------------------------------------------------

/*
 * @brief The vglite matrix can be mapped on a float array
 */
#define MAP_VGLITE_MATRIX(h) ((float*)(((vg_lite_matrix_t*)(h))->m))

/*
 * @brief The vglite gradient matrix can be mapped on a float array
 */
#define MAP_VGLITE_GRADIENT_MATRIX(h) MAP_VGLITE_MATRIX(&(((vg_lite_linear_gradient_t*)(h))->matrix))

// -----------------------------------------------------------------------------
// API
// -----------------------------------------------------------------------------

/*
 * @brief Converts the MicroVG blend in a VG-Lite blend.
 *
 * @param[in] blend: the drawing's blending
 *
 * @return the VG-Lite blend
 */
vg_lite_blend_t MICROVG_VGLITE_HELPER_get_blend(jint blend);

/*
 * @brief Converts the MicroVG fill rule in a VG-Lite rule.
 *
 * @param[in] blend: the drawing's fill rule
 *
 * @return the VG-Lite fill rule
 */
vg_lite_fill_t MICROVG_VGLITE_HELPER_get_fill_rule(jint fill_type);

/*
 * @brief Converts a MicroVG LinearGradient in a VG-Lite gradient according to
 * the drawing parameters. After this call, the gradient is ready to be updated
 * by calling vg_lite_update_grad()
 *
 * @param[in] gradient: the gradient destination (VG-Lite gradient)
 * @param[in] gradientData: the gradient source
 * @param[in] matrix: the gradient source's matrix
 * @param[in] globalMatrix: the drawing's matrix
 * @param[in] globalAlpha: the drawing's opacity
 *
 * @return a VG-Lite error code
 */
vg_lite_error_t MICROVG_VGLITE_HELPER_to_vg_lite_gradient(vg_lite_linear_gradient_t* gradient, jint* gradientData, jfloat* matrix, jfloat* globalMatrix, jint globalAlpha);

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // !defined MICROVG_VGLITE_HELPER_H
