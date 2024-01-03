/*
 * C
 *
 * Copyright 2015-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/* Includes ------------------------------------------------------------------*/

#include "cpuload_impl.h"
#include "fsl_debug_console.h"

#ifdef CPULOAD_DEBUG
// cppcheck-suppress misra-c2012-21.6 // Include only in debug
#include <stdio.h>
#define CPULOAD_DEBUG_TRACE(...) (void)PRINTF(__VA_ARGS__)
#else
#define CPULOAD_DEBUG_TRACE(...) ((void)0)
#endif

/* Globals -------------------------------------------------------------------*/

#ifdef CPULOAD_ENABLED
static volatile uint32_t cpuload_schedule_time = 0;
static volatile uint32_t cpuload_last_load = 0;
static volatile uint32_t cpuload_reference_counter = 0;
static volatile uint32_t cpuload_idle_counter = 0;
static volatile uint32_t cpuload_ask_counter = 0;
#endif

/* API -----------------------------------------------------------------------*/

void cpuload_idle(void)
{
#ifdef CPULOAD_ENABLED
	__disable_irq();
	cpuload_idle_counter++;
	__enable_irq();
#endif
}


int32_t cpuload_init(void)
{
#ifdef CPULOAD_ENABLED

	int32_t return_code = CPULOAD_OK;

	// get reference time (must be done before creating task)
	return_code = cpuload_impl_start_idle_task();
	if (return_code == CPULOAD_OK) {
		cpuload_impl_sync_os_tick();
		cpuload_idle_counter = 0;
		cpuload_impl_sleep(CPULOAD_SCHEDULE_TIME/10);
		
		if (cpuload_idle_counter == (uint32_t)0){
			// it is an error: this counter must have been updated
			// during the previous sleep.
			PRINTF("CPU load startup: invalid idle counter value: %u\n", cpuload_idle_counter);
			return_code = CPULOAD_INVALID_COUNTER;
		} else {

		//CPULOAD_DEBUG_TRACE("CPU load startup: valid idle counter value: %u\n", cpuload_idle_counter);
		// fix globals
		cpuload_schedule_time = CPULOAD_SCHEDULE_TIME;
		cpuload_reference_counter = (cpuload_idle_counter*(uint32_t)10)+(uint32_t)5;
		cpuload_idle_counter = 0;
		cpuload_last_load = 0;
		cpuload_ask_counter = 0;

		return_code = cpuload_impl_start_task();
		PRINTF("return code is %d \n",return_code);
		}
	}

	return return_code;

#else
	return CPULOAD_NOT_ENABLED;
#endif
}

uint32_t cpuload_get(void)
{
#ifdef CPULOAD_ENABLED
	uint32_t ret = cpuload_last_load;
	// clear average
	cpuload_ask_counter = 0;
	PRINTF("%d", ret);
	return ret;
#else
	return 0;
#endif
}

void cpuload_task(void)
{
#ifdef CPULOAD_ENABLED
	while(1)
	{
		// sleep during n milliseconds
		cpuload_impl_sleep(cpuload_schedule_time);

		// save global variables into local ones as they are volatile and might change while computing the average
		uint32_t cpuload_last_load_tmp = cpuload_last_load;
		uint32_t cpuload_reference_counter_tmp = cpuload_reference_counter;
		uint32_t cpuload_idle_counter_tmp = cpuload_idle_counter;
		uint32_t cpuload_ask_counter_tmp = cpuload_ask_counter;

		// average computing
		uint32_t average_compute = cpuload_last_load_tmp * cpuload_ask_counter_tmp;
		uint32_t last_average = (uint32_t)100 - (((uint32_t )100 * cpuload_idle_counter_tmp) / cpuload_reference_counter_tmp);
		cpuload_ask_counter++;
		cpuload_last_load = (average_compute + last_average) / cpuload_ask_counter;

		// reset cpuload counter
		cpuload_idle_counter = 0;
	}
#endif
}


/* Java API ------------------------------------------------------------------*/

uint32_t javaCPULoadInit(void)
{
	return cpuload_init();
}

uint32_t javaCPULoadGet(void)
{
	return cpuload_get();
}
