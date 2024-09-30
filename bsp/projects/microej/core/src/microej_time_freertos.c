/*
 * C
 *
 * Copyright 2018-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief MicroEJ Time APIs implementation for FreeRTOS.
 * @author MicroEJ Developer Team
 * @version 1.0.0
 */

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>

#include "microej.h"
#include "microej_time_freertos_configuration.h"
#include "microej_time.h"

#include FREERTOS_HEADER
#include FREERTOS_TASK_HEADER

#ifdef __cplusplus
extern "C" {
#endif

/* Defines -------------------------------------------------------------------*/

#define MICROSECONDS_PER_TICK (1000000 / (int64_t)configTICK_RATE_HZ)  // Number of microseconds per tick.
#define MILLISECONDS_PER_TICK (1000 / (int64_t)configTICK_RATE_HZ)  // Number of milliseconds per tick.
#define MILLISECONDS_TO_MICROSECONDS (1000) // Converts milliseconds to microseconds.
#define MILLISECONDS_TO_NANOSECONDS (1000000) // Converts milliseconds to nanoseconds.

#if (configUSE_TICKLESS_IDLE != 0)

// This implementation is not compatible with the tickless mode of FreeRTOS because
// the tick counter will not be incremented correctly when the system sleeps.
#error "OS Tickless mode not managed by this implementation"

#endif

/* Globals -------------------------------------------------------------------*/

/** Offset in milliseconds from system time to application time. */
static uint64_t microej_application_time_offset = 0;

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Converts a number of system ticks in milliseconds.
 * If an overflow occurs during the conversion, a saturated number of milliseconds is returned.
 * @param ticks the number of ticks to convert in milliseconds,
 * @return int64_t the time in milliseconds.
 */
static int64_t microej_time_tick_to_time(int64_t ticks) {
	int64_t time = 0;

	if (ticks >= 0) {
		// Check for no overflow
		if (ticks > (INT64_MAX / MILLISECONDS_PER_TICK)) {
			// An overflow occurs: saturate the value to int64_t max value
			time = INT64_MAX;
		} else {
			time = ticks * MILLISECONDS_PER_TICK;
		}
	}
	return time;
}

/* Public functions ----------------------------------------------------------*/

/**
 * @brief Initialisation of the timebase used by MicroEJ. Initialize in this function everything that is needed to
 * implement the API.
 *
 */
void microej_time_init(void) {
	/* Nothing to do here. */
}

/**
 * @brief Converts a time in milliseconds to a number of system ticks.
 * The result is round up to the next tick (ie. converting back the resulting ticks to milliseconds results in a value
 * greater than or equals to given time).
 * If an overflow occurs during the time conversion, a saturated number of ticks is returned.
 *
 * @param time the time to convert in milliseconds,
 * @return int64_t the number of ticks.
 */

int64_t microej_time_time_to_tick(int64_t time) {
	int64_t ticks_return = 0;

	if (time >= 0) {
		int8_t overflow_status = MICROEJ_TRUE;

		// Check for no overflow
		if (time <= (INT64_MAX / MILLISECONDS_TO_MICROSECONDS)) {
			int64_t time_microseconds = time * MILLISECONDS_TO_MICROSECONDS;

			// Check for no overflow
			if (time_microseconds < (INT64_MAX - MICROSECONDS_PER_TICK - 1)) {
				int64_t ticks64 = (time_microseconds + (MICROSECONDS_PER_TICK - 1)) / MICROSECONDS_PER_TICK;
				portTickType ticks = (portTickType)ticks64;

				// Check for no overflow
				// cppcheck-suppress [misra-c2012-10.4]: Check if no missing information during portTickType cast.
				if (ticks == ticks64) {
					overflow_status = MICROEJ_FALSE;
					ticks_return = (int64_t)ticks;
				}
			}
		}

		if (overflow_status == MICROEJ_TRUE) {
			// An overflow occurs: saturate the value to the max value for a portTickType
			ticks_return = (int64_t)portMAX_DELAY;
		}
	}

	return ticks_return;
}

/**
 * @brief Gets the current platform or application time.
 *
 * @param is_platform_time set to 1 to get the platform time or set to 0 to get the time elapsed since midnight, January
 * 1, 1970 UTC.
 * @return int64_t the time in milliseconds.
 */
int64_t microej_time_get_current_time(uint8_t is_platform_time) {
	int64_t ticks = (int64_t)xTaskGetTickCount();
	int64_t time = microej_time_tick_to_time(ticks);

	if (is_platform_time == (uint8_t)MICROEJ_FALSE) {
		time = time + (int64_t)microej_application_time_offset;
	}
	return time;
}

int64_t microej_time_get_time_nanos(void) {
	return microej_time_get_current_time(MICROEJ_TRUE) * MILLISECONDS_TO_NANOSECONDS;
}

/**
 * @brief Sets the application time.
 *
 * @param time value in milliseconds of the time elapsed since midnight, January 1, 1970 UTC.
 */
void microej_time_set_application_time(int64_t time) {
	int64_t currentTime = (int64_t)microej_time_get_current_time(MICROEJ_TRUE);
	microej_application_time_offset = time - currentTime;
}

#ifdef __cplusplus
}
#endif
