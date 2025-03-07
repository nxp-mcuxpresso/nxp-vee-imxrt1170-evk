/*
 * C
 *
 * Copyright 2021-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief Provides some API to analyse the MicroUI image heap. The default allocator available in the
 * Graphics Engine does not implement this file contrary to the allocator "LLUI_DISPLAY_HEAP_impl.c".
 * @see The option UI_FEATURE_ALLOCATOR in ui_configuration.h
 * @author MicroEJ Developer Team
 * @version 14.2.0
 * @since MicroEJ UI Pack 13.1.0
 */

#if !defined MICROUI_HEAP_H
#define MICROUI_HEAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ui_configuration.h"

#if defined(UI_FEATURE_ALLOCATOR)

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <stdint.h>

// -----------------------------------------------------------------------------
// API
// -----------------------------------------------------------------------------

/*
 * @brief Returns the MicroUI image heap size in bytes.
 */
uint32_t MICROUI_HEAP_total_space(void);

/*
 * @brief Returns the MicroUI image heap free space size in bytes.
 *
 * Warnings: The total free space cannot contain a block whose size is equal to
 * the total free space:
 * 	- The best fit allocator adds a header and a footer for each allocated
 * 	  block.
 * 	- Consecutive malloc/free produce cause memory fragmentation (all the free
 * 	  blocks are not contiguous in the memory). The function returns the sum of
 * 	  all the free blocks.
 */
uint32_t MICROUI_HEAP_free_space(void);

/*
 * @brief Returns the number of blocks allocated.
 *
 * The MicroUI image heap is mainly used to allocate the pixels buffers of MicroUI
 * ResourceImages (images decoded dynamically at runtime, copy of images located in
 * a non-byte addressable memory, and MicroUI BufferedImages).
 */
uint32_t MICROUI_HEAP_number_of_allocated_blocks(void);

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#endif // UI_FEATURE_ALLOCATOR

#ifdef __cplusplus
}
#endif

#endif // MICROUI_HEAP_H
