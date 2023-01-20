/*
 * C
 *
 * Copyright 2015-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * Implement the *_impl* functions according the CPU/OS/Compiler
 */

#ifndef FRAMERATE_IMPL
#define FRAMERATE_IMPL

/* Includes ------------------------------------------------------------------*/

#include "framerate.h"

/* API -----------------------------------------------------------------------*/

#ifdef FRAMERATE_ENABLED

/*
 * Framerate task have just to call this function
 */
void framerate_task_work(void);

/*
 * Create and start a framerate task
 */
int32_t framerate_impl_start_task(void);

/*
 * Sleep the framerate task
 */
void framerate_impl_sleep(uint32_t ms);

#endif	// FRAMERATE_ENABLED

#endif	// FRAMERATE_IMPL
