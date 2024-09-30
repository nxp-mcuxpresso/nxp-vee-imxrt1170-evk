/*
 * C
 *
 * Copyright 2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef UI_RECT_UTIL_H
#define UI_RECT_UTIL_H

/*
 * @file
 * @brief Utility functions to handle rectangles
 *
 * @see ui_rect_t
 *
 * @author MicroEJ Developer Team
 * @version 4.1.0
 */

#ifdef __cplusplus
extern "C" {
#endif

// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------

#include <stddef.h>

#include "ui_rect.h"
#include "ui_util.h"

// --------------------------------------------------------------------------------
// Constants
// --------------------------------------------------------------------------------

#define UI_RECT_EMPTY { 1u, 1u, 0u, 0u }

// --------------------------------------------------------------------------------
// Functions
// --------------------------------------------------------------------------------

/*
 * @brief Computes the minimum bounding rectangle of an array of two rectangles
 *
 * The behavior is undefined if the array is empty.
 *
 * @param[in] first the first rectangle
 * @param[in] second the second rectangle
 * @return the minimum bounding rectangle
 */
static inline ui_rect_t UI_RECT_get_minimum_bounding_rect_two_rects(const ui_rect_t *first, const ui_rect_t *second) {
	return UI_RECT_new_xyxy(MIN(first->x1, second->x1), MIN(first->y1, second->y1), MAX(first->x2, second->x2),
	                        MAX(first->y2, second->y2));
}

/*
 * @brief Computes the minimum bounding rectangle of an array of rectangles
 *
 * The behavior is undefined if the array is empty.
 *
 * @param[in] rects the array of rectangles
 * @param[in] count the length of the array
 * @return the minimum bounding rectangle
 */
ui_rect_t UI_RECT_get_minimum_bounding_rect(const ui_rect_t rects[], const size_t count);

/*
 * @brief Computes the union of two rectangles
 *
 * @param[out] output rectangles defining the union of the parameters
 * @param[in] first the first rectangle
 * @param[in] second the second rectangle
 *
 * @return the number of resulting rectangles. Some of them may be empty.
 */
uint32_t UI_RECT_union(ui_rect_t output[3], const ui_rect_t *first, const ui_rect_t *second);

/*
 * @brief Computes the difference between two rectangles
 *
 * @param[out] rectangles defining the difference between the parameters
 * @param[in] first the original rectangle
 * @param[in] second the rectangle that will be removed from the original one
 *
 * @return the number of resulting rectangles. Some of them may be empty.
 */
uint32_t UI_RECT_subtract(ui_rect_t output[4], const ui_rect_t *first, const ui_rect_t *second);

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // UI_RECT_UTIL_H
