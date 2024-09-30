/*
 * C
 *
 * Copyright 2019-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief MicroEJ MicroUI library low level API: implementation over VGLite. Provides
 * a set of helper functions to target the VGLite library.
 * @author MicroEJ Developer Team
 * @version 9.0.0
 */

#if !defined UI_VGLITE_H
#define UI_VGLITE_H

#if defined __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <LLUI_DISPLAY.h>
#include <LLUI_PAINTER_impl.h>

#include "ui_vglite_configuration.h"
#include "vg_lite.h"

// -----------------------------------------------------------------------------
// Configuration Sanity Check
// -----------------------------------------------------------------------------

#ifndef VG_DRIVER_SINGLE_THREAD
#error "This define must be set in the BSP pre-processor options."
#endif

/**
 * Sanity check between the expected version of the configuration and the actual version of
 * the configuration.
 * If an error is raised here, it means that a new version of the CCO has been installed and
 * the configuration ui_vglite_configuration.h must be updated based on the one provided
 * by the new CCO version.
 */

#if !defined UI_VGLITE_CONFIGURATION_VERSION
#error "Undefined UI_VGLITE_CONFIGURATION_VERSION, it must be defined in ui_vglite_configuration.h"
#endif

#if defined UI_VGLITE_CONFIGURATION_VERSION && UI_VGLITE_CONFIGURATION_VERSION != 1
#error "Version of the configuration file ui_vglite_configuration.h is not compatible with this implementation."
#endif

// -----------------------------------------------------------------------------
// API
// -----------------------------------------------------------------------------

/**
 * @brief Initializes the VGLite controller and library.
 */
void UI_VGLITE_initialize(void);

/**
 * @brief Configures and starts MicroUI over VGLite.
 *
 * @param[in] binary_semaphore: pointer to a binary semaphore; the semaphore must
 * be configured in a state such that the semaphore must first be 'given' before it
 * can be 'taken'.
 */
void UI_VGLITE_start(void *binary_semaphore);

/**
 * @brief The VEE Port must call this function in the GPU interrupt routine, just
 * after calling vg_lite_IRQHandler() (often implemented in vglite_support.c).
 */
void UI_VGLITE_IRQHandler(void);

/**
 * @brief Flush the GPU operations.
 *
 * @param[in] wakeup_graphics_engine: true to wakeup the graphics engine as soon as
 * the drawing is performed, false to perform an active waiting until the GPU interrupt
 * is thrown
 */
void UI_VGLITE_start_operation(bool wakeup_graphics_engine);

/**
 * @brief Operation to perform after a VGLite drawing operation.
 * On success, the asynchronous drawing is launched. This function does not wait for the end
 * of the drawing.
 * On error, an error message is displayed, the error flags in the graphics context are set,
 * the drawing is considered as done (there is nothing to draw), and a call to UI_VGLITE_IMPL_notify_gpu_stop()
 * is made to request GPU deactivation (if the function is implemented).
 *
 * @param[in] gc: the graphics context holding the error flags.
 * @param[in] vg_lite_error: the VGLite drawing operation's return code.
 *
 * @return: the MicroUI status according to the drawing operation status.
 */
DRAWING_Status UI_VGLITE_post_operation(MICROUI_GraphicsContext *gc, vg_lite_error_t vg_lite_error);

/**
 * @brief Enables hardware rendering
 * @see VGLITE_OPTION_TOGGLE_GPU
 */
void UI_VGLITE_enable_hardware_rendering(void);

/**
 * @brief Disables hardware rendering
 * @see VGLITE_OPTION_TOGGLE_GPU
 */
void UI_VGLITE_disable_hardware_rendering(void);

/**
 * @brief Checks if hardware rendering is enabled
 *
 * @return: true if hardware rendering is enabled, false otherwise
 * @see VGLITE_OPTION_TOGGLE_GPU
 */
bool UI_VGLITE_is_hardware_rendering_enabled(void);

/**
 * @brief Toggles hardware rendering
 * @see VGLITE_OPTION_TOGGLE_GPU
 */
void UI_VGLITE_toggle_hardware_rendering(void);

/**
 * Because of VGLite library implementation, a buffer located at the same address should not be modified and reused as
 * the VGLite library checks if the buffer address matches the one stored in its context. On a match,
 * the render target will not be updated.
 * This function rotates over an array of vg_lite_buffers to give a different buffer address at each call and configures
 * it from a MICROUI_Image.
 * The caller MUST send it to the VGLite library to update its context.
 *
 * @param[in] gc: graphics context of destination
 *
 * @return a vg_lite buffer that should not match vg_lite library render context
 */
vg_lite_buffer_t * UI_VGLITE_configure_destination(MICROUI_GraphicsContext *gc);

/**
 * @brief This function configures a source buffer from a MICROUI_Image.
 *
 * The blend mode in MicroUI is always SRC_OVER. VG_LITE_IMAGE_TRANSPARENT
 * ignores fully transparent pixels, which is not a problem with this blend mode.
 *
 * In the case an other blend mode is used and drawing fully transparent pixels is
 * needed, the caller could set buffer->transparency_mode to VG_LITE_IMAGE_OPAQUE
 * after this function returns true, at the cost of a slight performance drop.
 *
 * @param[in] buffer: source buffer
 * @param[in] image: source image
 *
 * @return false if source image format is not supported by vg_lite, true on success
 */
bool UI_VGLITE_configure_source(vg_lite_buffer_t *buffer, MICROUI_Image *image);

/**
 * @brief Premultiplies color components with the opacity to be compatible with VGLite GPU.
 *
 * @param[in] color: the color in non-premultiplied xRGB8888 format ('A' is ignored)
 * @param[in] alpha: the opacity to apply
 * @return the color in premultiplied ARGB8888 format
 */
uint32_t UI_VGLITE_premultiply_alpha(uint32_t color, uint8_t alpha);

/**
 * @brief Converts a non-premultiplied color into a premultiplied color.
 *
 * @param[in] color: the color in non-premultiplied ARGB8888 format
 * @return the color in premultiplied ARGB8888 format
 */
uint32_t UI_VGLITE_premultiply(uint32_t color);

/**
 * @brief Tells if the GPU requires a pre-multiplied color (means the color components are multiplied by the alpha
 * component).
 *
 * @return false if the GPU can manage non pre-multiplied colors.
 */
bool UI_VGLITE_need_to_premultiply(void);

/**
 * @brief Get color to give to VGLite library according to destination's foreground
 * color, image format and opacity level.
 *
 * @param[in] gc: the MicroUI GraphicsContext of destination
 * @param[in] img: the MicroUI Image of source
 * @param[in] alpha: the application opacity
 *
 * @return The VGLite color
 */
vg_lite_color_t UI_VGLITE_get_vglite_color(const MICROUI_GraphicsContext *gc, const MICROUI_Image *img, jint alpha);

/**
 * @brief Configures the vg_lite's scissor according to the MicroUI clip.
 *
 * If the clip is empty, the drawing is useless and this function returns false.
 *
 * Otherwise the VGLite scissor is configured to match the MicroUI clip.
 *
 * @param[in] gc: the destination where performing the drawing (MicroUI Graphics Context)
 *
 * @return false when the drawing is useless (bounds out of clip)
 */
bool UI_VGLITE_enable_vg_lite_scissor(MICROUI_GraphicsContext *gc);

/**
 * @brief Configures the vg_lite's scissor according to the MicroUI clip and
 * the given region.
 *
 * If the clip is empty or if the region is fully outside the clip,
 * the drawing is useless and this function returns false.
 *
 * Otherwise the VGLite scissor is configured to match the intersection of the
 * MicroUI clip and the region.
 *
 * @param[in] gc: the destination where performing the drawing (MicroUI Graphics Context)
 * @param[in] x1..y2: the region
 *
 * @return false when the drawing is useless (bounds out of clip)
 */
bool UI_VGLITE_enable_vg_lite_scissor_region(MICROUI_GraphicsContext *gc, int x1, int y1, int x2, int y2);

/**
 * @brief Configures the vg_lite's scissor according to the MicroUI clip and
 * the given area, transformed with the given matrix.
 *
 * If the clip is empty or if the area is fully outside the clip,
 * the drawing is useless and this function returns false.
 *
 * Otherwise the VGLite scissor is configured to match the intersection of the
 * MicroUI clip and the transformed area.
 *
 * @param[in] gc: the destination where performing the drawing (MicroUI Graphics Context)
 * @param[in] width, height: the area size
 * @param[in] matrix: the transformation to apply on the area
 *
 * @return false when the drawing is useless (bounds out of clip)
 */
bool UI_VGLITE_enable_vg_lite_scissor_area(MICROUI_GraphicsContext *gc, jfloat width, jfloat height,
                                           const jfloat *matrix);

/**
 * @brief Returns the bit per pixel from a MicroUI image format
 *
 * @param[in] image_format: The MicroUI format of the image
 *
 * @return The bit per pixel of the image, or -1 if the image format is unknown
 */
int32_t UI_VGLITE_get_bpp(MICROUI_ImageFormat image_format);

/**
 * @brief Sets the drawing log flags matching a VGLite error code.
 *
 * The error will be reported to UI_VGLITE_IMPL_error().
 *
 * @see LLUI_DISPLAY_reportError
 *
 * @param[in] gc: the graphics context holding the drawing log flags.
 * @param[in] vg_lite_error: the VGLite error code.
 *
 * @return the drawing status that should be return by the calling drawing function.
 */
DRAWING_Status UI_VGLITE_report_vglite_error(MICROUI_GraphicsContext *gc, vg_lite_error_t vg_lite_error);

// -----------------------------------------------------------------------------
// Low Level API
// -----------------------------------------------------------------------------

/**
 * @brief Notifies the GPU will be used just after this call. The implementation
 * must ensure the GPU can be used (power management, clocks, etc.).
 *
 * Default implementation does nothing.
 */
void UI_VGLITE_IMPL_notify_gpu_start(void);

/**
 * @brief Notifies the GPU is not used anymore by the library until the next call to
 * UI_VGLITE_IMPL_notify_gpu_start().
 *
 * Default implementation does nothing.
 */
void UI_VGLITE_IMPL_notify_gpu_stop(void);

/**
 * @brief Notifies an error has occurred. This error may be critical, in this case,
 * the implementation must stop the application execution (message + infinite loop).
 *
 * On error, default implementation loops indefinitely on error.
 *
 * @param[in] critical: tell whether the error is critical or not.
 * @param[in] format: the error message and its arguments
 */
void UI_VGLITE_IMPL_error(bool critical, const char *format, ...);

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // !defined UI_VGLITE_H
