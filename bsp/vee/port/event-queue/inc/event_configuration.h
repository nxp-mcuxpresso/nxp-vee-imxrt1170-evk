/*
 * C
 *
 * Copyright 2023-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef  EVENT_CONFIGURATION_H
#define  EVENT_CONFIGURATION_H

#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Max number of events in the queue.
 */
#define EVENT_QUEUE_SIZE (100U)

/**
 * Event function succeeded.
 */
#define EVENT_OK         (0U)

/**
 * An error was detected during the method execution.
 */
#define EVENT_NOK (-1)

#define LLEVENT_PUMP_LOGLEVEL_NONE               (0)
#define LLEVENT_PUMP_LOGLEVEL_ERROR              (LLEVENT_PUMP_LOGLEVEL_NONE + 1)
#define LLEVENT_PUMP_LOGLEVEL_WARNING            (LLEVENT_PUMP_LOGLEVEL_ERROR + 1)
#define LLEVENT_PUMP_LOGLEVEL_INFO               (LLEVENT_PUMP_LOGLEVEL_WARNING + 1)
#define LLEVENT_PUMP_LOGLEVEL_DEBUG              (LLEVENT_PUMP_LOGLEVEL_INFO + 1)

#define LLEVENT_PUMP_LOGLEVEL                    (LLEVENT_PUMP_LOGLEVEL_ERROR) // Set log level

/**
 * @brief EVENT log macros
 */
#if LLEVENT_PUMP_LOGLEVEL >= LLEVENT_PUMP_LOGLEVEL_DEBUG
#define LLEVENT_DEBUG_TRACE(...)      (void)printf("[DEB] (%s:%d) ", __func__, __LINE__); (void)printf(__VA_ARGS__)
#else
#define LLEVENT_DEBUG_TRACE(...)      ((void)0)
#endif
#if LLEVENT_PUMP_LOGLEVEL >= LLEVENT_PUMP_LOGLEVEL_INFO
#define LLEVENT_INFO_TRACE(...)       (void)printf("[INF] (%s:%d) ", __func__, __LINE__); (void)printf(__VA_ARGS__)
#else
#define LLEVENT_INFO_TRACE(...)       ((void)0)
#endif
#if LLEVENT_PUMP_LOGLEVEL >= LLEVENT_PUMP_LOGLEVEL_ERROR
#define LLEVENT_ERROR_TRACE(...)      (void)printf("[ERR] "); (void)printf(__VA_ARGS__)
#else
#define LLEVENT_ERROR_TRACE(...)      ((void)0)
#endif

#ifdef __cplusplus
}
#endif

#endif // EVENT_CONFIGURATION_H
