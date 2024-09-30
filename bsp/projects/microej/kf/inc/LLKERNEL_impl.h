/*
 * C
 *
 * Copyright 2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#include <stdlib.h>
#include <string.h>

#ifndef LLKERNEL_IMPL
#define LLKERNEL_IMPL

#ifdef __cplusplus
extern "C" {
#endif

int32_t LLKERNEL_IMPL_allocateFeature(int32_t size_ROM, int32_t size_RAM);

void LLKERNEL_IMPL_freeFeature(int32_t handle);

int32_t LLKERNEL_IMPL_getAllocatedFeaturesCount(void);

int32_t LLKERNEL_IMPL_getFeatureHandle(int32_t allocation_index);

void* LLKERNEL_IMPL_getFeatureAddressRAM(int32_t handle);

void* LLKERNEL_IMPL_getFeatureAddressROM(int32_t handle);

int32_t LLKERNEL_IMPL_copyToROM(void* dest_address_ROM, void* src_address, int32_t size);

int32_t LLKERNEL_IMPL_flushCopyToROM(void);

int32_t LLKERNEL_IMPL_onFeatureInitializationError(int32_t handle, int32_t error_code);

#ifdef __cplusplus
}
#endif

#endif /* LLKERNEL_IMPL */
