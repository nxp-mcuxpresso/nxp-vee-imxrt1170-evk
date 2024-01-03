/*
 * C
 *
 * Copyright 2020-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief MicroEJ MicroUI library low level API: implementation over VG-Lite. Provides
 * a set of functions to drive a DMA.
 * @author MicroEJ Developer Team
 * @version 6.0.1
 */

#if !defined DISPLAY_DMA_H
#define DISPLAY_DMA_H

#if defined __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "display_framebuffer.h"

// -----------------------------------------------------------------------------
// Macros and Defines
// -----------------------------------------------------------------------------

// #define DISPLAY_DMA_ENABLED       ( FRAME_BUFFER_COUNT > 1 )

// -----------------------------------------------------------------------------
// API
// -----------------------------------------------------------------------------

/*
 * @brief: Initialize the DMA synchronization between the framebuffers
 *
 * @param[in] framebuffers: display framebuffers
 */
void DISPLAY_DMA_initialize(const framebuffer_t * framebuffers[]);

/*
 * @brief: Starts a DMA transfert
 *
 * @param[in] src: source framebuffer
 * @param[in] dst: destination framebuffer
 * @param[in] ymin: first line to transfert
 * @param[in] ymax: last line to transfert + 1
 */
void DISPLAY_DMA_start( framebuffer_t *dst, framebuffer_t *src, uint32_t size);

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // !defined DISPLAY_DMA_H
