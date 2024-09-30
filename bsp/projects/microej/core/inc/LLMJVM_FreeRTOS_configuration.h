/*
 * C
 *
 * Copyright 2021-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef LLMJVM_FREERTOS_CONFIGURATION_H
#define LLMJVM_FREERTOS_CONFIGURATION_H

/* Defines used to include FreeRTOS API header files. Update it if header file location is different. */

#define FREERTOS_HEADER				   "FreeRTOS.h"
#define FREERTOS_TIMERS_HEADER		   "timers.h"
#define FREERTOS_SEMPHR_HEADER		   "semphr.h"
#define FREERTOS_TASK_HEADER		   "task.h"
#define YIELD_FROM_ISR(x)			   portYIELD_FROM_ISR(x)
#define IS_INSIDE_INTERRUPT			   xPortIsInsideInterrupt

/**
* @brief LLMJVM log level macros
*/
#define LLMJVM_LOGLEVEL_NONE           (0)
#define LLMJVM_LOGLEVEL_ERROR          (LLMJVM_LOGLEVEL_NONE  + 1)
#define LLMJVM_LOGLEVEL_INFO           (LLMJVM_LOGLEVEL_ERROR + 1)
#define LLMJVM_LOGLEVEL_DEBUG          (LLMJVM_LOGLEVEL_INFO  + 1)

/**
* @brief LLMJVM set log level
*/
#define LLMJVM_LOGLEVEL                LLMJVM_LOGLEVEL_ERROR

/**
* @brief LLMJVM log traces activation
*/
#if LLMJVM_LOGLEVEL > LLMJVM_LOGLEVEL_NONE
// cppcheck-suppress [misra-c2012-21.6]: Include only with traces activation
#include <stdio.h>
#define LLMJVM_TRACE(...)              (void)printf(__VA_ARGS__)
#endif

#if LLMJVM_LOGLEVEL >= LLMJVM_LOGLEVEL_DEBUG
#define LLMJVM_DEBUG_TRACE(...)        LLMJVM_TRACE(__VA_ARGS__);
#else
#define LLMJVM_DEBUG_TRACE(...)        ((void) 0)
#endif
#if LLMJVM_LOGLEVEL >= LLMJVM_LOGLEVEL_INFO
#define LLMJVM_INFO_TRACE(...)         LLMJVM_TRACE(__VA_ARGS__);
#else
#define LLMJVM_INFO_TRACE(...)         ((void) 0)
#endif
#if LLMJVM_LOGLEVEL >= LLMJVM_LOGLEVEL_ERROR
#define LLMJVM_ERROR_TRACE(...)        LLMJVM_TRACE(__VA_ARGS__);
#else
#define LLMJVM_ERROR_TRACE(...)        ((void) 0)
#endif

#endif /* LLMJVM_FREERTOS_CONFIGURATION_H */
