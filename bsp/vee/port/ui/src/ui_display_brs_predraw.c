/*
 * Copyright 2023-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief This file implements all the LLUI_DISPLAY_impl.h functions relating to the
 * display buffer strategy (BRS) "pre draw"
 * @see UI_FEATURE_BRS_PREDRAW comment
 * @author MicroEJ Developer Team
 * @version 14.2.0
 */

#include "ui_display_brs.h"
#if defined UI_FEATURE_BRS && UI_FEATURE_BRS == UI_FEATURE_BRS_PREDRAW

// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------

#include "ui_rect_collection.h"
#include "ui_rect_util.h"

// --------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------

/*
 * @brief This strategy requires the available number of back buffers.
 * @see the comment of UI_FEATURE_BRS_PREDRAW.
 */
#ifndef UI_FEATURE_BRS_DRAWING_BUFFER_COUNT
#error "Require the available number of display buffers (back and front buffers)."
#elif UI_FEATURE_BRS_DRAWING_BUFFER_COUNT < 2
#warning "This strategy is not optimized for less than two buffers."
#endif

// --------------------------------------------------------------------------------
// Private fields
// --------------------------------------------------------------------------------

/*
 * @brief A collection of dirty regions is kept for each back buffers.
 */
#if UI_FEATURE_BRS_DRAWING_BUFFER_COUNT > 1u
static ui_rect_collection_t dirty_regions[UI_FEATURE_BRS_DRAWING_BUFFER_COUNT - 1u] = { 0 };
#else
static ui_rect_collection_t dirty_regions[1u] = { 0 };
#endif

/*
 * @brief Index of the back buffer's collection to restore in the new back buffer.
 */
static uint32_t index_dirty_region_to_restore = 0;

static bool backbuffer_ready = true;

ui_rect_t diff[4] = { UI_RECT_EMPTY, UI_RECT_EMPTY, UI_RECT_EMPTY, UI_RECT_EMPTY };

#ifdef UI_FEATURE_BRS_FLUSH_SINGLE_RECTANGLE

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
// Private functions
// --------------------------------------------------------------------------------

/*
 * This function removes all regions saved in the next collection to restore in the
 * new back buffer if they are included in the new region.
 */
static void _remove_drawing_regions(MICROUI_GraphicsContext *gc, ui_rect_t *region) {
	(void)gc;

	for (uint32_t i = 0u; i < (UI_FEATURE_BRS_DRAWING_BUFFER_COUNT - 1u); i++) {
		ui_rect_t *r = dirty_regions[i].data;
		while (r != UI_RECT_COLLECTION_get_end(&dirty_regions[i])) {
			if (!UI_RECT_is_empty(r) && UI_RECT_contains_rect(region, r)) {
				// this region will be re-drawn: remove it from the restoration array
				LOG_REGION(UI_LOG_BRS_RemoveRegion, r);
				UI_RECT_mark_empty(r);
			}
			r++;
		}
	}
}

/*
 * This function adds the region to the regions to restore.
 */
static void _add_drawing_region(MICROUI_GraphicsContext *gc, ui_rect_t *region) {
	for (uint32_t i = 0u; i < (UI_FEATURE_BRS_DRAWING_BUFFER_COUNT - 1u); i++) {
		ui_rect_t *previous = UI_RECT_COLLECTION_get_last(&dirty_regions[i]);
		if ((NULL == previous) || !UI_RECT_contains_rect(previous, region)) {
			// add the dirty region if and only if previous dirty region does not
			// include the new dirty region
			ui_rect_t new_region;
			if (!UI_RECT_COLLECTION_is_full(&dirty_regions[i])) {
				new_region = *region;
			} else {
				// too many rectangles: replace them by only one whose fits the full display
				LLTRACE_record_event_void(LLUI_EVENT_group, LLUI_EVENT_offset + UI_LOG_BRS_ClearList);
				UI_RECT_COLLECTION_clear(&dirty_regions[i]);
				new_region = UI_RECT_new_xyxy(0, 0, gc->image.width - 1, gc->image.height - 1);
			}
			LOG_REGION(UI_LOG_BRS_AddRegion, &new_region);
			UI_RECT_COLLECTION_add_rect(&dirty_regions[i], new_region);
		}
	}

#ifdef UI_FEATURE_BRS_FLUSH_SINGLE_RECTANGLE
	flush_bounds.x1 = MIN(flush_bounds.x1, region->x1);
	flush_bounds.y1 = MIN(flush_bounds.y1, region->y1);
	flush_bounds.x2 = MAX(flush_bounds.x2, region->x2);
	flush_bounds.y2 = MAX(flush_bounds.y2, region->y2);
#endif // UI_FEATURE_BRS_FLUSH_SINGLE_RECTANGLE
}

/*
 * @brief Checks if the past has to be restored and if the collection that represents
 * the past has to be cleared.
 *
 * The past may be not restored if the new drawing region includes the past (no need to
 * restore) or if the current back buffer is the same buffer than before last flush()
 * (because it already contains the past).
 *
 * @param[in] gc the MicroUI GraphicsContext that targets the current back buffer
 * @param[in] dirty_region the region of the next drawing.
 * @param[out] clear_past true if the caller has to clear the past collection.
 * @return true if the past has to be restored.
 */
static bool _check_restore(MICROUI_GraphicsContext *gc, const ui_rect_t *dirty_region, bool *clear_past) {
	bool restore;
	*clear_past = false;

	if (!UI_RECT_COLLECTION_is_empty(&dirty_regions[index_dirty_region_to_restore])) {
		// remains something to restore
		restore = true;

		if (!UI_RECT_is_empty(dirty_regions[index_dirty_region_to_restore].data)) {
			// very first call after a flush (not a SNI callback)

			if (LLUI_DISPLAY_getBufferAddress(LLUI_DISPLAY_getSourceImage(&gc->image)) !=
			    LLUI_DISPLAY_getBufferAddress(&gc->image)) {
				// target a new buffer

				if ((dirty_region->x1 == 0) && (dirty_region->y1 == 0) && (dirty_region->x2 == (gc->image.width - 1)) &&
				    (dirty_region->y2 == (gc->image.height - 1))) {
					// new dirty region fits the full screen; the "past" is useless
					*clear_past = true;
					restore = false;
				}
			} else {
				// target the same buffer: restore is useless and just add the new dirty region (do not clear the past)
				restore = false;
			}
		}
		// else: continue the restore
	} else {
		// nothing to restore
		restore = false;
	}
	return restore;
}

/*
 * @brief Restores the rectangles stored in the array of 4 rectangles (a region previously splitted in four
 * rectangles).
 *
 * @param[in] gc the MicroUI GraphicsContext that targets the current back buffer
 * @param[in] previous_buffer the MicroUI Image that targets the current front buffer
 * @return DRAWING_RUNNING if the a restoration is step is running or DRAWING_DONE if the restoration
 * is fully completed.
 */
static DRAWING_Status _restore_sub_rect(MICROUI_GraphicsContext *gc, MICROUI_Image *previous_buffer) {
	DRAWING_Status restore_status = DRAWING_DONE;
	int ri = 0;
	do {
		ui_rect_t *r = &diff[ri];
		if (!UI_RECT_is_empty(r)) {
			LOG_REGION(UI_LOG_BRS_RestoreRegion, r);
			restore_status = UI_DISPLAY_BRS_restore(gc, previous_buffer, r);
			UI_RECT_mark_empty(r);
		}
		ri++;
	} while ((DRAWING_DONE == restore_status) && (ri < 4));
	return restore_status;
}

/*
 * @brief Prepares the back buffer before the very first drawing, which consists to
 * restore the past if it is required (see _check_restore()).
 *
 * The restoration steps are symbolized by a collection of rectangles. The restoration
 * of one rectangle might be asynchronous. In that case, this function launches a
 * restoration step and returns false. The complete restoration will be completed
 * until this function returns true.
 *
 * @param[in] gc the MicroUI GraphicsContext that targets the current back buffer
 * @param[in] dirty_region the region of the next drawing.
 * @return DRAWING_RUNNING if the a restoration is step is running or DRAWING_DONE if the restoration
 * is fully completed and the region added as region to restore
 */
static DRAWING_Status _prepare_back_buffer(MICROUI_GraphicsContext *gc, ui_rect_t *dirty_region) {
	DRAWING_Status restore_status = DRAWING_DONE;
	bool clear_past = false;

	if (_check_restore(gc, dirty_region, &clear_past)) {
		MICROUI_Image *previous_buffer = LLUI_DISPLAY_getSourceImage(&gc->image);
		LLUI_DISPLAY_configureClip(gc, false); // regions to restore can be fully out of clip

		// restore sub parts of previous restoration (if any)
		restore_status = _restore_sub_rect(gc, previous_buffer);

		if (DRAWING_DONE == restore_status) {
			ui_rect_t *r = dirty_regions[index_dirty_region_to_restore].data;
			while ((DRAWING_DONE == restore_status) &&
			       (r != UI_RECT_COLLECTION_get_end(&dirty_regions[index_dirty_region_to_restore]))) {
				if (!UI_RECT_is_empty(r)) {
					// split (if required) the current region in sub-parts
					if (0u < UI_RECT_subtract(diff, r, dirty_region)) {
						// restore sub-part(s)
						restore_status = _restore_sub_rect(gc, previous_buffer);
					}

					// current region is now restored or is currently in restore (thanks to array of sub-parts)
					UI_RECT_mark_empty(r);
				}
				r++;
			}

			// do not clear the past if a restoration step is running
			clear_past = DRAWING_DONE == restore_status;
		}
		// else: the restoration of a sub-part is running
	}

	if (clear_past) {
		UI_RECT_COLLECTION_clear(&dirty_regions[index_dirty_region_to_restore]);
#if UI_FEATURE_BRS_DRAWING_BUFFER_COUNT > 1
		index_dirty_region_to_restore++;
		index_dirty_region_to_restore %= (uint32_t)UI_FEATURE_BRS_DRAWING_BUFFER_COUNT - 1u;
#endif
	}

	if (DRAWING_DONE == restore_status) {
		// back buffer is considered as ready when it is fully restored
		backbuffer_ready = true;

		// add the new region as a region to restore
		_add_drawing_region(gc, dirty_region);
	}

	return restore_status;
}

// --------------------------------------------------------------------------------
// LLUI_DISPLAY_impl.h API
// --------------------------------------------------------------------------------
/*
 * @brief See the header file for the function documentation.
 *
 * This function restores the back buffer if required and store the new region as new
 * region to restore after next call to flush().
 */
DRAWING_Status LLUI_DISPLAY_IMPL_newDrawingRegion(MICROUI_GraphicsContext *gc, ui_rect_t *region, bool drawing_now) {
	LOG_REGION(UI_LOG_BRS_NewDrawing, region);

	DRAWING_Status ret = DRAWING_DONE;

	if (!backbuffer_ready) {
		if (!drawing_now) {
			_remove_drawing_regions(gc, region);
		} else {
			ret = _prepare_back_buffer(gc, region);
		}
	} else {
		_add_drawing_region(gc, region); // don't care if drawing now or not
	}

	return ret;
}

/*
 * @brief See the header file for the function documentation.
 *
 * This function calls LLUI_DISPLAY_IMPL_flush() with the rectangle that includes all dirty regions.
 */
DRAWING_Status LLUI_DISPLAY_IMPL_refresh(MICROUI_GraphicsContext *gc, uint8_t flushIdentifier) {
#ifdef UI_FEATURE_BRS_FLUSH_SINGLE_RECTANGLE

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

#else // UI_FEATURE_BRS_FLUSH_SINGLE_RECTANGLE

	size_t size = UI_RECT_COLLECTION_get_length(&dirty_regions[index_dirty_region_to_restore]);
	if (1u == size) {
		ui_rect_t *rect = &dirty_regions[index_dirty_region_to_restore].data[0];
		LLTRACE_record_event_u32x6(LLUI_EVENT_group, LLUI_EVENT_offset + UI_LOG_BRS_FlushSingle, flushIdentifier,
		                           (uint32_t)LLUI_DISPLAY_getBufferAddress(&gc->image), rect->x1, rect->y1, rect->x2,
		                           rect->y2);
	} else {
		LLTRACE_record_event_u32x3(LLUI_EVENT_group, LLUI_EVENT_offset + UI_LOG_BRS_FlushMulti, flushIdentifier,
		                           (uint32_t)LLUI_DISPLAY_getBufferAddress(&gc->image), size);
	}

	LLUI_DISPLAY_IMPL_flush(gc, flushIdentifier, dirty_regions[index_dirty_region_to_restore].data,
	                        UI_RECT_COLLECTION_get_length(&dirty_regions[index_dirty_region_to_restore]));

#endif // UI_FEATURE_BRS_FLUSH_SINGLE_RECTANGLE

	backbuffer_ready = false;
	return DRAWING_DONE;
}

#endif // UI_FEATURE_BRS_PREDRAW

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------
