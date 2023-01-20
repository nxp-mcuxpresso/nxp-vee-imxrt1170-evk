/*
 * C
 *
 * Copyright 2015-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */
#ifndef FRAMERATE_INTERN
#define FRAMERATE_INTERN

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include "framerate_conf.h"

/* Defines -------------------------------------------------------------------*/

#define FRAMERATE_OK (int32_t)0
#define FRAMERATE_ERROR (int32_t)-1

#ifndef FRAMERATE_SCHEDULE_TIME
#define FRAMERATE_SCHEDULE_TIME 500	// ms
#endif

#define OK_CALL(fn, OK_val) (return_code) = (fn); if(return_code != (OK_val)) goto end_fn;
#define ERR_RETURN(err_code) (return_code) = (err_code); goto end_fn;
#define UNUSED(var) ((void) (var))

/* API -----------------------------------------------------------------------*/

/*
 * Initialize the framework
 */
int32_t framerate_init(int32_t schedule_time);

/*
 * Update the framerate counter
 */
void framerate_increment(void);

/*
 * Return the last framerate
 */
uint32_t framerate_get(void);

/* Default Java API ----------------------------------------------------------*/

#ifndef javaFramerateInit
#define javaFramerateInit		Java_com_is2t_debug_Framerate_init
#endif
#ifndef javaFramerateGet
#define javaFramerateGet		Java_com_is2t_debug_Framerate_get
#endif

#endif	// _FRAMERATE_INTERN
