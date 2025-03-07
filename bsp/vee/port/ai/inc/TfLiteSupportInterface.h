/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef TFLITESUPPORTINTERFACE_H
#define TFLITESUPPORTINTERFACE_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#define MAX_MODEL_NUMBER 4
typedef int NXPVEE_handle_t;

NXPVEE_handle_t NXPEE_AI_TfLiteMicroSetUp(void* model_data, int arena_size);
int NXPVEE_AI_TfLiteMicroTearDown(int handle);

int NXPVEE_AI_TfLiteMicroGetInputTensorSize(int handle, int idx);
int NXPVEE_AI_TfLiteMicroGetInputTensorDims(int handle, int idx, int* sizes);
int NXPVEE_AI_TfLiteMicroGetInputTensorType(int handle, int idx);

int NXPVEE_AI_TfLiteMicroGetOutputTensorSize(int handle, int idx);
int NXPVEE_AI_TfLiteMicroGetOutputTensorDims(int handle, int idx, int* sizes);
int NXPVEE_AI_TfLiteMicroGetOutputTensorType(int handle, int idx);

int NXPVEE_AI_TfLiteMicroSetInputTensorInt8(int handle, int idx, const int8_t* src, int len);
int NXPVEE_AI_TfLiteMicroSetInputTensorUInt8(int handle, int idx, const uint8_t* src, int len);
int NXPVEE_AI_TfLiteMicroSetInputTensorFloat32(int handle, int idx, const float* src, int len);

int NXPVEE_AI_TfLiteMicroGetOutputTensorInt8(int handle, int idx, int8_t* dst, int len);
int NXPVEE_AI_TfLiteMicroGetOutputTensorUInt8(int handle, int idx, uint8_t* dst, int len);
int NXPVEE_AI_TfLiteMicroGetOutputTensorFloat32(int handle, int idx, float* dst, int len);

int NXPVEE_AI_TfLiteMicroRunInference(int handle);


#ifdef __cplusplus
}
#endif


#endif
