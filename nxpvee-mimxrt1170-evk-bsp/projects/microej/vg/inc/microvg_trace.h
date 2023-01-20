/*
 * C
 *
 * Copyright 2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */


#if !defined MICROVG_TRACE_H
#define MICROVG_TRACE_H

#if defined __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "LLTRACE_impl.h"
#include "microvg_configuration.h"

/*
 * @brief Useful macros to concatenate easily some strings and defines.
 */
#define CONCAT_STRINGS(p, s) p ## s
#define CONCAT_DEFINES(p, s) CONCAT_STRINGS(p,s)

/*
 * @brief External variable that contains the id for each trace group
 *
 * To be initialised with this code lines:
     vg_trace_group_id = LLTRACE_IMPL_declare_event_group("MICROVG", 100);
 */
extern int32_t vg_trace_group_id;

/*
 * @brief External variable which is set when the execution trace is enabled
 * in the application launcher.
 *
 */
void LOG_DRAWING_OPERATION(void);

/*
 * @brief Wrapping functions for trace start and end
 *
 */
static inline void LLTRACE_start(int log_id, int event)
{
#if defined VG_FEATURE_LLTRACE
	if((int32_t)(&LOG_DRAWING_OPERATION) != -1){
		LLTRACE_IMPL_record_event_u32(vg_trace_group_id, log_id, event);
	}
#endif
}

static inline void LLTRACE_end(int log_id, int event)
{
#if defined VG_FEATURE_LLTRACE
	if((int32_t)(&LOG_DRAWING_OPERATION) != -1){
		LLTRACE_IMPL_record_event_end_u32(vg_trace_group_id, log_id, event);
	}
#endif
}

// macros
#define LOG_VG_START(fn) LLTRACE_start(LOG_VG_ID, CONCAT_DEFINES(LOG_VG_, fn))
#define LOG_VG_END(fn) LLTRACE_end(LOG_VG_ID, CONCAT_DEFINES(LOG_VG_, fn))

#define LOG_FT_START(fn) LLTRACE_start(LOG_FT_ID, CONCAT_DEFINES(LOG_FT_, fn))
#define LOG_FT_END(fn) LLTRACE_end(LOG_FT_ID, CONCAT_DEFINES(LOG_FT_, fn))

/*
 * LOG_VG_EVENT logs identifiers
 */

#define LOG_VG_ID 1

#define LOG_VG_load_font                            10
#define LOG_VG_string_width                         11
#define LOG_VG_string_height                        12
#define LOG_VG_get_baseline_position                13
#define LOG_VG_get_height                           14
#define LOG_VG_draw_string                          15
#define LOG_VG_draw_string_gradient                 16
#define LOG_VG_draw_string_on_circle                17
#define LOG_VG_draw_string_on_circle_gradient       18
#define LOG_VG_drawPath                             19
#define LOG_VG_drawGradient                         20
#define LOG_VG_initializeGradient                   21

/*
 * LOG_FT_EVENT logs identifiers
 */

#define LOG_FT_ID 2

#define LOG_FT_GPU_DRAW_PATH                        10
#define LOG_FT_GPU_DRAW_GRADIENT                    11

/* The following lines must be added to a SYSVIEW_MICROVG.txt file
   in the <SYSTEMVIEW instalation dir>/Description folder

1        VG    	(MicroVG) Drawing operation %VG | Drawing operation %VG Done
2        FT    	(Freetype) Freetype operation %FT |Freetype operation %FT Done

NamedType VG 10=LOAD_FONT
NamedType VG 11=STRING_WIDTH
NamedType VG 12=STRING_HEIGHT
NamedType VG 13=GET_BASELINE_POSITION
NamedType VG 14=GET_HEIGHT
NamedType VG 15=DRAW_STRING
NamedType VG 16=DRAW_STRING_GRADIENT
NamedType VG 17=DRAW_STRING_ON_CIRCLE
NamedType VG 18=DRAW_STRING_ON_CIRCLE_GRADIENT
NamedType VG 19=DRAWPATH
NamedType VG 20=DRAWGRADIENT
NamedType VG 21=INITIALIZEGRADIENT

NamedType FT 10=GPU_DRAW_PATH
NamedType FT 11=GPU_DRAW_GRADIENT

 */


// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // !defined MICROVG_TRACE_H
