/*
 * C
 *
 * Copyright 2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef UI_UTIL_H
#define UI_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @file
 * @brief Utility functions
 *
 * @author MicroEJ Developer Team
 * @version 14.2.0
 */

// -----------------------------------------------------------------------------
// Macros and defines
// -----------------------------------------------------------------------------

/*
 * @brief Gets the lowest value.
 */
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/*
 * @brief Gets the highest value.
 */
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // UI_UTIL_H
