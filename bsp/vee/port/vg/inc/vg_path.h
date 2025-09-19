/*
 * C
 *
 * Copyright 2022-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief MicroEJ MicroVG library low level API: implementation of Path.
 * @author MicroEJ Developer Team
 * @version 7.0.1
 */

#if !defined VG_PATH_H
#define VG_PATH_H

#if defined __cplusplus
extern "C" {
#endif

#include "vg_configuration.h"

#ifdef VG_FEATURE_PATH

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <sni.h>
#include <stddef.h>

// -----------------------------------------------------------------------------
// Typedefs
// -----------------------------------------------------------------------------

#if defined(VG_FEATURE_PATH_DUAL_ARRAY) && (VG_FEATURE_PATH == VG_FEATURE_PATH_DUAL_ARRAY)

/*
 * @brief Defines the format of the path's commands.
 */
typedef uint8_t VG_path_command_t;

/*
 * @brief Defines the format of the commands' parameters.
 */
typedef float VG_path_param_t;

#endif // VG_FEATURE_PATH_DUAL_ARRAY

/*
 * @brief Map a jbyte array that represents a path
 */
typedef struct MICROVG_PATH_HEADER {
#if defined(VG_FEATURE_PATH_SINGLE_ARRAY) && (VG_FEATURE_PATH == VG_FEATURE_PATH_SINGLE_ARRAY)
	uint32_t data_size;  /* data size (without header) */
#else // VG_FEATURE_PATH_DUAL_ARRAY
	size_t param_length;
	size_t cmd_length;
	size_t cmd_offset;
#endif
	uint8_t format;
	uint8_t padding1;
	uint8_t padding2;
	uint8_t padding3;
	float bounds_xmin; /* left */
	float bounds_xmax; /* right */
	float bounds_ymin; /* top */
	float bounds_ymax; /* bottom */
} VG_PATH_HEADER_t;

// -----------------------------------------------------------------------------
// Specific path formatting functions [mandatory]
// -----------------------------------------------------------------------------

/*
 * @brief Gets the path's array format used to encode the commands and parameters. This format is stored into the path's
 * header.
 *
 * @return the path's format
 */
uint8_t VG_PATH_get_path_encoder_format(void);

/*
 * @brief Converts the command in destination format.
 *
 * @param[in] command: the command to convert
 *
 * @return the converted command
 */
uint32_t VG_PATH_convert_path_command(jint command);

// -----------------------------------------------------------------------------
// Specific path formatting functions [optional]
// -----------------------------------------------------------------------------

/*
 * @brief Initializes the path builder.
 *
 * The default implementation does nothing.
 */
void VG_PATH_initialize(void);

/*
 * @brief Gets the path's array header size.
 *
 * The default implementation returns sizeof(VG_PATH_HEADER_t).
 *
 * @return the size in bytes.
 */
uint32_t VG_PATH_get_path_header_size(void);

/*
 * @brief Gets the size to add in the path array to encode the command and its parameters.
 *
 * The default implementation uses 32-bit fields for the command and for each data.
 *
 * @param[in] command: the command to add.
 * @param[in] nbParams: the available number of command's parameters
 *
 * @return the size to add in the path array.
 */
uint32_t VG_PATH_get_path_command_size(jint command, uint32_t nbParams);

/*
 * @brief Appends the command with zero parameter in the path's array.
 *
 * The caller ensures the path's array is large enough to encode the command and its parameters.
 *
 * The default implementation uses 32-bit fields for the command and for each data.
 *
 * @param[in] path: the path's array
 * @param[in] array_length: the length of the array
 * @param[in] cmd: the command to add
 *
 * @return LLVG_SUCCESS on a success, or the missing space in the array in bytes
 *
 * @see #VG_PATH_get_path_command_size(jint, uint32_t)
 */
uint32_t VG_PATH_append_path_command0(jbyte *path, jint array_length, jint cmd);

/*
 * @brief Appends the command with 1 point parameter in the path's array.
 *
 * @param[in] path: the path's array
 * @param[in] array_length: the length of the array
 * @param[in] cmd: the command to add
 * @param[in] x: the command data 1.
 * @param[in] y: the command data 2.
 *
 * @return LLVG_SUCCESS on a success, or the missing space in the array in bytes
 *
 * @see #VG_PATH_append_path_command0(jbyte*, uint32_t, jint)
 */
uint32_t VG_PATH_append_path_command1(jbyte *path, jint array_length, jint cmd, jfloat x, jfloat y);

/*
 * @brief Appends the command with 2 points parameter in the path's array.
 *
 * @param[in] path: the path's array
 * @param[in] array_length: the length of the array
 * @param[in] cmd: the command to add
 * @param[in] x1: the command data 1.
 * @param[in] y1: the command data 2.
 * @param[in] x2: the command data 3.
 * @param[in] y2: the command data 4.
 *
 * @return LLVG_SUCCESS on a success, or the missing space in the array in bytes
 *
 * @see #VG_PATH_append_path_command0(jbyte*, uint32_t, jint)
 */
uint32_t VG_PATH_append_path_command2(jbyte *path, jint array_length, jint cmd, jfloat x1, jfloat y1, jfloat x2,
                                      jfloat y2);

/*
 * @brief Appends the command with 3 points parameter in the path's array.
 *
 * @param[in] path: the path's array
 * @param[in] array_length: the length of the array
 * @param[in] cmd: the command to add
 * @param[in] x1: the command data 1.
 * @param[in] y1: the command data 2.
 * @param[in] x2: the command data 3.
 * @param[in] y2: the command data 4.
 * @param[in] x3: the command data 5.
 * @param[in] y3: the command data 6.
 *
 * @return LLVG_SUCCESS on a success, or the missing space in the array in bytes
 *
 * @see #VG_PATH_append_path_command0(jbyte*, uint32_t, jint)
 */
uint32_t VG_PATH_append_path_command3(jbyte *path, jint array_length, jint cmd, jfloat x1, jfloat y1, jfloat x2,
                                      jfloat y2, jfloat x3, jfloat y3);

/*
 * @brief Gets the number of parameters for a specific command.
 *
 * @param[in] command: the command.
 *
 * @return the number of parameters for the given command.
 */
uint32_t VG_PATH_get_command_parameter_number(jint command);

#if defined(VG_FEATURE_PATH_DUAL_ARRAY) && (VG_FEATURE_PATH == VG_FEATURE_PATH_DUAL_ARRAY)

/*
 * @brief Returns a pointer to the beginning of a path's parameters
 *
 * @param[in] path the path
 * @return a pointer to the first parameter
 */
static inline VG_path_param_t * VG_PATH_get_path_param_begin(VG_PATH_HEADER_t *path) {
	// cppcheck-suppress [misra-c2012-11.3,invalidPointerCast] cast is valid
	return (VG_path_param_t *)(((uint8_t *)path) + VG_PATH_get_path_header_size());
}

/*
 * @brief Returns a pointer to the end of a path's parameters
 *
 * @param[in] path the path
 * @return a pointer to the memory area immediately after the last parameter
 */
static inline VG_path_param_t * VG_PATH_get_path_param_end(VG_PATH_HEADER_t *path) {
	return (VG_PATH_get_path_param_begin(path)) + path->param_length;
}

/*
 * @brief Returns a pointer to the beginning of a path's commands
 *
 * @param[in] path the path
 * @return a pointer to the first command
 */
static inline VG_path_command_t * VG_PATH_get_path_command_begin(VG_PATH_HEADER_t *path) {
	return ((uint8_t *)VG_PATH_get_path_param_begin(path)) + path->cmd_offset;
}

/*
 * @brief Returns a pointer to the end of a path's commands
 *
 * @param[in] path the path
 * @return a pointer to the memory area immediately after the last command
 */
static inline VG_path_command_t * VG_PATH_get_path_command_end(VG_PATH_HEADER_t *path) {
	return (VG_PATH_get_path_command_begin(path)) + path->cmd_length;
}

#endif // VG_FEATURE_PATH_DUAL_ARRAY

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#endif // VG_FEATURE_PATH

#ifdef __cplusplus
}
#endif

#endif // !defined VG_PATH_H
