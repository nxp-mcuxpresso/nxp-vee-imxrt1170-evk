/*
 * C
 *
 * Copyright 2015-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */ 
#ifndef CPULOAD_INTERN
#define CPULOAD_INTERN

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include "cpuload_conf.h"


/* Defines -------------------------------------------------------------------*/

#ifndef CPULOAD_SCHEDULE_TIME
#define CPULOAD_SCHEDULE_TIME 500	// ms
#endif

#define CPULOAD_OK 					 (int32_t)0
#define CPULOAD_NOT_ENABLED			(int32_t)-1
#define CPULOAD_INVALID_COUNTER 	(int32_t)-2
#define CPULOAD_START_TASK_ERROR 	(int32_t)-3

/* API -----------------------------------------------------------------------*/

/*
 * Initialize the framework
 * Must be called in very first OS stack. No more task must run
 * at same time.
 */
int32_t cpuload_init(void);

/*
 * Must be called by the OS idle function
 */
void cpuload_idle(void);

/*
 * Return the last CPU load measure
 */
uint32_t cpuload_get(void);

/* Default Java API ----------------------------------------------------------*/

#ifndef javaCPULoadInit
#define javaCPULoadInit		Java_com_is2t_debug_CPULoad_init
#endif

#ifndef javaCPULoadGet
#define javaCPULoadGet		Java_com_is2t_debug_CPULoad_get
#endif

#endif	// CPULOAD_INTERN
