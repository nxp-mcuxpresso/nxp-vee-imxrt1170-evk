/*
 * C
 *
 * Copyright 2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef  LLML_MICROAI_HEAP_H
#define  LLML_MICROAI_HEAP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * @brief MicroAI implementation over TensorFlow Lite.
 * @author MicroEJ Developer Team
 * @version 2.1.0
 */

/**
 * This static size heap is defined by java microai.heap.size property
 */
extern uint8_t _microai_heap_start[];
extern uint8_t _microai_heap_end[];

void LLML_microai_heap_init(void);

void * LLML_microai_heap_allocate(uint32_t size);

void LLML_microai_heap_free(void *block);

#ifdef __cplusplus
}
#endif

#endif // LLML_MICROAI_HEAP_H
