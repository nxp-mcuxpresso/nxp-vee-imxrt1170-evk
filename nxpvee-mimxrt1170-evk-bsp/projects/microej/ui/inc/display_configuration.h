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

#if !defined DISPLAY_CONFIGURATION_H
# define DISPLAY_CONFIGURATION_H

#if defined __cplusplus
extern "C" {
#endif

#include "display_support.h"

// -----------------------------------------------------------------------------
// Macros and Defines
// -----------------------------------------------------------------------------

/*
 * @brief Width of the display panel
 */
#define DISPLAY_PANEL_WIDTH (720)

/*
 * @brief Height of the display panel
 */
#define DISPLAY_PANEL_HEIGHT (1280)

/*
 * @brief Width of the frame buffers
 */
#define FRAME_BUFFER_WIDTH (DEMO_BUFFER_WIDTH)

/*
 * @brief Height of the frame buffers
 */
#define FRAME_BUFFER_HEIGHT (DEMO_BUFFER_HEIGHT)

/*
 * @brief Source image line alignment (in bytes), required by VGLite library
 * when an image is used as source: 32 bits for RGB565 format
 * (see _check_source_aligned() in vg_lite.c).
 */
#define VGLITE_IMAGE_LINE_ALIGN_BYTE (32)

/*
 * @brief Frame buffer line alignment (in bytes), required by VGLite library
 * when frame buffer is used as source.
 * Note: Set (1) to not use the frame buffer as source image.
 * Note 2: Require a patch in fsl_dc_fb_dsi_cmd.c
 */
#define FRAME_BUFFER_LINE_ALIGN_BYTE (1) /* 1 | VGLITE_IMAGE_LINE_ALIGN_BYTE */

/*
 * @brief Available number of frame buffers
 */
#define FRAME_BUFFER_COUNT (3)

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // !defined DISPLAY_CONFIGURATION_H
