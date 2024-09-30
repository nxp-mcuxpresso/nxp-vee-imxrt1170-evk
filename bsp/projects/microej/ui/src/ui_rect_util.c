/*
 * C
 *
 * Copyright 2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief Utility functions to handle rectangles
 *
 * @see ui_rect_t
 *
 * @author MicroEJ Developer Team
 * @version 4.1.0
 */

// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------

#include <assert.h>

#include "ui_util.h"
#include "ui_rect_util.h"

// --------------------------------------------------------------------------------
// Functions
// --------------------------------------------------------------------------------

// See the header file for the function documentation
ui_rect_t UI_RECT_get_minimum_bounding_rect(const ui_rect_t rects[], const size_t count) {
	assert(count > 0u);
	ui_rect_t result = rects[0];
	for (size_t i = 1; i < count; i++) {
		result = UI_RECT_get_minimum_bounding_rect_two_rects(&result, &rects[i]);
	}
	return result;
}

// See the header file for the function documentation
uint32_t UI_RECT_union(ui_rect_t output[3], const ui_rect_t *first, const ui_rect_t *second) {
	uint32_t ret;
	if (UI_RECT_contains_rect(first, second)) {
		output[0] = *first;
		UI_RECT_mark_empty(&output[1]);
		UI_RECT_mark_empty(&output[2]);
		ret = 1u;
	} else if (UI_RECT_contains_rect(second, first)) {
		output[0] = *second;
		UI_RECT_mark_empty(&output[1]);
		UI_RECT_mark_empty(&output[2]);
		ret = 1u;
	} else if (!UI_RECT_intersects_rect(first, second)) {
		output[0] = *first;
		output[1] = *second;
		UI_RECT_mark_empty(&output[2]);
		ret = 2u;
	} else {
		const ui_rect_t *top_above = (first->y1 < second->y1) ? first : second;
		const ui_rect_t *top_below = (first->y1 < second->y1) ? second : first;
		const ui_rect_t *bottom_below = (first->y2 > second->y2) ? first : second;
		const ui_rect_t *bottom_above = (first->y2 > second->y2) ? second : first;

		uint32_t count = 0u;

		output[count] = UI_RECT_new_xyxy(top_above->x1, top_above->y1, top_above->x2, top_below->y1 - 1);
		if (!UI_RECT_is_empty(output)) {
			count++;
		}

		output[count] = UI_RECT_new_xyxy(MIN(first->x1, second->x1), top_below->y1, MAX(first->x2, second->x2),
		                                 bottom_above->y2);
		if (!UI_RECT_is_empty(output)) {
			count++;
		}

		output[count] = UI_RECT_new_xyxy(bottom_below->x1, bottom_above->y2 + 1, bottom_below->x2, bottom_below->y2);
		if (!UI_RECT_is_empty(output)) {
			count++;
		}

		ret = count;
	}
	return ret;
}

// See the header file for the function documentation
uint32_t UI_RECT_subtract(ui_rect_t output[4], const ui_rect_t *first, const ui_rect_t *second) {
	uint32_t ret;
	if (UI_RECT_contains_rect(first, second)) {
		output[0] = UI_RECT_new_xyxy(first->x1, first->y1, first->x2, second->y1 - 1); // Top
		output[1] = UI_RECT_new_xyxy(first->x1, second->y1, second->x1 - 1, second->y2); // Left
		output[2] = UI_RECT_new_xyxy(second->x2 + 1, second->y1, first->x2, second->y2); // Right
		output[3] = UI_RECT_new_xyxy(first->x1, second->y2 + 1, first->x2, first->y2); // Bottom
		ret = 4u;
	} else if (UI_RECT_contains_rect(second, first)) {
		UI_RECT_mark_empty(&output[0]);
		UI_RECT_mark_empty(&output[1]);
		UI_RECT_mark_empty(&output[2]);
		UI_RECT_mark_empty(&output[3]);
		ret = 0u;
	} else if (!UI_RECT_intersects_rect(first, second)) {
		output[0] = *first;
		UI_RECT_mark_empty(&output[1]);
		UI_RECT_mark_empty(&output[2]);
		UI_RECT_mark_empty(&output[3]);
		ret = 1u;
	} else {
		output[0] = UI_RECT_new_xyxy(first->x1, first->y1, first->x2, second->y1 - 1); // Top
		output[1] = UI_RECT_new_xyxy(first->x1, MAX(first->y1, second->y1), second->x1 - 1, MIN(first->y2, second->y2)); // Left
		output[2] = UI_RECT_new_xyxy(second->x2 + 1, MAX(first->y1, second->y1), first->x2, MIN(first->y2, second->y2)); // Right
		output[3] = UI_RECT_new_xyxy(first->x1, second->y2 + 1, first->x2, first->y2); // Bottom
		ret = 4u;
	}
	return ret;
}
