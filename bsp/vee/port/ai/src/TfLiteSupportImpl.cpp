/*
 * Copyright 2023-2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "TfLiteSupportImpl.h"

#ifdef MCUXPRESSO_SDK
#define printf PRINTF
#endif

TfLiteSupportImpl::TfLiteSupportImpl(const void *modelData, size_t arena_size)
{
    model = tflite::GetModel(modelData);
    if(model == nullptr)
    {
        printf("ERROR: failed to get model\n");
    }
    assert (model != nullptr);

    // Allocate arena tensor memory
    tensor_arena = new uint8_t[arena_size];
    if(tensor_arena == nullptr)
    {
        printf("ERROR: failed to allocate tensor arena\n");
    }
    assert (tensor_arena != nullptr);

    extern tflite::MicroOpResolver* AI_GetOpsResolver();
    micro_op_resolver = AI_GetOpsResolver();
    if(micro_op_resolver == nullptr)
    {
        printf("ERROR: failed to get micro operations resolver\n");
    }
    assert (micro_op_resolver != nullptr);

    // Build an interpreter to run the model with
    interpreter = new tflite::MicroInterpreter(model, *micro_op_resolver,
            tensor_arena, arena_size);
    if(interpreter == nullptr)
    {
        printf("ERROR: failed to initialize interpreter\n");
    }
    assert (interpreter != nullptr);

    // Allocate memory from the tensor_arena for the model's tensor
    if(interpreter->AllocateTensors() != kTfLiteOk)
    {
        printf("Allocate tensor failed\n");
        assert (1 == 0);
    }
}

TfLiteSupportImpl::~TfLiteSupportImpl()
{
    delete [] tensor_arena;
    delete(interpreter);
}

int TfLiteSupportImpl::RunInference()
{
    int ret = 0;

    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk)
    {
        printf("Interpreter invocation failed\n");
        ret = kTfLiteError;
    }
    return ret;
}
