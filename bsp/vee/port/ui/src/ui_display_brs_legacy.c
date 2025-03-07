/*
 * Copyright 2023-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief This file implements all the LLUI_DISPLAY_impl.h functions relating to the
 * display buffer strategy (BRS) "legacy"
 * @see UI_FEATURE_BRS_LEGACY comment
 * @author MicroEJ Developer Team
 * @version 14.2.0
 */

#include "ui_display_brs.h"
#if defined UI_FEATURE_BRS && UI_FEATURE_BRS == UI_FEATURE_BRS_LEGACY

// --------------------------------------------------------------------------------
// Private fields
// --------------------------------------------------------------------------------

static ui_rect_t flush_bounds = {
	.x1 = INT16_MAX,
	.y1 = INT16_MAX,
	.x2 = 0,
	.y2 = 0,
};

// --------------------------------------------------------------------------------
// LLUI_DISPLAY_impl.h API
// --------------------------------------------------------------------------------

// See the header file for the function documentation
DRAWING_Status LLUI_DISPLAY_IMPL_newDrawingRegion(MICROUI_GraphicsContext *gc, ui_rect_t *region, bool drawing_now) {
	(void)gc;
	(void)drawing_now;
	flush_bounds.x1 = MIN(flush_bounds.x1, region->x1);
	flush_bounds.y1 = MIN(flush_bounds.y1, region->y1);
	flush_bounds.x2 = MAX(flush_bounds.x2, region->x2);
	flush_bounds.y2 = MAX(flush_bounds.y2, region->y2);
	return DRAWING_DONE;
}

DRAWING_Status LLUI_DISPLAY_IMPL_refresh(MICROUI_GraphicsContext *gc, uint8_t flushIdentifier) {
	LLTRACE_record_event_u32x6(LLUI_EVENT_group, LLUI_EVENT_offset + UI_LOG_BRS_FlushSingle, flushIdentifier,
	                           (uint32_t)LLUI_DISPLAY_getBufferAddress(&gc->image), flush_bounds.x1, flush_bounds.y1,
	                           flush_bounds.x2, flush_bounds.y2);

	LLUI_DISPLAY_IMPL_flush(gc, flushIdentifier, &flush_bounds, 1);

	// reset flush bounds to no flush again if there is no drawing until next flush
	flush_bounds.x1 = INT16_MAX;
	flush_bounds.y1 = INT16_MAX;
	flush_bounds.x2 = 0;
	flush_bounds.y2 = 0;

	return DRAWING_DONE;
}

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------

#endif // UI_FEATURE_BRS_LEGACY
