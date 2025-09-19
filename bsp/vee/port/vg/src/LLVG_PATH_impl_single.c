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

#if defined(VG_FEATURE_PATH) && defined(VG_FEATURE_PATH_SINGLE_ARRAY) && \
	(VG_FEATURE_PATH == VG_FEATURE_PATH_SINGLE_ARRAY)

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
static int32_t _extend_path(VG_PATH_HEADER_t *path, jint length, jint cmd, uint32_t nb_fields) {
	uint32_t index = VG_PATH_get_path_header_size() + path->data_size;
	uint32_t extra_size = VG_PATH_get_path_command_size(cmd, nb_fields);
	int32_t ret;

	if (length >= (index + extra_size)) {
		path->data_size += extra_size;

		// return next free space (return a positive value)
		ret = index;
	} else {
		// too small buffer, ret is the required extra size
		// (return a negative value)
		ret = -extra_size;
	}

	return ret;
}

static int32_t _close_path(VG_PATH_HEADER_t *path, jint length, jfloat x1, jfloat y1, jfloat x2, jfloat y2) {
	int32_t index = _extend_path(path, length, LLVG_PATH_CMD_CLOSE, 0);
	int32_t ret = LLVG_SUCCESS;
	if (index > 0) {
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
	return (nbParams + (uint32_t)1 /* command */) * sizeof(uint32_t);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT uint32_t VG_PATH_append_path_command0(jbyte *path, jint offset, jint cmd) {
	uint32_t *data = (uint32_t *)(path + offset);
	*data = VG_PATH_convert_path_command(cmd);
	return sizeof(uint32_t);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT uint32_t VG_PATH_append_path_command1(jbyte *path, jint offset, jint cmd, jfloat x, jfloat y) {
	uint32_t *data = (uint32_t *)(path + offset);
	*data = VG_PATH_convert_path_command(cmd);
	++data;
	*data = JFLOAT_TO_UINT32_t(x);
	++data;
	*data = JFLOAT_TO_UINT32_t(y);
	return (uint32_t)3 * sizeof(uint32_t);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT uint32_t VG_PATH_append_path_command2(jbyte *path, jint offset, jint cmd, jfloat x1, jfloat y1,
                                                            jfloat x2, jfloat y2) {
	uint32_t *data = (uint32_t *)(path + offset);
	*data = VG_PATH_convert_path_command(cmd);
	++data;
	*data = JFLOAT_TO_UINT32_t(x1);
	++data;
	*data = JFLOAT_TO_UINT32_t(y1);
	++data;
	*data = JFLOAT_TO_UINT32_t(x2);
	++data;
	*data = JFLOAT_TO_UINT32_t(y2);
	return (uint32_t)5 * sizeof(uint32_t);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT uint32_t VG_PATH_append_path_command3(jbyte *path, jint offset, jint cmd, jfloat x1, jfloat y1,
                                                            jfloat x2, jfloat y2, jfloat x3, jfloat y3) {
	uint32_t *data = (uint32_t *)(path + offset);
	*data = VG_PATH_convert_path_command(cmd);
	++data;
	*data = JFLOAT_TO_UINT32_t(x1);
	++data;
	*data = JFLOAT_TO_UINT32_t(y1);
	++data;
	*data = JFLOAT_TO_UINT32_t(x2);
	++data;
	*data = JFLOAT_TO_UINT32_t(y2);
	++data;
	*data = JFLOAT_TO_UINT32_t(x3);
	++data;
	*data = JFLOAT_TO_UINT32_t(y3);
	return (uint32_t)7 * sizeof(uint32_t);
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
		path->data_size = 0;
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
	if (index > 0) {
		(void)VG_PATH_append_path_command1((jbyte *)path, (uint32_t)index, cmd, x, y);
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
		if (index > 0) {
			(void)VG_PATH_append_path_command2((jbyte *)path, (uint32_t)index, cmd, x1, y1, x2, y2);
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
	if (index > 0) {
		(void)VG_PATH_append_path_command3((jbyte *)path, (uint32_t)index, cmd, x1, y1, x2, y2, x3, y3);
	} else {
		// too small buffer, ret is the required extra size * -1
		ret = -index;
	}

	return ret;
}

// See the header file for the function documentation
void LLVG_PATH_IMPL_reopenPath(jbyte *jpath) {
	VG_PATH_HEADER_t *path = (VG_PATH_HEADER_t *)jpath;
	path->data_size -= VG_PATH_get_path_command_size(LLVG_PATH_CMD_CLOSE, 0);
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#endif // VG_FEATURE_PATH
