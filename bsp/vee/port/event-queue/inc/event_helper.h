/*
 * C
 *
 * Copyright 2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef  EVENT_HELPER_H
#define  EVENT_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * @brief LLEVENT helper file that stores defines and function prototypes.
 * @author MicroEJ Developer Team
 * @version 1.1.1
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Macros and defines
// -----------------------------------------------------------------------------

/**
 * Event function succeeded.
 */
#define EVENT_OK (0)

/**
 * An error was detected during the method execution.
 */
#define EVENT_NOK (-1)

// -----------------------------------------------------------------------------
// Function prototypes
// -----------------------------------------------------------------------------

jbyte read_one_byte(void);
jshort read_two_bytes(void);
jint read_four_bytes(void);
jlong read_eight_bytes(void);

// -----------------------------------------------------------------------------
// End
// -----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // EVENT_CONFIGURATION_H
