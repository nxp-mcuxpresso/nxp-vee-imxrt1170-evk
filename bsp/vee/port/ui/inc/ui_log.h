/*
 * Copyright 2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#if !defined UI_LOG_H
#define UI_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief Provides elements that allow to log some external events in the MicroUI event
 * group.
 *
 * The MicroUI event group is identified by the global LLUI_EVENT_group. The events are
 * 0-based and have to start at the offset LLUI_EVENT_offset.
 *
 * Notes:
 * - The number of events is 100 (fixed by MicroUI).
 * - The event 0 is reserved to log the drawings. It is used by the MicroUI CCO and by
 * the MicroUI Graphics Engine (to log the internal drawings).
 * - The events [10,20] are reserved to log the buffer refresh strategies (BRS) events.
 *
 * Example:
 *
 * 		#include "ui_log.h"
 * 		LLTRACE_record_event_u32(LLUI_EVENT_group, LLUI_EVENT_offset + MY_EVENT_OFFSET, my_event_data);
 *
 * @author MicroEJ Developer Team
 * @version 14.2.0
 */

// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------

/*
 * @brief Includes right header file according the Architecture.
 * - Architecture 7: include "trace.h"
 * - Architecture 8: include "LLTRACE.h"
 */
#define UI_LOG_LLTRACE __has_include("LLTRACE.h")
#if UI_LOG_LLTRACE == (0u)
#include <trace.h>
#else
#include <LLTRACE.h>
#endif

// --------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------

/*
 * @brief Identifies the log for the drawings (known by the MicroUI CCO and by the
 * MicroUI Graphics Engine).
 */
#define UI_LOG_DRAW (0)

/*
 * @brief Identifies the logs for the Buffer Refresh Strategies (BRS).
 */
#define UI_LOG_BRS_NewDrawing    (10)   // New drawing region (%u,%u) to (%u,%u)
#define UI_LOG_BRS_FlushSingle   (11)   // Flush LCD (id=%u buffer=%p) single region (%u,%u) to (%u,%u)
#define UI_LOG_BRS_FlushMulti    (12)   // Flush LCD (id=%u buffer=%p) %u regions
#define UI_LOG_BRS_AddRegion     (13)   // Add region (%u,%u) to (%u,%u)
#define UI_LOG_BRS_RemoveRegion  (14)   // Remove region (%u,%u) to (%u,%u)
#define UI_LOG_BRS_RestoreRegion (15)   // Restore region (%u,%u) to (%u,%u)
#define UI_LOG_BRS_ClearList     (16)   // Clear the list of regions

/*
 * @brief Compatibility of Architecture 7 with Architecture 8: use the prototypes
 * of LLTRACE.h (Architecture 8).
 */
#if UI_LOG_LLTRACE == (0u)
#define LLTRACE_record_event_void TRACE_record_event_void
#define LLTRACE_record_event_u32 TRACE_record_event_u32
#define LLTRACE_record_event_u32x2 TRACE_record_event_u32x2
#define LLTRACE_record_event_u32x3 TRACE_record_event_u32x3
#define LLTRACE_record_event_u32x4 TRACE_record_event_u32x4
#define LLTRACE_record_event_u32x5 TRACE_record_event_u32x5
#define LLTRACE_record_event_u32x6 TRACE_record_event_u32x6
#define LLTRACE_record_event_u32x7 TRACE_record_event_u32x7
#define LLTRACE_record_event_u32x8 TRACE_record_event_u32x8
#define LLTRACE_record_event_u32x9 TRACE_record_event_u32x9
#define LLTRACE_record_event_u32x10 TRACE_record_event_u32x10
#define LLTRACE_record_event_end TRACE_record_event_end
#define LLTRACE_record_event_end_u32 TRACE_record_event_end_u32
#endif // if UI_LOG_LLTRACE == (0u)

// --------------------------------------------------------------------------------
// Fields
// --------------------------------------------------------------------------------

/*
 * @brief Identifies the MicroUI group to log an event.
 */
extern int32_t LLUI_EVENT_group;

/*
 * @brief Identifies the offset in the MicroUI group to log an event.
 */
extern int32_t LLUI_EVENT_offset;

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // UI_LOG_H
