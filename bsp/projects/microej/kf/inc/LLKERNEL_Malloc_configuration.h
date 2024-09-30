/*
 * C
 *
 * Copyright 2023-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief LLKERNEL implementation over C standard library (malloc/free) configuration.
 * @author MicroEJ Developer Team
 * @version 2.0.1
 */

#ifndef  LLKERNEL_MALLOC_CONFIGURATION_H
#define  LLKERNEL_MALLOC_CONFIGURATION_H

/**@brief LLKERNEL log level */
#define LLKERNEL_LOG_LEVEL LLKERNEL_LOG_ERROR

/**
 * @brief Uncomment this macro to set {@link #LLKERNEL_MAX_NB_DYNAMIC_FEATURES}
 * value to a custom value.
 */
//#define LLKERNEL_MAX_NB_DYNAMIC_FEATURES	XX

#endif // LLKERNEL_MALLOC_CONFIGURATION_H
