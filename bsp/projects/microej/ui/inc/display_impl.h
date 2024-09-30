/*
 * C
 *
 * Copyright 2022-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief MicroEJ MicroUI library low level API: implementation over VG-Lite
 * @author MicroEJ Developer Team
 * @version 6.0.1
 *
 * @brief This file lists a set of functions called by the display*.c files. First functions
 * are mandatory; optional functions are listed in a second time.
 *
 * These functions are
 * optional and a stub implementation is already available in the CCO (see display_stub.c).
 */

#if !defined DISPLAY_IMPL_H
#define DISPLAY_IMPL_H

#if defined __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>

#include "display_framebuffer.h"

// --------------------------------------------------------------------------------
// Functions that must be implemented
// --------------------------------------------------------------------------------

/*
 * Same signature than interrupts.h
 */
uint8_t interrupt_enter(void);

/*
 * Same signature than interrupts.h
 */
void interrupt_leave(uint8_t leave);

/*
 * Same signature than interrupts.h
 */
uint8_t interrupt_is_in(void);

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // !defined DISPLAY_IMPL_H
