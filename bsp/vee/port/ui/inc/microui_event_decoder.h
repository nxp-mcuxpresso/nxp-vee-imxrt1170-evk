/*
 * C
 *
 * Copyright 2021-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief See microui_event_decoder.c.
 * @author MicroEJ Developer Team
 * @version 14.2.0
 * @since MicroEJ UI Pack 13.1.0
 */

#if !defined MICROUI_EVENT_DECODER_H
#define MICROUI_EVENT_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ui_configuration.h"

#if defined(UI_FEATURE_EVENT_DECODER)

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// -----------------------------------------------------------------------------
// Types
// -----------------------------------------------------------------------------

/*
 * @brief Function to decode the data of an event.
 *
 * @param[in] event the event that requires the data.
 * @param[in] data the event data.
 * @param[in] index the data index in queue.
 */
typedef void (* MICROUI_EVENT_DECODER_decode_event_data) (uint32_t event, uint32_t data, uint32_t index);

// -----------------------------------------------------------------------------
// API
// -----------------------------------------------------------------------------

/*
 * @brief Notifies the start of MicroUI FIFO dump.
 */
void MICROUI_EVENT_DECODER_describe_dump_start(void);

/*
 * @brief Notifies the next logs are already consumed.
 */
void MICROUI_EVENT_DECODER_describe_dump_past(void);

/*
 * @brief Notifies the next logs are not consumed yet.
 */
void MICROUI_EVENT_DECODER_describe_dump_future(void);

/*
 * @brief Notifies the dump will log the Java objects associated with the events
 * that are not consumed yet.
 */
void MICROUI_EVENT_DECODER_describe_dump_events_objects(void);

/*
 * @brief Notifies the end of MicroUI FIFO dump.
 */
void MICROUI_EVENT_DECODER_describe_dump_end(void);

/*
 * @brief Drops a data. This is an orphaned data of an old event already executed.
 */
void MICROUI_EVENT_DECODER_drop_data(uint32_t data, uint32_t index);

/*
 * @brief Decodes an event.
 *
 * @param[in] event the 32-bit event.
 * @param[in] index the event's index in queue.
 * @param[out] fct_data_decoder function to decode the data of the event.
 */
void MICROUI_EVENT_DECODER_decode_event(uint32_t event, uint32_t index,
                                        MICROUI_EVENT_DECODER_decode_event_data *fct_data_decoder);

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#endif // UI_FEATURE_EVENT_DECODER

#ifdef __cplusplus
}
#endif

#endif // MICROUI_EVENT_DECODER_H
