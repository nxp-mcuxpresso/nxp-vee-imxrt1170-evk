/*
 * C
 *
 * Copyright 2023-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef UI_RECT_COLLECTION_H
#define UI_RECT_COLLECTION_H
#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief Exposes the ui_rect_collection_t type that handles unordered ui_rect_t collections
 */

// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------

#include <assert.h>

#include "ui_rect.h"

// --------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------

/*
 * @brief Defines the number of rectangles that collections can contain.
 */
#ifndef UI_RECT_COLLECTION_MAX_LENGTH
#define UI_RECT_COLLECTION_MAX_LENGTH 8u
#endif

// --------------------------------------------------------------------------------
// Typedefs
// --------------------------------------------------------------------------------

/*
 * @brief A rectangle collection contains an array of rectangles and a pointer to
 * the latest rectangle.
 */
typedef struct ui_rect_collection_t {
	ui_rect_t data[UI_RECT_COLLECTION_MAX_LENGTH];
	size_t length;
} ui_rect_collection_t;

// --------------------------------------------------------------------------------
// Public functions
// --------------------------------------------------------------------------------

/*
 * @brief Clears the collection: the latest rectangle points on the first rectangle.
 *
 * @param[in] collection the collection to clear
 */
static inline void UI_RECT_COLLECTION_clear(ui_rect_collection_t *collection) {
	collection->length = 0;
}

/*
 * @brief Initializes the collection.
 *
 * @param[in] collection the collection to initialize
 */
static inline void UI_RECT_COLLECTION_init(ui_rect_collection_t *collection) {
	UI_RECT_COLLECTION_clear(collection);
}

/*
 * @brief Gets the number of rectangles that have been added to the collection.
 *
 * @param[in] collection the collection to check
 *
 * @return the available number of rectangles in the collection
 */
static inline size_t UI_RECT_COLLECTION_get_length(const ui_rect_collection_t *collection) {
	return collection->length;
}

/*
 * @brief Tells if the collection is full.
 *
 * @param[in] collection the collection to check
 *
 * @return true when the collection is full
 */
static inline bool UI_RECT_COLLECTION_is_full(const ui_rect_collection_t *collection) {
	return UI_RECT_COLLECTION_get_length(collection) >= UI_RECT_COLLECTION_MAX_LENGTH;
}

/*
 * @brief Tells if the collection is empty.
 *
 * @param[in] collection the collection to check
 *
 * @return true when the collection is empty
 */
static inline bool UI_RECT_COLLECTION_is_empty(const ui_rect_collection_t *collection) {
	return collection->length == 0u;
}

/*
 * @brief Gets the last rectangle added to the collection or NULL if the collection is empty.
 *
 * @param[in] collection the collection where retrieve latest element
 *
 * @return a rectangle or NULL
 */
static inline ui_rect_t * UI_RECT_COLLECTION_get_last(ui_rect_collection_t *collection) {
	return UI_RECT_COLLECTION_is_empty(collection) ? NULL : (collection->data + collection->length - 1u);
}

/*
 * @brief Gets a pointer to the address after the last rectangle in the collection.
 *
 * @param[in] collection the collection
 *
 * @return a pointer to the address after the last element in the collection
 */
static inline ui_rect_t * UI_RECT_COLLECTION_get_end(ui_rect_collection_t *collection) {
	return collection->data + collection->length;
}

/*
 * @brief Adds a rectangle in the collection. The implementation assumes that caller ensure that
 * the collection is not full. The rectangle is copied to the collection.
 *
 * @param[in] collection the collection where copying the rectangle
 * @param[in] element the rectangle to add
 */
static inline void UI_RECT_COLLECTION_add_rect(ui_rect_collection_t *collection, const ui_rect_t element) {
	assert(!UI_RECT_COLLECTION_is_full(collection));
	collection->data[collection->length] = element;
	collection->length++;
}

/*
 * @brief Removes a rectangle from the collection. The behavior is undefined if the rectangle is not part of the
 * collection.
 *
 * @param[in] collection the collection which contains the rectangle
 * @param[in] element the rectangle to remove
 */
static inline void UI_RECT_COLLECTION_remove_rect(ui_rect_collection_t *collection, ui_rect_t *element) {
	assert(!UI_RECT_COLLECTION_is_empty(collection));
	assert(element >= collection->data);
	assert(element < (collection->data + collection->length));
	*element = *UI_RECT_COLLECTION_get_last(collection);
	collection->length--;
}

/*
 * @brief Removes all empty rectangles from the collection.
 *
 * @param[in] collection the collection
 */
static inline void UI_RECT_COLLECTION_remove_empty_rects(ui_rect_collection_t *collection) {
	for (size_t i = 0; i < collection->length;) {
		if (UI_RECT_is_empty(&collection->data[i])) {
			UI_RECT_COLLECTION_remove_rect(collection, &collection->data[i]);
		} else {
			i++;
		}
	}
}

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
#endif // UI_RECT_COLLECTION_H
