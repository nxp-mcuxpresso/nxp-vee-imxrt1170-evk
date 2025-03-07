/*
 * C
 *
 * Copyright 2023-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief LLKERNEL RAM implementation.
 * @author MicroEJ Development Team
 * @version 3.0.0
 */

#ifndef LLKERNEL_RAM_H
#define LLKERNEL_RAM_H

#include "LLKERNEL_RAM_configuration.h"

/**@brief Log priority levels */
#define LLKERNEL_LOG_DEBUG      0
#define LLKERNEL_LOG_INFO       1
#define LLKERNEL_LOG_WARNING    2
#define LLKERNEL_LOG_ERROR      3
#define LLKERNEL_LOG_ASSERT     4
#define LLKERNEL_LOG_NONE       5

#ifndef LLKERNEL_LOG_LEVEL
	#error "LLKERNEL_LOG_LEVEL must be defined"
#endif

#if (LLKERNEL_LOG_NONE > LLKERNEL_LOG_LEVEL)
// cppcheck-suppress [misra-c2012-21.6] use of malloc is required
#include "fsl_debug_console.h"
#endif

/**@brief Debug logger */
#if (LLKERNEL_LOG_DEBUG >= LLKERNEL_LOG_LEVEL)
	#define LLKERNEL_DEBUG_LOG        (void)PRINTF("[LLKERNEL][D] "); (void)PRINTF
#else
	#define LLKERNEL_DEBUG_LOG(...)   ((void)0)
#endif

/**@brief Info logger */
#if (LLKERNEL_LOG_INFO >= LLKERNEL_LOG_LEVEL)
	#define LLKERNEL_INFO_LOG         (void)PRINTF("[LLKERNEL][I] "); (void)PRINTF
#else
	#define LLKERNEL_INFO_LOG(...)    ((void)0)
#endif

/**@brief Warning logger */
#if (LLKERNEL_LOG_WARNING >= LLKERNEL_LOG_LEVEL)
	#define LLKERNEL_WARNING_LOG      (void)PRINTF("[LLKERNEL][W] "); (void)PRINTF
#else
	#define LLKERNEL_WARNING_LOG(...) ((void)0)
#endif

/**@brief Error logger */
#if (LLKERNEL_LOG_ERROR >= LLKERNEL_LOG_LEVEL)
	#define LLKERNEL_ERROR_LOG        (void)PRINTF("[LLKERNEL][E] %s:%d ", __FILE__, __LINE__); (void)PRINTF
#else
	#define LLKERNEL_ERROR_LOG(...)   ((void)0)
#endif

/**@brief Assert logger */
#if (LLKERNEL_LOG_ASSERT >= LLKERNEL_LOG_LEVEL)
	#define LLKERNEL_ASSERT_LOG(Format, ...)       (void)PRINTF("[LLKERNEL][A] " Format, __VA_ARGS__); while (1)
#else
	#define LLKERNEL_ASSERT_LOG(...)  ((void)0)
#endif

#ifdef KERNEL_RAM_IMPL_BESTFIT
#include <limits.h>
#include "BESTFIT_ALLOCATOR.h"

/**
 * @brief Bestfit allocator initialization, to be called before starting MicroEJ VEE.
 *
 * @param start_address the start address of the kernel buffer.
 * @param end_address the start address of the kernel buffer.
 */
void LLKERNEL_RAM_BESTFIT_initialize(int32_t start_address, int32_t end_address);

/**
 * @brief Check the size to be allocated before bestfit allocation.
 *
 * @param size the size of the memory to allocate.
 * @return void* the pointer to the allocated memory.
 */
void * BESTFIT_ALLOCATOR_allocate_with_check(int32_t size);
#endif // KERNEL_RAM_IMPL_BESTFIT

/**
 * @brief Kernel memory allocation macro.
 *
 */
#ifdef KERNEL_RAM_IMPL_MALLOC
#define KERNEL_MALLOC(size) malloc((size_t)(size))
#else
#ifdef KERNEL_RAM_IMPL_BESTFIT
#define KERNEL_MALLOC(size) BESTFIT_ALLOCATOR_allocate_with_check(size)
#endif // KERNEL_RAM_IMPL_BESTFIT
#endif // KERNEL_RAM_IMPL_MALLOC

/**
 * @brief Kernel memory free macro.
 *
 */
#ifdef KERNEL_RAM_IMPL_MALLOC
#define KERNEL_FREE(addr) free(addr)
#else
#ifdef KERNEL_RAM_IMPL_BESTFIT
#define KERNEL_FREE(addr) BESTFIT_ALLOCATOR_free(&bestfit_allocator_instance, addr)
#endif // KERNEL_RAM_IMPL_BESTFIT
#endif // KERNEL_RAM_IMPL_MALLOC

/**@brief Maximum number of Features that can be dynamically installed */
#ifndef LLKERNEL_MAX_NB_DYNAMIC_FEATURES
// set {@link #LLKERNEL_MAX_NB_DYNAMIC_FEATURES} value to the value of the MicroEJ
// Kernel maximum number of Features that can be dynamically installed.
// See
// https://docs.microej.com/en/latest/ApplicationDeveloperGuide/applicationOptions.html#option-maximum-number-of-dynamic-features
extern int32_t _java_max_nb_dynamic_features;
	#define LLKERNEL_MAX_NB_DYNAMIC_FEATURES ((int32_t)&_java_max_nb_dynamic_features)
#endif // LLKERNEL_MAX_NB_DYNAMIC_FEATURES

#endif // LLKERNEL_RAM_H
