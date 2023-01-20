/*
 * C
 *
 * Copyright 2019-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief MicroEJ MicroUI library low level API: implementation over VG-Lite
 * @author MicroEJ Developer Team
 * @version 4.0.0
 */

#if !defined DISPLAY_VGLITE_H
#define DISPLAY_VGLITE_H

#if defined __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <LLUI_PAINTER_impl.h>

#include "vglite_window.h"

// -----------------------------------------------------------------------------
// API
// -----------------------------------------------------------------------------

/*
 * @brief Flush the GPU operations.
 *
 * @param[in] wakeup_graphics_engine: true to wakeup the graphics engine as soon as
 * the drawing is performed, false to perform an active waiting until the GPU interrupt
 * is thrown
 */
void DISPLAY_VGLITE_start_operation(bool wakeup_graphics_engine);

/*
 * @brief Enables hardware rendering
 * @see VGLITE_OPTION_TOGGLE_GPU
 */
void DISPLAY_VGLITE_enable_hardware_rendering(void);

/*
 * @brief Disables hardware rendering
 * @see VGLITE_OPTION_TOGGLE_GPU
 */
void DISPLAY_VGLITE_disable_hardware_rendering(void);

/*
 * @brief Checks if hardware rendering is enabled
 *
 * @return: true if hardware rendering is enabled, false otherwise
 * @see VGLITE_OPTION_TOGGLE_GPU
 */
bool DISPLAY_VGLITE_is_hardware_rendering_enabled(void);

/*
 * @brief Toggles hardware rendering
 * @see VGLITE_OPTION_TOGGLE_GPU
 */
void DISPLAY_VGLITE_toggle_hardware_rendering(void);

/*
 * @brief Create display, window contexes and initialize the VGLite library
 */
void DISPLAY_VGLITE_init(void);

/*
 * @brief Get the main VGLite window
 */
vg_lite_window_t* DISPLAY_VGLITE_get_window(void);

/*
 * @brief Get the next graphics buffer to use
 */
vg_lite_buffer_t *DISPLAY_VGLITE_get_next_graphics_buffer(void);

/*
 * Because of VGLite library implementation,
 * a buffer located at the same address should not be modified and reused
 * as the VGLite library check if the buffer address match with the one
 * stored in its context. If matching is successfull, render target will not
 * be updated.
 * This function rotate over an array of vg_lite_buffers to give a different
 * buffer address at each call and configures it from a MICROUI_Image.
 * The caller MUST send it to the vg_lite library to update its context.
 *
 * @param[in] gc: graphics context of destination
 *
 * @return a vg_lite buffer that should not match vg_lite library render context
 */
vg_lite_buffer_t* DISPLAY_VGLITE_configure_destination(MICROUI_GraphicsContext* gc);

/*
 * This function configures a source buffer from a MICROUI_Image.
 *
 * @param[in] buffer: source buffer
 * @param[in] image: source image
 *
 * @return false if source image format is not supported by vg_lite, true on success
 */
bool DISPLAY_VGLITE_configure_source(vg_lite_buffer_t *buffer, MICROUI_Image* image);

/*
 * @brief Premultiplies color components with the opacity to be compatible with VG-Lite GPU.
 *
 * @param[in] color: the color in non-premultiplied xRGB8888 format ('A' is ignored)
 * @param[in] alpha: the opacity to apply
 * @return the color in premultiplied ARGB8888 format
 */
uint32_t DISPLAY_VGLITE_premultiply_alpha(uint32_t color, uint8_t alpha);

/*
 * @brief Converts a non-premultiplied color into a premultiplied color.
 *
 * @param[in] color: the color in non-premultiplied ARGB8888 format
 * @return the color in premultiplied ARGB8888 format
 */
uint32_t DISPLAY_VGLITE_premultiply(uint32_t color);

/*
 * @brief Called by display_vglite.c in the GPU interrupt routine, just after calling vg_lite_IRQHandler().
 */
void DISPLAY_VGLITE_IRQHandler(void);

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // !defined DISPLAY_VGLITE_H
