/*
 * C
 *
 * Copyright 2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief MicroAI implementation over TensorFlow Lite.
 * @author MicroEJ Developer Team
 * @version 2.1.0
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BESTFIT_ALLOCATOR.h"
#include "LLML_configuration.h"
#include "LLML_microai_heap.h"

static BESTFIT_ALLOCATOR microai_heap;
static bool microai_heap_initialized = false;

void LLML_microai_heap_init(void) {
	if (!microai_heap_initialized) {
		BESTFIT_ALLOCATOR_new(&microai_heap);
		BESTFIT_ALLOCATOR_initialize(&microai_heap, (int32_t)_microai_heap_start, (int32_t)_microai_heap_end);
		microai_heap_initialized = true;
		LLML_DEBUG_TRACE("HEAP start: 0x%p end: 0x%p size: %d\n",
		                 (void *)_microai_heap_start, (void *)_microai_heap_end,
		                 _microai_heap_end - _microai_heap_start);
	}
}

void * LLML_microai_heap_allocate(uint32_t size) {
	LLML_DEBUG_TRACE("allocate %d\n", size);
	uint8_t *addr = (uint8_t *)BESTFIT_ALLOCATOR_allocate(&microai_heap, (int32_t)size);

	if ((uint8_t *)0 == addr) {
		LLML_DEBUG_TRACE("ERROR: Could not allocate size: %d\n", size);
	}
	return addr;
}

void LLML_microai_heap_free(void *block) {
	return BESTFIT_ALLOCATOR_free(&microai_heap, block);
}
