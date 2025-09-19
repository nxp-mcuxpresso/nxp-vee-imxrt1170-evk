/*
 * C
 *
 * Copyright 2023-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief Provides some functions to draw VGLite elements on a BufferedVectorImage
 * (in VGLite internal format).
 *
 * @author MicroEJ Developer Team
 * @version 9.0.1
 */

#if !defined VG_BVI_VGLITE_H
#define VG_BVI_VGLITE_H

#if defined __cplusplus
extern "C" {
#endif

#include "vg_configuration.h"
#if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <LLVG_BVI_impl.h>

#include "vg_lite.h"

// --------------------------------------------------------------------------------
// Public API
// --------------------------------------------------------------------------------

/*
 * @brief Initializes the engine that implements the MicroVG BufferedVectorImage feature
 * over the VGLite GPU.
 */
void VG_BVI_VGLITE_initialize(void);

/*
 * @brief Adjusts the image data size to store the implementation of the BufferedVectorImage.
 *
 * @see See ui_drawing.h
 */
void VG_BVI_VGLITE_adjust_new_image_characteristics(uint32_t width, uint32_t height, uint32_t *data_size,
                                                    uint32_t *data_alignment);

/*
 * @brief Initializes the BufferedVectorImage.
 *
 * @see See ui_drawing.h
 */
void VG_BVI_VGLITE_initialize_new_image(MICROUI_Image *image);

/*
 * @brief Closes the BufferedVectorImage's resources.
 *
 * @see See ui_drawing.h
 */
void VG_BVI_VGLITE_free_resources(MICROUI_Image *image);

/*
 * @brief Adds an operation "draw path with a color" in the BufferedVectorImage.
 *
 * @param[in] target: the BufferedVectorImage.
 *
 * @return MicroVG error code
 *
 * @see vg_lite_draw()
 */
jint VG_BVI_VGLITE_add_draw_path(void *target, const vg_lite_path_t *path, vg_lite_fill_t fill_rule,
                                 const vg_lite_matrix_t *matrix, vg_lite_blend_t blend, vg_lite_color_t color);

/*
 * @brief Adds an operation "draw path with a gradient" in the BufferedVectorImage.
 *
 * @param[in] target: the BufferedVectorImage.
 *
 * @return MicroVG error code
 *
 * @see vg_lite_draw_gradient()
 */
jint VG_BVI_VGLITE_add_draw_gradient(void *target, const vg_lite_path_t *path, vg_lite_fill_t fill_rule,
                                     const vg_lite_matrix_t *matrix, const vg_lite_linear_gradient_t *grad,
                                     vg_lite_blend_t blend);

/*
 * @brief Adds an operation "draw VG image" in the BufferedVectorImage. If the image
 * is a BufferedVectorImage, the all its operations are copied into the destination.
 * If the image is an image in flash, an operation that points on the image is added.
 *
 * @param[in] target: the BufferedVectorImage.
 *
 * @return MicroVG error code
 *
 * @see vg_drawing.h
 */
jint VG_BVI_VGLITE_add_draw_image(void *target, const MICROVG_Image *image, const jfloat *drawing_matrix, jint alpha,
                                  jlong elapsed, const float color_matrix[]);

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#endif // #if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE

#ifdef __cplusplus
}
#endif
#endif // !defined VG_BVI_VGLITE_H
