/*
 * C
 *
 * Copyright 2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief MicroAI implementation over TensorFlow Lite.
 * @author MicroEJ Developer Team
 * @version 2.1.0
 */

#include <LLML_impl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LLMJVM.h"
#include "LLML_configuration.h"
#include "LLML_microai_heap.h"

#if ((LLML_BACKEND == LLML_USE_TFLITE) || (LLML_BACKEND == LLML_USE_TFLITE_MICRO))
#include "LLML_tensorflow_lite.h"
#else
/* insert your ML backend here */
#endif

typedef struct {
	void *data;
	uint32_t size;
} java_resource;

jlong LLML_IMPL_init_model_from_resource(jbyte *modelPath, jint inferenceMemoryPoolSize) {
	java_resource resource;
	LLML_DEBUG_TRACE("modelPath = %s, inferenceMemoryPoolSize %d\n\n", (const char *)modelPath,
	                 inferenceMemoryPoolSize);
	int32_t ret = LLMJVM_getJavaResource((void *)modelPath, strlen((const char *)modelPath), &resource);
	if (0 == ret) {
		ret = LLML_CreateInterpreter((void *)resource.data, resource.size, inferenceMemoryPoolSize);
	} else {
		LLML_DEBUG_TRACE("cannot get model data from resource\n");
	}
	return ret;
}

jlong LLML_IMPL_allocate_model(jint modelByteSize) {
	/* needs to be initialized at least once */
	LLML_microai_heap_init();
	/* must be freed by LLML_IMPL_free_model */
	return (int64_t)(uintptr_t)LLML_microai_heap_allocate(modelByteSize);
}

void LLML_IMPL_free_model(jlong modelBufferHandle) {
	if (0 != modelBufferHandle) {
		LLML_microai_heap_free((void *)(uintptr_t)modelBufferHandle);
	}
}

void LLML_IMPL_load_model_chunk(jbyte *data, jlong modelBufferHandle, jint index, jint length) {
	LLML_DEBUG_TRACE("modelBufferHandle 0x%p index %d length %d\n", (void *)(uintptr_t)modelBufferHandle, index,
	                 length);
	memcpy((void *)((void *)(uintptr_t)modelBufferHandle + index), (void *)data, length);
}

jlong LLML_IMPL_init_model_from_buffer(jlong modelBufferHandle, jint modelByteSize, jint inferenceMemoryPoolSize) {
	LLML_DEBUG_TRACE("modelBufferHandle 0x%p modelByteSize %d inferenceMemoryPoolSize %d\n",
	                 (void *)(uintptr_t)modelBufferHandle,
	                 modelByteSize, inferenceMemoryPoolSize);
	return LLML_CreateInterpreter((void *)(uintptr_t)modelBufferHandle, modelByteSize, inferenceMemoryPoolSize);
}

jint LLML_IMPL_reset(jlong modelHandle) {
	return LLML_Reset((LLML_InterpreterHandle_t)modelHandle);
}

jint LLML_IMPL_run(jlong modelHandle) {
	return LLML_RunInference((LLML_InterpreterHandle_t)modelHandle);
}

void LLML_IMPL_clean(jlong modelHandle) {
	LLML_TearDown((LLML_InterpreterHandle_t)modelHandle);
}

jint LLML_IMPL_get_input_tensor_count(jlong modelHandle) {
	return LLML_GetInputTensorCount((LLML_InterpreterHandle_t)modelHandle);
}

jint LLML_IMPL_get_output_tensor_count(jlong modelHandle) {
	return LLML_GetOutputTensorCount((LLML_InterpreterHandle_t)modelHandle);
}

void LLML_IMPL_set_input_data_as_byte_array(jlong modelHandle, jint index, jbyte *inputData) {
	LLML_SetInputTensorByte((LLML_InterpreterHandle_t)modelHandle, index, inputData);
}

void LLML_IMPL_set_input_data_as_int_array(jlong modelHandle, jint index, jint *inputData) {
	LLML_SetInputTensorInt((LLML_InterpreterHandle_t)modelHandle, index, inputData);
}

void LLML_IMPL_set_input_data_as_float_array(jlong modelHandle, jint index, jfloat *inputData) {
	LLML_SetInputTensorFloat((LLML_InterpreterHandle_t)modelHandle, index, inputData);
}

jboolean LLML_IMPL_output_quantized(jlong modelHandle, jint index) {
	return LLML_OutputQuantized((LLML_InterpreterHandle_t)modelHandle, index);
}

jboolean LLML_IMPL_input_quantized(jlong modelHandle, jint index) {
	return LLML_InputQuantized((LLML_InterpreterHandle_t)modelHandle, index);
}

jint LLML_IMPL_get_output_data_type(jlong modelHandle, jint index) {
	return LLML_GetOutputTensorType((LLML_InterpreterHandle_t)modelHandle, index);
}

jint LLML_IMPL_get_input_data_type(jlong modelHandle, jint index) {
	return LLML_GetInputTensorType((LLML_InterpreterHandle_t)modelHandle, index);
}

jint LLML_IMPL_get_output_zero_point(jlong modelHandle, jint index) {
	return LLML_GetOutputZeroPoint((LLML_InterpreterHandle_t)modelHandle, index);
}

jint LLML_IMPL_get_input_zero_point(jlong modelHandle, jint index) {
	return LLML_GetInputZeroPoint((LLML_InterpreterHandle_t)modelHandle, index);
}

jfloat LLML_IMPL_get_output_scale(jlong modelHandle, jint index) {
	return LLML_GetOutputScale((LLML_InterpreterHandle_t)modelHandle, index);
}

jfloat LLML_IMPL_get_input_scale(jlong modelHandle, jint index) {
	return LLML_GetInputScale((LLML_InterpreterHandle_t)modelHandle, index);
}

jint LLML_IMPL_get_output_num_bytes(jlong modelHandle, jint index) {
	return LLML_GetOutputTensorNumBytes((LLML_InterpreterHandle_t)modelHandle, index);
}

jint LLML_IMPL_get_input_num_bytes(jlong modelHandle, jint index) {
	return LLML_GetInputTensorNumBytes((LLML_InterpreterHandle_t)modelHandle, index);
}

jint LLML_IMPL_get_input_num_elements(jlong modelHandle, jint index) {
	return LLML_GetInputTensorNumElements((LLML_InterpreterHandle_t)modelHandle, index);
}

jint LLML_IMPL_get_output_num_elements(jlong modelHandle, jint index) {
	return LLML_GetOutputTensorNumElements((LLML_InterpreterHandle_t)modelHandle, index);
}

jint LLML_IMPL_get_input_num_dimensions(jlong modelHandle, jint index) {
	return LLML_GetInputTensorNumDimensions((LLML_InterpreterHandle_t)modelHandle, index);
}

jint LLML_IMPL_get_output_num_dimensions(jlong modelHandle, jint index) {
	return LLML_GetOutputTensorNumDimensions((LLML_InterpreterHandle_t)modelHandle, index);
}

void LLML_IMPL_get_input_shape(jlong modelHandle, jint index, jint *sizes) {
	LLML_GetInputTensorShape((LLML_InterpreterHandle_t)modelHandle, index, sizes);
}

void LLML_IMPL_get_output_shape(jlong modelHandle, jint index, jint *sizes) {
	LLML_GetOutputTensorShape((LLML_InterpreterHandle_t)modelHandle, index, sizes);
}

void LLML_IMPL_get_output_data_as_byte_array(jlong modelHandle, jint index, jbyte *outputData) {
	LLML_GetOutputTensorByte((LLML_InterpreterHandle_t)modelHandle, index, outputData);
}

void LLML_IMPL_get_output_data_as_integer_array(jlong modelHandle, jint index, jint *outputData) {
	LLML_GetOutputTensorInt((LLML_InterpreterHandle_t)modelHandle, index, outputData);
}

void LLML_IMPL_get_output_data_as_float_array(jlong modelHandle, jint index, jfloat *outputData) {
	LLML_GetOutputTensorFloat((LLML_InterpreterHandle_t)modelHandle, index, outputData);
}
