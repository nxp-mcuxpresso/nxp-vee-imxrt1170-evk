/*
 * C
 *
 * Copyright 2018-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief LLKERNEL RAM implementation.
 * @author MicroEJ Development Team
 * @version 3.0.1
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "LLKERNEL_RAM.h"
#include "LLKERNEL_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

// Utility macros for allocating RAM and ROM areas with required alignment constraints
#define KERNEL_AREA_GET_MAX_SIZE(size, alignment) ((size) + ((alignment) - 1))
#define KERNEL_AREA_GET_START_ADDRESS(addr, \
									  alignment) ((void *)((((int32_t)(addr)) + (alignment) - 1) & ~((alignment) - 1)))

typedef struct installed_feature {
	void *ROM_area;
	void *RAM_area;
	int32_t ROM_area_size;
	int32_t RAM_area_size;
	struct installed_feature *next;
} installed_feature_t;

static installed_feature_t *first_installed_feature = NULL;

static int32_t nb_allocated_features = 0;

static const char *_error_code_to_str(uint32_t error_code) {
	const char *str = "";
	switch (error_code) {
	case LLKERNEL_FEATURE_INIT_ERROR_CORRUPTED_CONTENT:
		str = "CORRUPTED CONTENT";
		break;
	case LLKERNEL_FEATURE_INIT_ERROR_INCOMPATIBLE_KERNEL_WRONG_UID:
		str = "INCOMPATIBLE_KERNEL_WRONG_UID";
		break;
	case LLKERNEL_FEATURE_INIT_ERROR_TOO_MANY_INSTALLED:
		str = "TOO_MANY_INSTALLED";
		break;
	case LLKERNEL_FEATURE_INIT_ERROR_ALREADY_INSTALLED:
		str = "ALREADY_INSTALLED";
		break;
	case LLKERNEL_FEATURE_INIT_ERROR_INCOMPATIBLE_KERNEL_WRONG_ADDRESSES:
		str = "INCOMPATIBLE_KERNEL_WRONG_ADDRESSES";
		break;
	case LLKERNEL_FEATURE_INIT_ERROR_ROM_OVERLAP:
		str = "ROM_OVERLAP";
		break;
	case LLKERNEL_FEATURE_INIT_ERROR_RAM_OVERLAP:
		str = "RAM_OVERLAP";
		break;
	case LLKERNEL_FEATURE_INIT_ERROR_RAM_ADDRESS_CHANGED:
		str = "RAM_ADDRESS_CHANGED";
		break;
	default:
		LLKERNEL_ASSERT_LOG("No LLKERNEL error code found for %d\n", (int)error_code);
		break;
	}
	return str;
}

static int8_t _is_valid_feature_handle(installed_feature_t *handle) {
	installed_feature_t *ptr_feature = first_installed_feature;
	uint8_t found = 0;

	while (NULL != ptr_feature) {
		if (ptr_feature == handle) {
			// Feature handle is the first feature in the list
			found = 1;
			break;
		} else {
			// Go to the next feature
			ptr_feature = ptr_feature->next;
		}
	}
	return found;
}

#ifdef KERNEL_RAM_IMPL_BESTFIT
/**
 * @brief Bestfit allocator instance.
 *
 */
static BESTFIT_ALLOCATOR bestfit_allocator_instance;

void LLKERNEL_RAM_BESTFIT_initialize(int32_t start_address, int32_t end_address) {
	BESTFIT_ALLOCATOR_new(&bestfit_allocator_instance);
	BESTFIT_ALLOCATOR_initialize(&bestfit_allocator_instance, start_address, end_address);
}

void * BESTFIT_ALLOCATOR_allocate_with_check(int32_t size) {
	void *result = NULL;
	if ((0 < size) && (INT_MAX > size)) {
		result = BESTFIT_ALLOCATOR_allocate(&bestfit_allocator_instance, (int32_t)(size));
	}
	return result;
}

#endif // KERNEL_RAM_IMPL_BESTFIT

int32_t LLKERNEL_IMPL_allocateFeature(int32_t size_ROM, int32_t size_RAM) {
	LLKERNEL_DEBUG_LOG("%s(%d, %d)\n", __func__, size_ROM, size_RAM);
	int32_t ret = 0;

	if (nb_allocated_features == LLKERNEL_MAX_NB_DYNAMIC_FEATURES) {
		LLKERNEL_WARNING_LOG("Max number of dynamic features installed reached\n");
	} else {
		int total_size = sizeof(struct installed_feature);
		total_size += KERNEL_AREA_GET_MAX_SIZE(size_ROM, LLKERNEL_ROM_AREA_ALIGNMENT);
		total_size += KERNEL_AREA_GET_MAX_SIZE(size_RAM, LLKERNEL_RAM_AREA_ALIGNMENT);

		installed_feature_t *total_area = (installed_feature_t *)KERNEL_MALLOC(total_size);
		if (NULL != total_area) {
			// Add new allocated feature at the end of the installed features list
			installed_feature_t *f = (installed_feature_t *)total_area;
			if (NULL == first_installed_feature) {
				first_installed_feature = f;
			} else {
				installed_feature_t *ptr_feature = first_installed_feature;
				while (NULL != ptr_feature->next) {
					ptr_feature = ptr_feature->next;
				}
				ptr_feature->next = f;
			}

			++nb_allocated_features;

			// Initialize new feature
			f->ROM_area = KERNEL_AREA_GET_START_ADDRESS((void *)(((int32_t)f) + ((int32_t)sizeof(installed_feature_t))),
			                                            LLKERNEL_ROM_AREA_ALIGNMENT);
			f->ROM_area_size = size_ROM;
			f->RAM_area = KERNEL_AREA_GET_START_ADDRESS((void *)(((int32_t)f->ROM_area) + size_ROM),
			                                            LLKERNEL_RAM_AREA_ALIGNMENT);
			f->RAM_area_size = size_RAM;
			f->next = NULL;
			ret = (int32_t)f;
		}
		// else Out of memory
	}
	return ret;
}

void LLKERNEL_IMPL_freeFeature(int32_t handle) {
	LLKERNEL_DEBUG_LOG("%s(0x%.8x)\n", __func__, handle);

	// Remove installed feature from the installed features list
	installed_feature_t *remove_feature = (installed_feature_t *)handle;
	installed_feature_t *ptr_feature = first_installed_feature;
	if (NULL == first_installed_feature) {
		LLKERNEL_ERROR_LOG("Feature free issue (no feature installed)\n");
	} else {
		if (remove_feature == first_installed_feature) {
			// Feature to remove is the first one in the installed features list.
			first_installed_feature = remove_feature->next;
			--nb_allocated_features;
			KERNEL_FREE((void *)handle);
		} else {
			// Search feature in the installed features list and remove it
			while (NULL != ptr_feature->next) {
				if (ptr_feature->next == remove_feature) {
					// Feature to remove found in the installed features list. Now remove it from the list.
					ptr_feature->next = remove_feature->next;
					--nb_allocated_features;
					break;
				}
				ptr_feature = ptr_feature->next;
			}
			if (ptr_feature->next != remove_feature->next) {
				LLKERNEL_ERROR_LOG("Feature free issue (feature not found)\n");
			} else {
				KERNEL_FREE((void *)handle);
			}
		}
	}
}

// cppcheck-suppress [misra-c2012-5.5] Macro name is configured in VEE Port headers.
int32_t LLKERNEL_IMPL_getAllocatedFeaturesCount(void) {
	LLKERNEL_DEBUG_LOG("%s()\n", __func__);
	LLKERNEL_INFO_LOG("%d feature(s) allocated\n", nb_allocated_features);
	return nb_allocated_features;
}

int32_t LLKERNEL_IMPL_getFeatureHandle(int32_t allocation_index) {
	LLKERNEL_DEBUG_LOG("%s(%d)\n", __func__, allocation_index);
	int32_t ret = 0;

	installed_feature_t *ptr_feature = first_installed_feature;
	int32_t tmp_index = 0;
	while ((tmp_index != allocation_index) && (NULL != ptr_feature) && (NULL != ptr_feature->next)) {
		++tmp_index;
		ptr_feature = ptr_feature->next;
	}

	if ((tmp_index != allocation_index) || (NULL == ptr_feature)) {
		// Allocation index not in range of allocated features count
		LLKERNEL_ERROR_LOG("No feature found at index (allocation_index=%d)\n", allocation_index);
	} else {
		ret = (int32_t)ptr_feature;
	}
	return ret;
}

void * LLKERNEL_IMPL_getFeatureAddressRAM(int32_t handle) {
	LLKERNEL_DEBUG_LOG("%s(0x%.8x)\n", __func__, handle);
	void *ret = NULL;

	// Check if feature handle is valid (not freed)
	if (0 == _is_valid_feature_handle((installed_feature_t *)handle)) {
		LLKERNEL_ERROR_LOG("Feature handle not valid (handle=0x%.8x\n", (unsigned int)handle);
	} else {
		ret = ((installed_feature_t *)handle)->RAM_area;
	}
	return ret;
}

void * LLKERNEL_IMPL_getFeatureAddressROM(int32_t handle) {
	LLKERNEL_DEBUG_LOG("%s(0x%.8x)\n", __func__, handle);
	void *ret = NULL;

	// Check if feature handle is valid (not freed)
	if (0 == _is_valid_feature_handle((installed_feature_t *)handle)) {
		LLKERNEL_ERROR_LOG("Feature handle not valid (handle=0x%.8x\n", (unsigned int)handle);
	} else {
		ret = ((installed_feature_t *)handle)->ROM_area;
	}
	return ret;
}

int32_t LLKERNEL_IMPL_copyToROM(void *dest_address_ROM, void *src_address, int32_t size) {
	LLKERNEL_DEBUG_LOG("%s(0x%.8x, 0x%.8x, %d)\n", __func__, (unsigned int)dest_address_ROM, (unsigned int)src_address,
	                   size);
	int32_t ret = LLKERNEL_ERROR;

	// Check function parameters consistency
	if ((NULL == dest_address_ROM) || (NULL == src_address) || (size < 0)) {
		LLKERNEL_ERROR_LOG("Wrong parameters passed\n");
	} else {
		// Check that copy to ROM area match an allocated installed feature ROM area
		installed_feature_t *ptr_feature = first_installed_feature;
		do{
			uint32_t ptr_feature_ROM_area_end = ((uint32_t)ptr_feature->ROM_area) +
			                                    ((uint32_t)ptr_feature->ROM_area_size);
			if ((size <= ptr_feature->ROM_area_size) &&
			    (((uint32_t)dest_address_ROM) >= ((uint32_t)ptr_feature->ROM_area)) &&
			    (((uint32_t)dest_address_ROM) < ptr_feature_ROM_area_end) &&
			    (((uint32_t)dest_address_ROM + ((uint32_t)size)) <= ptr_feature_ROM_area_end)) {
				// ROM destination address match an allocated installed feature ROM area
				// cppcheck-suppress [misra-c2012-17.7] no need to check memcpy return value here
				memcpy(dest_address_ROM, src_address, size);
				break;
			}
			ptr_feature = ptr_feature->next;
		} while (NULL != ptr_feature);

		if (NULL == ptr_feature) {
			LLKERNEL_ERROR_LOG("ROM destination address do not match LLKERNEL installed feature ROM area\n");
		} else {
			ret = LLKERNEL_OK;
		}
	}
	return ret;
}

// cppcheck-suppress [misra-c2012-5.5] Macro name is configured in VEE Port headers.
int32_t LLKERNEL_IMPL_flushCopyToROM(void) {
	LLKERNEL_DEBUG_LOG("%s()\n", __func__);
	// Nothing to do here, flush already done
	return LLKERNEL_OK;
}

int32_t LLKERNEL_IMPL_onFeatureInitializationError(int32_t handle, int32_t error_code) {
	LLKERNEL_ERROR_LOG("Failed to initialize feature handle 0x%.8x with error %d(%s)\n", (int)handle, (int)error_code,
	                   _error_code_to_str(error_code));
	switch (error_code) {
	case LLKERNEL_FEATURE_INIT_ERROR_CORRUPTED_CONTENT:
		LLKERNEL_ERROR_LOG(
			"Feature detected but content is corrupted: automatically uninstalling the feature to free the memory.\n");
		LLKERNEL_IMPL_freeFeature(handle);
		break;
	case LLKERNEL_FEATURE_INIT_ERROR_INCOMPATIBLE_KERNEL_WRONG_UID:
		LLKERNEL_ERROR_LOG(
			"Feature detected but not compatible with the Kernel, automatically uninstalling the feature to free the memory.\n");
		LLKERNEL_IMPL_freeFeature(handle);
		break;
	case LLKERNEL_FEATURE_INIT_ERROR_INCOMPATIBLE_KERNEL_WRONG_ADDRESSES:
		LLKERNEL_ERROR_LOG(
			"Feature detected but addresses are wrong, automatically uninstalling the feature to free the memory.\n");
		LLKERNEL_IMPL_freeFeature(handle);
		break;
	default:
		break;
	}

	return LLKERNEL_OK;
}

#ifdef __cplusplus
}
#endif
