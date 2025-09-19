/*
 * C++
 *
 * Copyright 2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef LLML_TENSORFLOW_LITE_OPS_H
#define LLML_TENSORFLOW_LITE_OPS_H

/**
 * @file
 * @brief MicroAI implementation over TensorFlow Lite.
 * @author MicroEJ Developer Team
 * @version 2.1.0
 */

#if (LLML_BACKEND == LLML_USE_TFLITE_MICRO)
/**
 * This code is inspired from tensorflow lite micro gen_micro_mutable_op_resolver tool:
 * https://github.com/tensorflow/tflite-micro/blob/main/tensorflow/lite/micro/tools/gen_micro_mutable_op_resolver/templates/micro_mutable_op_resolver.h.mako
 *
 * Each tflite model comes from a list of operators, which then need to be instantiated at compile time.
 * Tflite micro https://ai.google.dev/edge/litert/microcontrollers/get_started#6_instantiate_operations_resolver
 */

#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"

constexpr int kNumberOperators = 12;

inline tflite::MicroMutableOpResolver<kNumberOperators> & LLML_get_resolver() {
	// Initialize static micro_op_resolver variable using a lambda so that operator list is not registered more than once.
    static tflite::MicroMutableOpResolver<kNumberOperators> micro_op_resolver = [](){
        tflite::MicroMutableOpResolver<kNumberOperators> operators;

		/**
		 * By default, a subset of tensorflow lite micro Builtin operators are registered.
		 * It allows to run the MicroAI testsuite models and some demo models such as MNIST/CIFARNET.
		 *
		 * Update the registered list to match your model's list of operators.
		 * Note: you must increase kNumberOperators value.
		 */
		/* for CIFARNET model apps/aiSample/src/main/resources/model/cifarnet_quant_int8.tflite */
		operators.AddReshape();
		operators.AddConv2D();
		operators.AddRelu();
		operators.AddFullyConnected();
		operators.AddDequantize();
		operators.AddQuantize();
		operators.AddSoftmax();
		operators.AddAveragePool2D();

		/* for test suites models vee-port/validation/ai/src/test/resources/model */
		operators.AddCast();
		operators.AddMean();
		operators.AddMul();

		/* for MNIST LSTM model: https://github.com/MicroEJ/Example-Foundation-Libraries */
		operators.AddUnidirectionalSequenceLSTM();

		/**
		 * Register custom operator for a specific NPU.
		 * Note: you must increase kNumberOperators value.
		 *
		 * micro_op_resolver.AddCustom("com.vendor.npu_operator",
		 *                             tflite::Register_NPU_OPERATOR());
		 *
		 * example with NXP Neutron NPU Custom Operator:
		 *
		 * #include "tensorflow/lite/micro/kernels/neutron/neutron.h"
		 * micro_op_resolver.AddCustom(tflite::GetString_NEUTRON_GRAPH(),
		 *                             tflite::Register_NEUTRON_GRAPH());
		 */

        return operators;
    }();

    return micro_op_resolver;
}

#endif

#endif // LLML_TENSORFLOW_LITE_OPS_H
