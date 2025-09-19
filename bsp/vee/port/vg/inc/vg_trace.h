/*
 * C
 *
 * Copyright 2023-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#if !defined VG_TRACE_H
#define VG_TRACE_H

#if defined __cplusplus
extern "C" {
#endif

/*
 * @file
 * @brief Provides the logs of the CCO MicroVG.
 * @author MicroEJ Developer Team
 * @version 7.0.1
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "LLTRACE_impl.h"

// -----------------------------------------------------------------------------
// Defines
// -----------------------------------------------------------------------------

/*
 * @brief Available number of events: IMAGE, FONT and DRAWING
 */
#define LOG_MICROVG_EVENTS 3

/*
 * Events identifiers
 */
#define LOG_MICROVG_IMAGE_ID 0
#define LOG_MICROVG_FONT_ID 1
#define LOG_MICROVG_DRAWING_ID 2

/*
 * @brief Types of Image events
 */
#define LOG_MICROVG_IMAGE_load 0
#define LOG_MICROVG_IMAGE_create 1
#define LOG_MICROVG_IMAGE_close 2

/*
 * @brief Types of Font events
 */
#define LOG_MICROVG_FONT_load 0
#define LOG_MICROVG_FONT_baseline 1
#define LOG_MICROVG_FONT_height 2
#define LOG_MICROVG_FONT_stringWidth 3
#define LOG_MICROVG_FONT_stringHeight 4

/*
 * @brief Types of Drawing events
 */
#define LOG_MICROVG_DRAW_path 0
#define LOG_MICROVG_DRAW_pathGradient 1
#define LOG_MICROVG_DRAW_string 2
#define LOG_MICROVG_DRAW_stringGradient 3
#define LOG_MICROVG_DRAW_stringOnCircle 4
#define LOG_MICROVG_DRAW_stringOnCircleGradient 5
#define LOG_MICROVG_DRAW_image 6

/*
 * @brief Useful macros to concatenate easily some strings and defines.
 */
#define CONCAT_STRINGS(p, s) p ## s
#define CONCAT_DEFINES(p, s) CONCAT_STRINGS(p, s)

/*
 * @brief Macro to add an event and its type.
 */
#define LOG_MICROVG_START(event, type) LLTRACE_IMPL_record_event_u32(VG_TRACE_group_id, event, type);

/*
 * @brief Macro to notify the end of an event and its type.
 */
#define LOG_MICROVG_END(event, type) LLTRACE_IMPL_record_event_end_u32(VG_TRACE_group_id, event, type);

/* The following lines must be added to a SYSVIEW_MicroVG.txt file
 * in the <SYSTEMVIEW instalation dir>/Description folder
 *
 * NamedType VGImage 0=LOAD_IMAGE
 * NamedType VGImage 1=CREATE_IMAGE
 * NamedType VGImage 2=CLOSE_IMAGE
 *
 * NamedType VGFont 0=LOAD_FONT
 * NamedType VGFont 1=FONT_BASELINE
 * NamedType VGFont 2=FONT_HEIGHT
 * NamedType VGFont 3=STRING_WIDTH
 * NamedType VGFont 4=STRING_HEIGHT
 *
 * NamedType VGDraw 0=DRAW_PATH
 * NamedType VGDraw 1=DRAW_PATH_GRADIENT
 * NamedType VGDraw 2=DRAW_STRING
 * NamedType VGDraw 3=DRAW_STRING_GRADIENT
 * NamedType VGDraw 4=DRAW_STRING_ON_CIRCLE
 * NamedType VGDraw 5=DRAW_STRING_ON_CIRCLE_GRADIENT
 * NamedType VGDraw 6=DRAW_IMAGE
 *
 * 0        VG_ImageEvent      (MicroVG) Execute image event %VGImage  | (MicroVG) Image event %VGImage done
 * 1        VG_FontEvent       (MicroVG) Execute font event %VGFont  | (MicroVG) Font event %VGFont done
 * 2        VG_DrawingEvent    (MicroVG) Execute drawing event %VGDraw  | (MicroVG) Drawing event %VGDraw done
 *
 */

// -----------------------------------------------------------------------------
// Extern symbols
// -----------------------------------------------------------------------------

/*
 * @brief External variable that contains the id for each trace group.
 */
extern int32_t VG_TRACE_group_id;

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // !defined VG_TRACE_H
