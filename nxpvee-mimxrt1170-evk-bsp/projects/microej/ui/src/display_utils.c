/*
 * C
 *
 * Copyright 2020-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief MicroEJ MicroUI library low level API: implementation over VG-Lite
 * @author MicroEJ Developer Team
 * @version 4.0.0
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <stdint.h>

#include "display_utils.h"

// -----------------------------------------------------------------------------
// Global Variables
// -----------------------------------------------------------------------------

/*
 * @brief Look Up Table to translate MicroUI image to BPP
 */
static const int __microui_to_bpp[] = {
		(5+6+5),	// MICROUI_IMAGE_FORMAT_LCD = 0 = RGB565,
		DISPLAY_UNKNOWN_FORMAT,	// UNKNOWN = 1,
		(4*8),		            // MICROUI_IMAGE_FORMAT_ARGB8888 = 2,
		DISPLAY_UNKNOWN_FORMAT,	// MICROUI_IMAGE_FORMAT_RGB888 = 3, unsupported
		(5+6+5),			    // MICROUI_IMAGE_FORMAT_RGB565 = 4,
		(1+3*5),	// MICROUI_IMAGE_FORMAT_ARGB1555 = 5,
		(4*4),		            // MICROUI_IMAGE_FORMAT_ARGB4444 = 6,
		4,	// MICROUI_IMAGE_FORMAT_A4 = 7,
		8,	// MICROUI_IMAGE_FORMAT_A8 = 8,
		// outside of the table ... MICROUI_IMAGE_FORMAT_CUSTOM = 255,
};

// -----------------------------------------------------------------------------
// display_util.h functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
int32_t DISPLAY_UTILS_get_bpp(MICROUI_ImageFormat image_format)
{
	int bpp = DISPLAY_UNKNOWN_FORMAT;

	// cppcheck-suppress [misra-c2012-10.8] retrieve the array's length
	if ((int) image_format < (int)(sizeof(__microui_to_bpp) / sizeof(__microui_to_bpp[0]))) {
		bpp = __microui_to_bpp[image_format];
	}

	return (DISPLAY_UNKNOWN_FORMAT == bpp) ? -1 : bpp;
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------
