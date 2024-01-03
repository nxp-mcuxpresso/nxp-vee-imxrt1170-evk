/*
 * C
 *
 * Copyright 2019-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief MicroEJ MicroUI library low level API: implementation over VG-Lite. Provides
 * a set of defines to configure the implementation.
 * @author MicroEJ Developer Team
 */

#if !defined UI_VGLITE_CONFIGURATION_H
# define UI_VGLITE_CONFIGURATION_H

#if defined __cplusplus
extern "C" {
#endif

/**
 * @brief Compatibility sanity check value.
 * This define value is checked in the implementation to validate that the version of this configuration
 * is compatible with the implementation.
 *
 * This value must not be changed by the user of the CCO.
 * This value must be incremented by the implementor of the CCO when a configuration define is added, deleted or modified.
 */
#define UI_VGLITE_CONFIGURATION_VERSION (1)

// -----------------------------------------------------------------------------
// Macros and Defines
// -----------------------------------------------------------------------------

/*
 * @brief Width of the Tesselation window
 *
 * @Warning: this impacts the VGLite allocation size
 */
#define VGLITE_TESSELATION_WIDTH	256

/*
 * @brief Height of the Tesselation window
 *
 * @Warning: this impacts the VGLite allocation size
 */
#define VGLITE_TESSELATION_HEIGHT	256

/*
 * @brief GPU is less efficient than CPU to perform simple aliased drawings (line, rectangle etc.)
 *
 * This define forces to use the GPU. Comment it to use the software algorithms instead.
 */
//#define VGLITE_USE_GPU_FOR_SIMPLE_DRAWINGS

/*
 * @brief GPU is less efficient than CPU to perform a simple "draw image": when the image to render has
 * the same pixel definition than the destination buffer and no alpha blending is required.
 *
 * This defines forces to use the GPU to draw RGB565 images. Comment it to use the software algorithms
 * instead.
 */
//#define VGLITE_USE_GPU_FOR_RGB565_IMAGES

/*
 * @brief GCNanoLite-V does not support MSAA. By consequence the "draw image" functions (with or without
 * transformation like rotation or scale) cannot use GPU when the image to render contains transparent
 * pixels.
 *
 * This defines forces to use the GPU to draw transparent images. Comment it to use the software algorithms
 * instead.
 */
#define VGLITE_USE_GPU_FOR_TRANSPARENT_IMAGES

/*
 * @brief Option to enable and disable the use of the GPU (toggle) at runtime. When the option is disabled,
 * the calls to UI_VGLITE_xxx_hardware_rendering have no effect.
 *
 * This defines forces to enable the option. Comment it to disable the option.
 */
//#define VGLITE_OPTION_TOGGLE_GPU

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // !defined UI_VGLITE_CONFIGURATION_H
