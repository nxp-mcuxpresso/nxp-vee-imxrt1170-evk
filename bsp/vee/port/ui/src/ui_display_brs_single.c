/*
 * Copyright 2023-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief This file implements all the LLUI_DISPLAY_impl.h functions relating to the
 * display buffer strategy (BRS) "single"
 * @see UI_FEATURE_BRS_SINGLE comment
 * @author MicroEJ Developer Team
 * @version 14.2.0
 */

#include "ui_display_brs.h"
#if defined UI_FEATURE_BRS && UI_FEATURE_BRS == UI_FEATURE_BRS_SINGLE

// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------

#include "ui_rect_collection.h"

// --------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------

/*
 * @brief This strategy uses only one buffer
 * @see the comment of UI_FEATURE_BRS_SINGLE.
 */
#if defined UI_FEATURE_BRS_DRAWING_BUFFER_COUNT && UI_FEATURE_BRS_DRAWING_BUFFER_COUNT != 1
#error "This strategy uses always the same back buffer."
#endif

// --------------------------------------------------------------------------------
// Private fields
// --------------------------------------------------------------------------------

#ifndef UI_FEATURE_BRS_FLUSH_SINGLE_RECTANGLE

/*
 * @brief A collection of rectangles to transmit to the LCD
 */
static ui_rect_collection_t dirty_regions;

#else // UI_FEATURE_BRS_FLUSH_SINGLE_RECTANGLE

/*
 * @brief Rectangle given to LLUI_DISPLAY_IMPL_flush(): it includes all dirty regions since
 * last call to flush().
 */
static ui_rect_t flush_bounds = {
	.x1 = INT16_MAX,
	.y1 = INT16_MAX,
	.x2 = 0,
	.y2 = 0,
};

#endif // UI_FEATURE_BRS_FLUSH_SINGLE_RECTANGLE

// --------------------------------------------------------------------------------
// LLUI_DISPLAY_impl.h API
// --------------------------------------------------------------------------------

/*
 * @brief See the header file for the function documentation.
 *
 * This function stores the new region as new region to transmit to the LCD at next call to flush().
 */
DRAWING_Status LLUI_DISPLAY_IMPL_newDrawingRegion(MICROUI_GraphicsContext *gc, ui_rect_t *region, bool drawing_now) {
	(void)drawing_now;

	LOG_REGION(UI_LOG_BRS_NewDrawing, region);

#ifndef UI_FEATURE_BRS_FLUSH_SINGLE_RECTANGLE

	ui_rect_t *previous = UI_RECT_COLLECTION_get_last(&dirty_regions);
	if ((NULL == previous) || !UI_RECT_contains_rect(previous, region)) {
		// add the dirty region if and only if previous dirty region does not
		// include the new dirty region
		ui_rect_t new_region;
		if (!UI_RECT_COLLECTION_is_full(&dirty_regions)) {
			new_region = *region;
		} else {
			// too many rectangles: replace them by only one whose fits the full display
			LLTRACE_record_event_void(LLUI_EVENT_group, LLUI_EVENT_offset + UI_LOG_BRS_ClearList);
			UI_RECT_COLLECTION_clear(&dirty_regions);
			new_region = UI_RECT_new_xyxy(0, 0, gc->image.width - 1, gc->image.height - 1);
		}
		LOG_REGION(UI_LOG_BRS_AddRegion, &new_region);
		UI_RECT_COLLECTION_add_rect(&dirty_regions, new_region);
	}
#else // UI_FEATURE_BRS_FLUSH_SINGLE_RECTANGLE
	(void)gc;

	flush_bounds.x1 = MIN(flush_bounds.x1, region->x1);
	flush_bounds.y1 = MIN(flush_bounds.y1, region->y1);
	flush_bounds.x2 = MAX(flush_bounds.x2, region->x2);
	flush_bounds.y2 = MAX(flush_bounds.y2, region->y2);

#endif // UI_FEATURE_BRS_FLUSH_SINGLE_RECTANGLE

	return DRAWING_DONE;
}

/*
 * @brief See the header file for the function documentation.
 *
 * This function calls LLUI_DISPLAY_IMPL_flush() with the rectangle that includes all dirty regions.
 */
DRAWING_Status LLUI_DISPLAY_IMPL_refresh(MICROUI_GraphicsContext *gc, uint8_t flushIdentifier) {
#ifndef UI_FEATURE_BRS_FLUSH_SINGLE_RECTANGLE

	size_t size = UI_RECT_COLLECTION_get_length(&dirty_regions);
	if (1u == size) {
		ui_rect_t *rect = &dirty_regions.data[0];
		LLTRACE_record_event_u32x6(LLUI_EVENT_group, LLUI_EVENT_offset + UI_LOG_BRS_FlushSingle, flushIdentifier,
		                           (uint32_t)LLUI_DISPLAY_getBufferAddress(&gc->image), rect->x1, rect->y1, rect->x2,
		                           rect->y2);
	} else {
		LLTRACE_record_event_u32x3(LLUI_EVENT_group, LLUI_EVENT_offset + UI_LOG_BRS_FlushMulti, flushIdentifier,
		                           (uint32_t)LLUI_DISPLAY_getBufferAddress(&gc->image), size);
	}

	LLUI_DISPLAY_IMPL_flush(gc, flushIdentifier, dirty_regions.data, UI_RECT_COLLECTION_get_length(&dirty_regions));
	UI_RECT_COLLECTION_clear(&dirty_regions);

#else // UI_FEATURE_BRS_FLUSH_SINGLE_RECTANGLE

	LLTRACE_record_event_u32x6(LLUI_EVENT_group, LLUI_EVENT_offset + UI_LOG_BRS_FlushSingle, flushIdentifier,
	                           (uint32_t)LLUI_DISPLAY_getBufferAddress(&gc->image), flush_bounds.x1, flush_bounds.y1,
	                           flush_bounds.x2, flush_bounds.y2);

	// refresh the LCD; use the flush_bounds as dirty region (includes all dirty regions)
	LLUI_DISPLAY_IMPL_flush(gc, flushIdentifier, &flush_bounds, 1);

	// reset flush bounds to no flush again if there is no drawing until next flush
	flush_bounds.x1 = INT16_MAX;
	flush_bounds.y1 = INT16_MAX;
	flush_bounds.x2 = 0;
	flush_bounds.y2 = 0;

#endif // UI_FEATURE_BRS_FLUSH_SINGLE_RECTANGLE

	return DRAWING_DONE;
}

#endif // UI_FEATURE_BRS_SINGLE

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------
