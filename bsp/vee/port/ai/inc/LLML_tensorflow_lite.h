/*
 * C++
 *
 * Copyright 2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef LLML_TENSORFLOW_LITE_H
#define LLML_TENSORFLOW_LITE_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * @brief MicroAI implementation over TensorFlow Lite.
 * @author MicroEJ Developer Team
 * @version 2.1.0
 */

typedef int64_t LLML_InterpreterHandle_t;

/**
 * This is a backend-agnostic C wrapper to the model interpreter C++ API.
 * The functions are almost a one to one mapping to LLML Abstraction Layer.
 * See LLML_impl.h for a more detailed documentation.
 */
LLML_InterpreterHandle_t LLML_CreateInterpreter(void *model_data, int model_size, int inference_memory_pool_size);
void LLML_TearDown(LLML_InterpreterHandle_t handle);
int LLML_Reset(LLML_InterpreterHandle_t handle);
int LLML_RunInference(LLML_InterpreterHandle_t handle);

int LLML_GetInputTensorCount(LLML_InterpreterHandle_t handle);
int LLML_GetOutputTensorCount(LLML_InterpreterHandle_t handle);

int LLML_GetInputTensorNumDimensions(LLML_InterpreterHandle_t handle, int index);
int LLML_GetInputTensorShape(LLML_InterpreterHandle_t handle, int index, int32_t *sizes);
int LLML_GetInputTensorType(LLML_InterpreterHandle_t handle, int index);
int LLML_GetInputTensorNumElements(LLML_InterpreterHandle_t handle, int index);
int LLML_GetInputTensorNumBytes(LLML_InterpreterHandle_t handle, int index);
int LLML_GetInputZeroPoint(LLML_InterpreterHandle_t handle, int index);
float LLML_GetInputScale(LLML_InterpreterHandle_t handle, int index);
bool LLML_InputQuantized(LLML_InterpreterHandle_t handle, int index);

int LLML_GetOutputTensorNumDimensions(LLML_InterpreterHandle_t handle, int index);
int LLML_GetOutputTensorShape(LLML_InterpreterHandle_t handle, int index, int32_t *sizes);
int LLML_GetOutputTensorType(LLML_InterpreterHandle_t handle, int index);
int LLML_GetOutputTensorNumElements(LLML_InterpreterHandle_t handle, int index);
int LLML_GetOutputTensorNumBytes(LLML_InterpreterHandle_t handle, int index);
int LLML_GetOutputZeroPoint(LLML_InterpreterHandle_t handle, int index);
float LLML_GetOutputScale(LLML_InterpreterHandle_t handle, int index);
bool LLML_OutputQuantized(LLML_InterpreterHandle_t handle, int index);

void LLML_SetInputTensorByte(LLML_InterpreterHandle_t handle, int index, const void *src);
void LLML_SetInputTensorInt(LLML_InterpreterHandle_t handle, int index, const void *src);
void LLML_SetInputTensorFloat(LLML_InterpreterHandle_t handle, int index, const void *src);

void LLML_GetOutputTensorByte(LLML_InterpreterHandle_t handle, int index, void *dst);
void LLML_GetOutputTensorInt(LLML_InterpreterHandle_t handle, int index, void *dst);
void LLML_GetOutputTensorFloat(LLML_InterpreterHandle_t handle, int index, void *dst);

#ifdef __cplusplus
}
#endif

#endif // LLML_TENSORFLOW_LITE_H
