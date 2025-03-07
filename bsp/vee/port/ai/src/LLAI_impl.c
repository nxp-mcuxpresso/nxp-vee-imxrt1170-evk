/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <LLAI_impl.h>

#include "TfLiteSupportInterface.h"
#include "LLMJVM.h"
#include <stdint.h>

typedef struct
{
    void* data;
    uint32_t size;
} java_resource;

uint32_t LLAI_IMPL_function(void)
{
    return 0;
}

int LLAI_IMPL_TfLiteMicroSetUp(char* model_path, int len, int arena_size)
{
    java_resource resource;
    int32_t ret = LLMJVM_getJavaResource(model_path, len, &resource);
    if (ret == 0)
    {
        return NXPEE_AI_TfLiteMicroSetUp(resource.data, arena_size);
    }
    else
    {
        return -1;
    }
}

int LLAI_IMPL_TfLiteMicroTearDown(int handle)
{
    return NXPVEE_AI_TfLiteMicroTearDown(handle);
}

/* Get input tensor data */
int LLAI_IMPL_GetInputTensorSize(int handle, int idx)
{
    return NXPVEE_AI_TfLiteMicroGetInputTensorSize(handle, idx);
}

int LLAI_IMPL_GetInputTensorDims(int handle, int idx, int* sizes)
{
    return NXPVEE_AI_TfLiteMicroGetInputTensorDims(handle, idx, sizes);
}

int LLAI_IMPL_GetInputTensorType(int handle, int idx)
{
    return NXPVEE_AI_TfLiteMicroGetInputTensorType(handle, idx);
}

/* Get output tensor data */
int LLAI_IMPL_GetOutputTensorSize(int handle, int idx)
{
    return NXPVEE_AI_TfLiteMicroGetOutputTensorSize(handle, idx);
}

int LLAI_IMPL_GetOutputTensorDims(int handle, int idx, int* sizes)
{
    return NXPVEE_AI_TfLiteMicroGetOutputTensorDims(handle, idx, sizes);
}

int LLAI_IMPL_GetOutputTensorType(int handle, int idx)
{
    return NXPVEE_AI_TfLiteMicroGetOutputTensorType(handle, idx);
}

/* Set input data */
int LLAI_IMPL_SetInputDataInt8(int handle, int idx, int8_t* src, int len)
{
    return NXPVEE_AI_TfLiteMicroSetInputTensorInt8(handle, idx, src, len);
}

int LLAI_IMPL_SetInputDataUInt8(int handle, int idx, uint8_t* src, int len)
{
    return NXPVEE_AI_TfLiteMicroSetInputTensorUInt8(handle, idx, src, len);
}

int LLAI_IMPL_SetInputDataFloat32(int handle, int idx, float* src, int len)
{
    return NXPVEE_AI_TfLiteMicroSetInputTensorFloat32(handle, idx, src, len);
}

/* Get output data */
int LLAI_IMPL_GetOutputDataInt8(int handle, int idx, int8_t* dst, int len)
{
    return NXPVEE_AI_TfLiteMicroGetOutputTensorInt8(handle, idx, dst, len);
}

int LLAI_IMPL_GetOutputDataUInt8(int handle, int idx, uint8_t* dst, int len)
{
    return NXPVEE_AI_TfLiteMicroGetOutputTensorUInt8(handle, idx, dst, len);
}

int LLAI_IMPL_GetOutputDataFloat32(int handle, int idx, float* dst, int len)
{
    return NXPVEE_AI_TfLiteMicroGetOutputTensorFloat32(handle, idx, dst, len);
}

/* Run inference */
int LLAI_IMPL_RunInference(int handle)
{
    return NXPVEE_AI_TfLiteMicroRunInference(handle);
}
