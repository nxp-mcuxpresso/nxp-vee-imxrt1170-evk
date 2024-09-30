/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"

tflite::MicroOpResolver &AI_GetOpsResolver()
{
    static tflite::MicroMutableOpResolver<9> s_microOpResolver;

    s_microOpResolver.AddReshape();
    s_microOpResolver.AddSlice();
    s_microOpResolver.AddDequantize();
	s_microOpResolver.AddConv2D();
	s_microOpResolver.AddDepthwiseConv2D();
	s_microOpResolver.AddFullyConnected();
	s_microOpResolver.AddMaxPool2D();
	s_microOpResolver.AddSoftmax();
	s_microOpResolver.AddAveragePool2D();

    return s_microOpResolver;
}
