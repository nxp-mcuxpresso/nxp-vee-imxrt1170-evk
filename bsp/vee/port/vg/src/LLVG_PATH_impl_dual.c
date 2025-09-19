/*
 * C
 *
 * Copyright 2022-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief MicroVG library low level API: implementation of path.
 *
 * This implementation uses a 32-bit "integer" value to store a path command and
 * a 32-bit "float" value to store each command parameter.
 *
 * The encoding can be overridden, see "[optional]: weak functions" in "vg_path.h"
 *
 * @author MicroEJ Developer Team
 * @version 7.0.1
 */

#include "vg_configuration.h"

#if defined(VG_FEATURE_PATH) && defined(VG_FEATURE_PATH_DUAL_ARRAY) && (VG_FEATURE_PATH == VG_FEATURE_PATH_DUAL_ARRAY)

// TODO Merge with LLVG_PATH_impl.c

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <math.h>
#include <string.h>

#include <LLVG_PATH_impl.h>

#include "vg_path.h"
#include "vg_helper.h"
#include "bsp_util.h"

// -----------------------------------------------------------------------------
// Private functions
// -----------------------------------------------------------------------------

/*
 * @brief Extends the path to be able to store the command and its parameters.
 *
 * @return the offset in path buffer where the command will be stored. If the path
 * buffer is not large enough to contain the requested command, returns a negative
 * number corresponding to size the buffer must be enlarged for this command.
 */
static int32_t _extend_path(VG_PATH_HEADER_t *path, jint array_length, VG_path_command_t cmd, uint32_t nb_params) {
	size_t current_size = VG_PATH_get_path_header_size() + (path->param_length * sizeof(VG_path_param_t)) +
	                      (path->cmd_length * sizeof(VG_path_command_t));
	size_t required_size = current_size + VG_PATH_get_path_command_size(cmd, nb_params);

	int32_t ret;

	if (required_size > array_length) {
		ret = array_length - required_size;
	} else {
		if (
			// Parameters would write over the commands
			// or
			// Commands would go past the array end
			((uint8_t *)VG_PATH_get_path_param_end(path)) + (nb_params * sizeof(VG_path_param_t)) >
			(uint8_t *)VG_PATH_get_path_command_begin(path)
			|| ((uint8_t *)VG_PATH_get_path_command_end(path)) + sizeof(VG_path_command_t) >
			((uint8_t *)path) + array_length
			) {
			size_t new_cmd_offset = array_length - VG_PATH_get_path_header_size() - ((path->cmd_length + 1u) *
			                                                                         sizeof(VG_path_command_t));
			// memmove is used instead of memcpy as the source and destination are likely to overlap.
			(void)memmove(((uint8_t *)VG_PATH_get_path_param_begin(path)) + new_cmd_offset,
			              VG_PATH_get_path_command_begin(path), path->cmd_length * sizeof(VG_path_command_t));
			path->cmd_offset = new_cmd_offset;
		}
		ret = 0;
	}

	return ret;
}

static int32_t _close_path(VG_PATH_HEADER_t *path, jint length, jfloat x1, jfloat y1, jfloat x2, jfloat y2) {
	int32_t index = _extend_path(path, length, LLVG_PATH_CMD_CLOSE, 0);
	int32_t ret = LLVG_SUCCESS;
	if (index == 0) {
		// finalizes the path by storing the path's bounds
		path->bounds_xmin = x1;
		path->bounds_xmax = x2;
		path->bounds_ymin = y1;
		path->bounds_ymax = y2;
		(void)VG_PATH_append_path_command0((jbyte *)path, (uint32_t)index, LLVG_PATH_CMD_CLOSE);
	} else {
		// too small buffer, ret is the required extra size * -1
		ret = -index;
	}
	return ret;
}

// -----------------------------------------------------------------------------
// Specific path format functions [optional]: weak functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT void VG_PATH_initialize(void) {
	// nothing to do
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT uint32_t VG_PATH_get_path_header_size(void) {
	return sizeof(VG_PATH_HEADER_t);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT uint32_t VG_PATH_get_path_command_size(jint command, uint32_t nbParams) {
	(void)command;
	return (1u * sizeof(VG_path_command_t)) + (nbParams * sizeof(VG_path_param_t));
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT uint32_t VG_PATH_append_path_command0(jbyte *bytes, jint array_length, jint cmd) {
	(void)array_length;

	VG_PATH_HEADER_t *path = (VG_PATH_HEADER_t *)bytes;

	*VG_PATH_get_path_command_end(path) = VG_PATH_convert_path_command(cmd);

	path->cmd_length += 1u;

	return LLVG_SUCCESS;
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT uint32_t VG_PATH_append_path_command1(jbyte *bytes, jint array_length, jint cmd, jfloat x,
                                                            jfloat y) {
	(void)array_length;
	VG_PATH_HEADER_t *path = (VG_PATH_HEADER_t *)bytes;

	*VG_PATH_get_path_command_end(path) = VG_PATH_convert_path_command(cmd);

	VG_path_param_t *param_ptr = VG_PATH_get_path_param_end(path);
	*(param_ptr++) = x;
	*(param_ptr++) = y;

	path->cmd_length += 1u;
	path->param_length += 2u;

	return LLVG_SUCCESS;
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT uint32_t VG_PATH_append_path_command2(jbyte *bytes, jint array_length, jint cmd, jfloat x1,
                                                            jfloat y1, jfloat x2, jfloat y2) {
	(void)array_length;
	VG_PATH_HEADER_t *path = (VG_PATH_HEADER_t *)bytes;

	*VG_PATH_get_path_command_end(path) = VG_PATH_convert_path_command(cmd);

	VG_path_param_t *param_ptr = VG_PATH_get_path_param_end(path);
	*(param_ptr++) = x1;
	*(param_ptr++) = y1;
	*(param_ptr++) = x2;
	*(param_ptr++) = y2;

	path->cmd_length += 1u;
	path->param_length += 4u;

	return LLVG_SUCCESS;
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT uint32_t VG_PATH_append_path_command3(jbyte *bytes, jint array_length, jint cmd, jfloat x1,
                                                            jfloat y1, jfloat x2, jfloat y2, jfloat x3, jfloat y3) {
	(void)array_length;
	VG_PATH_HEADER_t *path = (VG_PATH_HEADER_t *)bytes;

	*VG_PATH_get_path_command_end(path) = VG_PATH_convert_path_command(cmd);

	VG_path_param_t *param_ptr = VG_PATH_get_path_param_end(path);
	*(param_ptr++) = x1;
	*(param_ptr++) = y1;
	*(param_ptr++) = x2;
	*(param_ptr++) = y2;
	*(param_ptr++) = x3;
	*(param_ptr++) = y3;

	path->cmd_length += 1u;
	path->param_length += 6u;

	return LLVG_SUCCESS;
}

// -----------------------------------------------------------------------------
// LLVG_PATH_impl.h functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
jint LLVG_PATH_IMPL_initializePath(jbyte *jpath, jint length) {
	VG_PATH_HEADER_t *path = (VG_PATH_HEADER_t *)jpath;
	uint32_t header_size = VG_PATH_get_path_header_size();
	jint ret = LLVG_SUCCESS;

	if (length >= header_size) {
		path->cmd_length = 0;
		path->param_length = 0;
		path->cmd_offset = 0;
		path->format = VG_PATH_get_path_encoder_format();
	} else {
		// the given byte array is too small
		ret = header_size;
	}

	return ret;
}

// See the header file for the function documentation
jint LLVG_PATH_IMPL_appendPathCommand1(jbyte *jpath, jint length, jint cmd, jfloat x, jfloat y) {
	VG_PATH_HEADER_t *path = (VG_PATH_HEADER_t *)jpath;
	jint ret = LLVG_SUCCESS;

	int32_t index = _extend_path(path, length, cmd, 2);
	if (index == 0) {
		(void)VG_PATH_append_path_command1((jbyte *)path, (uint32_t)length, cmd, x, y);
	} else {
		// too small buffer, ret is the required extra size * -1
		ret = -index;
	}

	return ret;
}

// See the header file for the function documentation
jint LLVG_PATH_IMPL_appendPathCommand2(jbyte *jpath, jint length, jint cmd, jfloat x1, jfloat y1, jfloat x2,
                                       jfloat y2) {
	VG_PATH_HEADER_t *path = (VG_PATH_HEADER_t *)jpath;
	jint ret = LLVG_SUCCESS;

	if (LLVG_PATH_CMD_CLOSE == cmd) {
		// parameters are path's bounds
		ret = _close_path(path, length, x1, y1, x2, y2);
	} else {
		int32_t index = _extend_path(path, length, cmd, 4);
		if (index == 0) {
			(void)VG_PATH_append_path_command2((jbyte *)path, (uint32_t)length, cmd, x1, y1, x2, y2);
		} else {
			// too small buffer, ret is the required extra size * -1
			ret = -index;
		}
	}

	return ret;
}

// See the header file for the function documentation
jint LLVG_PATH_IMPL_appendPathCommand3(jbyte *jpath, jint length, jint cmd, jfloat x1, jfloat y1, jfloat x2, jfloat y2,
                                       jfloat x3, jfloat y3) {
	VG_PATH_HEADER_t *path = (VG_PATH_HEADER_t *)jpath;
	jint ret = LLVG_SUCCESS;

	int32_t index = _extend_path(path, length, cmd, 6);
	if (index == 0) {
		(void)VG_PATH_append_path_command3((jbyte *)path, (uint32_t)length, cmd, x1, y1, x2, y2, x3, y3);
	} else {
		// too small buffer, ret is the required extra size * -1
		ret = -index;
	}

	return ret;
}

// See the header file for the function documentation
void LLVG_PATH_IMPL_reopenPath(jbyte *jpath) {
	VG_PATH_HEADER_t *path = (VG_PATH_HEADER_t *)jpath;
	path->cmd_length -= 1u;
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#endif // VG_FEATURE_PATH
