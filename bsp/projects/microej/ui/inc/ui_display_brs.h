/*
 * Copyright 2023-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef UI_DISPLAY_BRS_H
#define UI_DISPLAY_BRS_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @file
 * @brief Provides some implementations of the LLUI_DISPLAY_impl.h functions relating
 * to the display buffer strategy (BRS).
 *
 * A BRS
 * - implements the LLUI_DISPLAY_impl.h's API LLUI_DISPLAY_IMPL_refresh() and
 * LLUI_DISPLAY_IMPL_newDrawingRegion().
 * - calls the LLUI_DISPLAY_impl.h's API LLUI_DISPLAY_IMPL_flush() to let the display
 * driver transmit / copy / flush back buffer data to the front buffer or swap back and
 * front buffer (double or triple buffer management).
 * - ensures the coherence of the back buffer's content before and after a flush.
 *
 * Several BRS are available in ui_display_brs_configuration.h (the implementation has
 * to select one of them).
 *
 * @author MicroEJ Developer Team
 * @version 4.1.0
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <LLUI_DISPLAY_impl.h>

#include "ui_util.h"
#include "ui_display_brs_configuration.h"
#include "ui_log.h"

// -----------------------------------------------------------------------------
// Configuration Sanity Check
// -----------------------------------------------------------------------------

/**
 * Sanity check between the expected version of the configuration and the actual version of
 * the configuration.
 * If an error is raised here, it means that a new version of the CCO has been installed and
 * the configuration ui_display_brs_configuration.h must be updated based on the one provided
 * by the new CCO version.
 */

#if !defined UI_DISPLAY_BRS_CONFIGURATION_VERSION
#error "Undefined UI_DISPLAY_BRS_CONFIGURATION_VERSION, it must be defined in ui_display_brs_configuration.h"
#endif

#if defined UI_DISPLAY_BRS_CONFIGURATION_VERSION && UI_DISPLAY_BRS_CONFIGURATION_VERSION != 1
#error "Version of the configuration file ui_display_brs_configuration.h is not compatible with this implementation."
#endif

// -----------------------------------------------------------------------------
// Macros and defines
// -----------------------------------------------------------------------------

/*
 * @brief Logs a region (a rectangle)
 */
#define LOG_REGION(log, rect) LLTRACE_record_event_u32x4((LLUI_EVENT_group), (LLUI_EVENT_offset) + (log), (rect)->x1, \
														 (rect)->y1, (rect)->x2, (rect)->y2)

// --------------------------------------------------------------------------------
// Display BRS public API
// --------------------------------------------------------------------------------

/*
 * @brief Restores (copies) the given rectangular region from old back buffer to the graphics
 * context's current buffer (the destination buffer can be retrieved thanks
 * LLUI_DISPLAY_getBufferAddress(&gc->image)).
 *
 * When the copy is synchronous (immediate), the implementation has to return
 * DRAWING_DONE. When the copy is asynchronous (performed by a DMA for instance), the
 * the implementation has to return DRAWING_RUNNING. As the end of the asynchronous
 * copy, the implementation has to call LLUI_DISPLAY_notifyAsynchronousDrawingEnd()
 * to unlock the caller of this function.
 *
 * The implementation of this function is optional; a weak function provides a simple
 * implementation using memcpy().
 *
 * @param[in] gc the MicroUI GraphicsContext that targets the current back buffer
 * @param[in] old_back_buffer a MicroUI Image that symbolizes (targets) the old back buffer.
 * @param[in] rect the rectangular region to copy from old back buffer to new back
 * buffer.
 */
DRAWING_Status UI_DISPLAY_BRS_restore(MICROUI_GraphicsContext *gc, MICROUI_Image *old_back_buffer, ui_rect_t *rect);

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
#endif // UI_DISPLAY_BRS_H
