/*
 * C
 *
 * Copyright 2021-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief Implementation of vg_path.h functions
 *
 * @author MicroEJ Developer Team
 * @version 8.0.1
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "ui_vglite.h"
#include "vg_path.h"
#include "vg_drawing_vglite.h"
#include "vg_helper.h"
#include "vg_vglite_helper.h"

// -----------------------------------------------------------------------------
// Defines
// -----------------------------------------------------------------------------

#ifndef VG_FEATURE_PATH
#error "This implementation is only compatible when VG_FEATURE_PATH is set"
#endif

/*
 * @brief Specific error code when VGLite library throws an error.
 */
#define RET_ERROR_VGLITE -1

/*
 * @brief Specific error code when gradient data is invalid.
 */
#define RET_ERROR_GRADIENT -2

// -----------------------------------------------------------------------------
// vg_path.h functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
uint8_t VG_PATH_get_path_encoder_format(void) {
	return VG_LITE_FP32;
}

// See the header file for the function documentation
uint32_t VG_PATH_convert_path_command(jint command) {
	uint32_t ret;
	switch (command) {
	default: // unknown -> close (should not occur)
	case LLVG_PATH_CMD_CLOSE:
		ret = VLC_OP_END;
		break;

	case LLVG_PATH_CMD_MOVE:
		ret = VLC_OP_MOVE;
		break;

	case LLVG_PATH_CMD_MOVE_REL:
		ret = VLC_OP_MOVE_REL;
		break;

	case LLVG_PATH_CMD_LINE:
		ret = VLC_OP_LINE;
		break;

	case LLVG_PATH_CMD_LINE_REL:
		ret = VLC_OP_LINE_REL;
		break;

	case LLVG_PATH_CMD_QUAD:
		ret = VLC_OP_QUAD;
		break;

	case LLVG_PATH_CMD_QUAD_REL:
		ret = VLC_OP_QUAD_REL;
		break;

	case LLVG_PATH_CMD_CUBIC:
		ret = VLC_OP_CUBIC;
		break;

	case LLVG_PATH_CMD_CUBIC_REL:
		ret = VLC_OP_CUBIC_REL;
		break;
	}
	return ret;
}

// See the header file for the function documentation
uint32_t VG_PATH_get_command_parameter_number(jint command) {
	uint32_t ret;
	switch (command) {
	default: // unknown -> close (should not occur)
	case VLC_OP_END:
	case VLC_OP_CLOSE:
		ret = 0;
		break;

	case VLC_OP_MOVE:
	case VLC_OP_MOVE_REL:
	case VLC_OP_LINE:
	case VLC_OP_LINE_REL:
		ret = 2;
		break;

	case VLC_OP_QUAD:
	case VLC_OP_QUAD_REL:
		ret = 4;
		break;

	case VLC_OP_CUBIC:
	case VLC_OP_CUBIC_REL:
		ret = 6;
		break;
	}
	return ret;
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------
