/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "TfLiteSupportInterface.h"
#include "TfLiteSupportImpl.h"
#include <vector>

static TfLiteSupportImpl *models_g[MAX_MODEL_NUMBER] = {nullptr, nullptr, nullptr, nullptr};


extern "C"
NXPVEE_handle_t NXPEE_AI_TfLiteMicroSetUp(void* model_data, int arena_size)
{
    int handle = 0;
    for (handle = 0; handle < MAX_MODEL_NUMBER; handle++)
    {
        if (nullptr == models_g[handle])
        {
          TfLiteSupportImpl *impl = new TfLiteSupportImpl(model_data, arena_size);
          assert (impl != nullptr);

          models_g[handle] = impl;
          break;
        }
    }
    assert (handle != MAX_MODEL_NUMBER);

    return handle;
}

int NXPVEE_AI_TfLiteMicroTearDown(int handle)
{
    TfLiteSupportImpl *impl = models_g[handle];
    assert (impl!=nullptr);

    delete impl;
    models_g[handle] = nullptr;

    return 0;
}

// Get input tensor data
extern "C"
int NXPVEE_AI_TfLiteMicroGetInputTensorSize(int handle, int idx)
{
    TfLiteSupportImpl *impl = models_g[handle];
    assert (impl != nullptr);

    TfLiteTensor* t = impl->GetInputTensor(idx);
    assert (t != nullptr);

    return t->dims->size;
}

extern "C"
int NXPVEE_AI_TfLiteMicroGetInputTensorDims(int handle, int idx, int* sizes)
{
    TfLiteSupportImpl *impl = models_g[handle];
    assert (impl != nullptr);

    TfLiteTensor* t = impl->GetInputTensor(idx);
    assert (t != nullptr);

    TfLiteIntArray* idims = t->dims;
    for (int i = 0; i < idims->size; i++)
    {
        sizes[i] = idims->data[i];
    }

    return 0;
}

extern "C"
int NXPVEE_AI_TfLiteMicroGetInputTensorType(int handle, int idx)
{
    TfLiteSupportImpl *impl = models_g[handle];
    assert (impl != nullptr);

    TfLiteTensor* t = impl->GetInputTensor(idx);
    assert (t != nullptr);

    return t->type;
}

// Get output tensor data
extern "C"
int NXPVEE_AI_TfLiteMicroGetOutputTensorSize(int handle, int idx)
{
    TfLiteSupportImpl *impl = models_g[handle];
    assert (impl != nullptr);

    TfLiteTensor* t = impl->GetOutputTensor(idx);
    assert (t != nullptr);

    return t->dims->size;
}

extern "C"
int NXPVEE_AI_TfLiteMicroGetOutputTensorDims(int handle, int idx, int* sizes)
{
    TfLiteSupportImpl *impl = models_g[handle];
    assert (impl != nullptr);

    TfLiteTensor* t = impl->GetOutputTensor(idx);
    assert (t != nullptr);

    TfLiteIntArray* idims = t->dims;
    for (int i = 0; i < idims->size; i++)
    {
        sizes[i] = idims->data[i];
    }

    return 0;
}

extern "C"
int NXPVEE_AI_TfLiteMicroGetOutputTensorType(int handle, int idx)
{
    TfLiteSupportImpl *impl = models_g[handle];
    assert (impl != nullptr);

    TfLiteTensor* t = impl->GetOutputTensor(idx);
    assert (t != nullptr);

    return t->type;
}


// Set input data
extern "C"
int NXPVEE_AI_TfLiteMicroSetInputTensorInt8(int handle, int idx, const int8_t* src, int len)
{
    TfLiteSupportImpl *impl = models_g[handle];
    assert (impl != nullptr);

    int8_t* dst = impl->GetInputData<int8_t>(idx);
    assert (dst != nullptr);
    assert (src != nullptr);

    memcpy(dst, src, len*sizeof(int8_t));
    return 0;
}

extern "C"
int NXPVEE_AI_TfLiteMicroSetInputTensorUInt8(int handle, int idx, const uint8_t* src, int len)
{
    TfLiteSupportImpl *impl = models_g[handle];
    assert (impl != nullptr);

    uint8_t* dst = impl->GetInputData<uint8_t>(idx);
    assert (dst != nullptr);
    assert (src != nullptr);

    memcpy(dst, src, len*sizeof(uint8_t));
    return 0;
}

extern "C"
int NXPVEE_AI_TfLiteMicroSetInputTensorFloat32(int handle, int idx, const float* src, int len)
{
    TfLiteSupportImpl *impl = models_g[handle];
    assert (impl != nullptr);

    float* dst = impl->GetInputData<float>(idx);
    assert (dst != nullptr);
    assert (src != nullptr);

    memcpy(dst, src, len*sizeof(float));
    return 0;
}

// Get output data
extern "C"
int NXPVEE_AI_TfLiteMicroGetOutputTensorInt8(int handle, int idx, int8_t* dst, int len)
{
    TfLiteSupportImpl *impl = models_g[handle];
    assert (impl != nullptr);

    int8_t* src = impl->GetOutputData<int8_t>(idx);
    assert (dst != nullptr);
    assert (src != nullptr);

    memcpy(dst, src, len*sizeof(int8_t));
    return 0;
}

extern "C"
int NXPVEE_AI_TfLiteMicroGetOutputTensorUInt8(int handle, int idx, uint8_t* dst, int len)
{
    TfLiteSupportImpl *impl = models_g[handle];
    assert (impl != nullptr);

    uint8_t* src = impl->GetOutputData<uint8_t>(idx);
    assert (dst != nullptr);
    assert (src != nullptr);

    memcpy(dst, src, len*sizeof(uint8_t));
    return 0;
}

extern "C"
int NXPVEE_AI_TfLiteMicroGetOutputTensorFloat32(int handle, int idx, float* dst, int len)
{
    TfLiteSupportImpl *impl = models_g[handle];
    assert (impl != nullptr);

    float* src = impl->GetOutputData<float>(idx);
    assert (dst != nullptr);
    assert (src != nullptr);

    memcpy(dst, src, len*sizeof(float));
    return 0;
}

// Run inference
extern "C"
int NXPVEE_AI_TfLiteMicroRunInference(int handle)
{
    TfLiteSupportImpl *impl = models_g[handle];
    assert (impl != nullptr);

    return impl->RunInference();
}
