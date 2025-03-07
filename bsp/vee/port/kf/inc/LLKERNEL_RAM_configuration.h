/*
 * C
 *
 * Copyright 2023-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief LLKERNEL RAM implementation.
 * @author MicroEJ Development Team
 * @version 3.0.0
 */

#ifndef  LLKERNEL_RAM_CONFIGURATION_H
#define  LLKERNEL_RAM_CONFIGURATION_H

#include <stdint.h>

/**
 * @brief LLKERNEL log level.
 *
 */
#define LLKERNEL_LOG_LEVEL LLKERNEL_LOG_ERROR

/**
 * @brief Kernel RAM allocation mode:
 * - KERNEL_RAM_IMPL_MALLOC with malloc()/free(): recommended configuration on Linux systems.
 * - KERNEL_RAM_IMPL_BESTFIT with best fit allocator: recommended configuration on MCUs.
 *
 */
#define KERNEL_RAM_IMPL_BESTFIT
#ifndef KERNEL_RAM_IMPL_BESTFIT
#define KERNEL_RAM_IMPL_MALLOC
#endif

/**
 * @brief Uncomment this macro to set {@link #LLKERNEL_MAX_NB_DYNAMIC_FEATURES}
 * value to a custom value.
 */
//#define LLKERNEL_MAX_NB_DYNAMIC_FEATURES	XX

#endif // LLKERNEL_RAM_CONFIGURATION_H
