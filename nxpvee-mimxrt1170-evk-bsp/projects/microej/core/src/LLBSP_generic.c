/*
 * C
 *
 * Copyright 2014-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief Generic LLBSP implementation.
 * @author MicroEJ Developer Team
 * @version 1.1.0
 */

#include <stdint.h>
#include <stdio.h>

#include "LLBSP_impl.h"

#include "fsl_debug_console.h"

#ifdef __cplusplus
	extern "C" {
#endif

extern void SOAR_START(void);
extern void SOAR_END(void);

#if defined(MICROEJ_MULTIAPP)
extern void _java_installed_features_text_start(void);
extern void _java_installed_features_text_size(void);
#endif // MICROEJ_MULTIAPP

/**
 * @brief Writes the character <code>c</code>, cast to an unsigned char, to stdout stream.
 * This function is used by the default implementation of the Java <code>System.out</code>.
 */
void LLBSP_IMPL_putchar(int32_t c){
	PUTCHAR(c);
}

#ifdef __cplusplus
	}
#endif
