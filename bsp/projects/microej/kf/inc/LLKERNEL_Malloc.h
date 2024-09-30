/*
 * C
 *
 * Copyright 2023-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief LLKERNEL implementation over C standard library (malloc/free).
 * @author MicroEJ Developer Team
 * @version 2.0.1
 */

#include "LLKERNEL_Malloc_configuration.h"
#include "fsl_debug_console.h"

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
#include <stdio.h>
#endif

/**@brief Debug logger */
#if (LLKERNEL_LOG_DEBUG >= LLKERNEL_LOG_LEVEL)
	#define LLKERNEL_DEBUG_LOG        (void)PRINTF("[LLKERNEL][D] ");(void)PRINTF
#else
	#define LLKERNEL_DEBUG_LOG(...)   ((void) 0)
#endif

/**@brief Info logger */
#if (LLKERNEL_LOG_INFO >= LLKERNEL_LOG_LEVEL)
	#define LLKERNEL_INFO_LOG         (void)PRINTF("[LLKERNEL][I] ");(void)PRINTF
#else
	#define LLKERNEL_INFO_LOG(...)    ((void) 0)
#endif

/**@brief Warning logger */
#if (LLKERNEL_LOG_WARNING >= LLKERNEL_LOG_LEVEL)
	#define LLKERNEL_WARNING_LOG      (void)PRINTF("[LLKERNEL][W] ");(void)PRINTF
#else
	#define LLKERNEL_WARNING_LOG(...) ((void) 0)
#endif

/**@brief Error logger */
#if (LLKERNEL_LOG_ERROR >= LLKERNEL_LOG_LEVEL)
	#define LLKERNEL_ERROR_LOG        (void)PRINTF("[LLKERNEL][E] %s:%d ", __FILE__, __LINE__);(void)PRINTF
#else
	#define LLKERNEL_ERROR_LOG(...)   ((void) 0)
#endif

/**@brief Assert logger */
#if (LLKERNEL_LOG_ASSERT >= LLKERNEL_LOG_LEVEL)
	#define LLKERNEL_ASSERT_LOG(Format, ...)       (void)PRINTF("[LLKERNEL][A] " Format, __VA_ARGS__);while(1)
#else
	#define LLKERNEL_ASSERT_LOG(...)  ((void) 0)
#endif

/**@brief Maximum number of Features that can be dynamically installed */
#ifndef LLKERNEL_MAX_NB_DYNAMIC_FEATURES
	// set {@link #LLKERNEL_MAX_NB_DYNAMIC_FEATURES} value to the value of the MicroEJ
	// Kernel maximum number of Features that can be dynamically installed.
	// See https://docs.microej.com/en/latest/ApplicationDeveloperGuide/applicationOptions.html#option-maximum-number-of-dynamic-features
	extern void _java_max_nb_dynamic_features;
	#define LLKERNEL_MAX_NB_DYNAMIC_FEATURES	((int)(& _java_max_nb_dynamic_features))
#endif
