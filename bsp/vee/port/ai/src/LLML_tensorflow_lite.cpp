/*
 * C++
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

#include "LLML_impl.h"
#include "LLML_configuration.h"
#include "LLML_microai_heap.h"
#include "LLML_tensorflow_lite.h"

#if (LLML_BACKEND == LLML_USE_TFLITE_MICRO)
#include <tensorflow/lite/micro/micro_log.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/micro/system_setup.h>
#include <tensorflow/lite/schema/schema_generated.h>
#include "LLML_tensorflow_lite_ops.h"
#elif (LLML_BACKEND == LLML_USE_TFLITE)
#include <tensorflow/lite/c/c_api_types.h>
#include <tensorflow/lite/c/common.h>
#include <tensorflow/lite/interpreter_builder.h>
#include <tensorflow/lite/interpreter.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/model_builder.h>
#include <tensorflow/lite/schema/schema_generated.h>
#endif

/**
 * Hide the type of the interpreter and improve readability.
 * This allows to use tflite/tflite-micro interpreters since they provide almost the same API
 */
#if (LLML_BACKEND == LLML_USE_TFLITE_MICRO)
using TFliteInterpreter = tflite::MicroInterpreter;
#elif (LLML_BACKEND == LLML_USE_TFLITE)
using TFliteInterpreter = tflite::Interpreter;
#endif

/**
 * This structure is passed to the LLAPI, and exists to store any context that need to be freed later on.
 * Note: smart pointers don't need to be freed explicitely, deleting this struct will also free them.
 */
typedef struct {
	std::unique_ptr<TFliteInterpreter> interpreter;
	uint8_t *tensorArena; /* TFLM Arena Size */
} LLML_TFLiteInterpreterHandle_t;

/* helper function */
static TFliteInterpreter * getInterpreter(LLML_InterpreterHandle_t handle) {
	if (handle != 0) {
		TFliteInterpreter *interpreter = reinterpret_cast<LLML_TFLiteInterpreterHandle_t *>(handle)->interpreter.get();
		return interpreter;
	}
	return nullptr;
}

extern "C"
LLML_InterpreterHandle_t LLML_CreateInterpreter(void *model_data, int model_size, int inference_memory_pool_size) {
	/* Allocate memory for the interpreter handler structure */
	LLML_TFLiteInterpreterHandle_t *handle = new LLML_TFLiteInterpreterHandle_t;
	if (handle == nullptr) {
		LLML_DEBUG_TRACE("ERROR: Failed to create interpreter handle\n");
		return 0;
	}

#if (LLML_BACKEND == LLML_USE_TFLITE_MICRO)
	/* Map the model into a usable data structure. */
	const tflite::Model *model = tflite::GetModel(model_data);
	if (model == nullptr) {
		LLML_DEBUG_TRACE("ERROR: Failed to build model\n");
		return 0;
	}

	/* Allocate tensor arena, specific to tflite micro */
	LLML_microai_heap_init();
	handle->tensorArena = (uint8_t *)LLML_microai_heap_allocate(inference_memory_pool_size);
	if (handle->tensorArena == nullptr) {
		LLML_DEBUG_TRACE("ERROR: Failed to allocate tensor arena\n");
		return 0;
	}

	/* Get the Ops resolver defined statically for each model */
	tflite::MicroMutableOpResolver<kNumberOperators> &micro_op_resolver = LLML_get_resolver();

	/* Build the interpreter */
	tflite::MicroInterpreter *micro_interpreter = new tflite::MicroInterpreter(model, micro_op_resolver,
	                                                                           handle->tensorArena,
	                                                                           inference_memory_pool_size);

	/* make unique for later use and cleanup */
	std::unique_ptr<tflite::MicroInterpreter> interpreter(micro_interpreter);

	if (interpreter == nullptr) {
		LLML_DEBUG_TRACE("ERROR: failed to initialize interpreter\n");
		return 0;
	}

	/* Allocate tensors */
	if (interpreter->AllocateTensors() != kTfLiteOk) {
		LLML_DEBUG_TRACE("ERROR: Failed to allocate tensors\n");
		return 0;
	}

	LLML_DEBUG_TRACE("%s(%d) Arena used bytes: %d\n", __func__, __LINE__, interpreter->arena_used_bytes());

	/* store the interpreter */
	handle->interpreter = std::move(interpreter);
#elif (LLML_BACKEND == LLML_USE_TFLITE)
	/* Map the model into a usable data structure. */
	std::unique_ptr<tflite::FlatBufferModel> model = tflite::FlatBufferModel::BuildFromBuffer((const char *)model_data,
	                                                                                          model_size);
	if (model == nullptr) {
		LLML_DEBUG_TRACE("ERROR: Failed to build model\n");
		return 0;
	}

	/* No arena for tflite, TODO: return an exception is inferenceMemoryPool was allocated */
	handle->tensorArena = nullptr;

	/* Get the default Ops resolver */
	tflite::ops::builtin::BuiltinOpResolver resolver;

	/* Build the interpreter */
	std::unique_ptr<tflite::Interpreter> interpreter;
	tflite::InterpreterBuilder(*model, resolver)(&interpreter);
	if (interpreter == nullptr) {
		LLML_DEBUG_TRACE("ERROR: failed to initialize interpreter\n");
		return 0;
	}

	/* Allocate tensors */
	if (interpreter->AllocateTensors() != kTfLiteOk) {
		LLML_DEBUG_TRACE("ERROR: Failed to allocate tensors\n");
		return 0;
	}

	/* store the interpreter */
	handle->interpreter = std::move(interpreter);
#endif

	return (LLML_InterpreterHandle_t)handle;
}

extern "C"
void LLML_TearDown(LLML_InterpreterHandle_t handle) {
	LLML_TFLiteInterpreterHandle_t *handle_ptr = reinterpret_cast<LLML_TFLiteInterpreterHandle_t *>(handle);
	if (handle_ptr != nullptr) {
		if (handle_ptr->tensorArena != nullptr) {
			LLML_microai_heap_free(handle_ptr->tensorArena);
		}
		/* smart pointers within the struct will be destroyed */
		delete handle_ptr;
	}
}

extern "C"
int LLML_Reset(LLML_InterpreterHandle_t handle) {
	int ret = 0;
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
#if (LLML_BACKEND == LLML_USE_TFLITE_MICRO)
		TfLiteStatus reset_status = interpreter->Reset();
#elif (LLML_BACKEND == LLML_USE_TFLITE)
		TfLiteStatus reset_status = interpreter->ResetVariableTensors();
#endif
		if (reset_status != kTfLiteOk) {
			LLML_DEBUG_TRACE("Interpreter reset failed\n");
			ret = kTfLiteError;
		}
	}
	return ret;
}

extern "C"
int LLML_RunInference(LLML_InterpreterHandle_t handle) {
	int ret = 0;
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
		TfLiteStatus invoke_status = interpreter->Invoke();
		if (invoke_status != kTfLiteOk) {
			LLML_DEBUG_TRACE("Interpreter invocation failed\n");
			ret = kTfLiteError;
		}
	}
	return ret;
}

extern "C"
int LLML_GetInputTensorCount(LLML_InterpreterHandle_t handle) {
	int ret = 0;
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
#if (LLML_BACKEND == LLML_USE_TFLITE_MICRO)
		ret = interpreter->inputs_size();
#elif (LLML_BACKEND == LLML_USE_TFLITE)
		ret = interpreter->inputs().size();
#endif
	}
	return ret;
}

extern "C"
int LLML_GetOutputTensorCount(LLML_InterpreterHandle_t handle) {
	int ret = 0;
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
#if (LLML_BACKEND == LLML_USE_TFLITE_MICRO)
		ret = interpreter->outputs_size();
#elif (LLML_BACKEND == LLML_USE_TFLITE)
		ret = interpreter->outputs().size();
#endif
	}
	return ret;
}

extern "C"
int LLML_GetInputTensorNumDimensions(LLML_InterpreterHandle_t handle, int index) {
	int ret = 0;
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
		TfLiteTensor *tensor = interpreter->input_tensor(index);
		if (tensor) {
			ret = tensor->dims->size;
		}
	}
	return ret;
}

extern "C"
int LLML_GetInputTensorShape(LLML_InterpreterHandle_t handle, int index, int32_t *sizes) {
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
		TfLiteTensor *tensor = interpreter->input_tensor(index);
		if (tensor) {
			TfLiteIntArray *idims = tensor->dims;
			for (int i = 0; i < idims->size; i++) {
				sizes[i] = idims->data[i];
			}
		}
	}
	return 0;
}

extern "C"
int LLML_GetInputTensorType(LLML_InterpreterHandle_t handle, int index) {
	LLML_Tensor_Type ML_tensor_type = LLML_TENSOR_TYPE_UNKNOWN;
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
		TfLiteTensor *tensor = interpreter->input_tensor(index);
		if (tensor) {
			switch (tensor->type) {
			case kTfLiteFloat32:
				ML_tensor_type = LLML_TENSOR_TYPE_FLOAT32;
				break;
			case kTfLiteInt32:
				ML_tensor_type = LLML_TENSOR_TYPE_INT32;
				break;
			case kTfLiteUInt8:
				ML_tensor_type = LLML_TENSOR_TYPE_UINT8;
				break;
			case kTfLiteInt8:
				ML_tensor_type = LLML_TENSOR_TYPE_INT8;
				break;
			case kTfLiteUInt32:
				ML_tensor_type = LLML_TENSOR_TYPE_UINT32;
				break;
			default:
				ML_tensor_type = LLML_TENSOR_TYPE_UNKNOWN;
				break;
			}
		}
	}
	return ML_tensor_type;
}

extern "C"
int LLML_GetInputTensorNumElements(LLML_InterpreterHandle_t handle, int index) {
	int elements = 1;
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
		TfLiteTensor *tensor = interpreter->input_tensor(index);
		if (tensor) {
			/* this counts the number of elements, not the number of bytes */
			TfLiteIntArray *idims = tensor->dims;
			for (int i = 0; i < idims->size; i++) {
				elements *= idims->data[i];
			}
		}
	}
	return elements;
}

extern "C"
int LLML_GetInputTensorNumBytes(LLML_InterpreterHandle_t handle, int index) {
	int ret = 0;
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
		TfLiteTensor *tensor = interpreter->input_tensor(index);
		if (tensor) {
			ret = tensor->bytes;
		}
	}
	return ret;
}

extern "C"
int LLML_GetInputZeroPoint(LLML_InterpreterHandle_t handle, int index) {
	int ret = 0;
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
		TfLiteTensor *tensor = interpreter->input_tensor(index);
		if (tensor) {
			ret = tensor->params.zero_point;
		}
	}
	return ret;
}

extern "C"
float LLML_GetInputScale(LLML_InterpreterHandle_t handle, int index) {
	float ret = 0;
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
		TfLiteTensor *tensor = interpreter->input_tensor(index);
		if (tensor) {
			ret = tensor->params.scale;
		}
	}
	return ret;
}

extern "C"
bool LLML_InputQuantized(LLML_InterpreterHandle_t handle, int index) {
	bool ret = false;
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
		TfLiteTensor *tensor = interpreter->input_tensor(index);
		if (tensor) {
			ret = tensor->quantization.type != kTfLiteNoQuantization;
		}
	}
	return ret;
}

extern "C"
int LLML_GetOutputTensorNumDimensions(LLML_InterpreterHandle_t handle, int index) {
	int ret = false;
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
		TfLiteTensor *tensor = interpreter->output_tensor(index);
		if (tensor) {
			ret = tensor->dims->size;
		}
	}
	return ret;
}

extern "C"
int LLML_GetOutputTensorShape(LLML_InterpreterHandle_t handle, int index, int32_t *sizes) {
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
		TfLiteTensor *tensor = interpreter->output_tensor(index);
		if (tensor) {
			TfLiteIntArray *idims = tensor->dims;
			for (int i = 0; i < idims->size; i++) {
				sizes[i] = idims->data[i];
			}
		}
	}
	return 0;
}

extern "C"
int LLML_GetOutputTensorType(LLML_InterpreterHandle_t handle, int index) {
	LLML_Tensor_Type ML_tensor_type = LLML_TENSOR_TYPE_UNKNOWN;
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
		TfLiteTensor *tensor = interpreter->output_tensor(index);
		if (tensor) {
			switch (tensor->type) {
			case kTfLiteFloat32:
				ML_tensor_type = LLML_TENSOR_TYPE_FLOAT32;
				break;
			case kTfLiteInt32:
				ML_tensor_type = LLML_TENSOR_TYPE_INT32;
				break;
			case kTfLiteUInt8:
				ML_tensor_type = LLML_TENSOR_TYPE_UINT8;
				break;
			case kTfLiteInt8:
				ML_tensor_type = LLML_TENSOR_TYPE_INT8;
				break;
			case kTfLiteUInt32:
				ML_tensor_type = LLML_TENSOR_TYPE_UINT32;
				break;
			default:
				ML_tensor_type = LLML_TENSOR_TYPE_UNKNOWN;
				break;
			}
		}
	}
	return ML_tensor_type;
}

extern "C"
int LLML_GetOutputTensorNumElements(LLML_InterpreterHandle_t handle, int index) {
	int elements = 1;
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
		TfLiteTensor *tensor = interpreter->output_tensor(index);
		if (tensor) {
			/* this counts the number of elements, not the number of bytes */
			TfLiteIntArray *idims = tensor->dims;
			for (int i = 0; i < idims->size; i++) {
				elements *= idims->data[i];
			}
		}
	}
	return elements;
}

extern "C"
int LLML_GetOutputTensorNumBytes(LLML_InterpreterHandle_t handle, int index) {
	int ret = 0;
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
		TfLiteTensor *tensor = interpreter->output_tensor(index);
		if (tensor) {
			ret = tensor->bytes;
		}
	}
	return ret;
}

extern "C"
int LLML_GetOutputZeroPoint(LLML_InterpreterHandle_t handle, int index) {
	int ret = 0;
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
		TfLiteTensor *tensor = interpreter->output_tensor(index);
		if (tensor) {
			ret = tensor->params.zero_point;
		}
	}
	return ret;
}

extern "C"
float LLML_GetOutputScale(LLML_InterpreterHandle_t handle, int index) {
	float ret = 0;
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
		TfLiteTensor *tensor = interpreter->output_tensor(index);
		if (tensor) {
			ret = tensor->params.scale;
		}
	}
	return ret;
}

extern "C"
bool LLML_OutputQuantized(LLML_InterpreterHandle_t handle, int index) {
	bool ret = false;
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
		TfLiteTensor *tensor = interpreter->output_tensor(index);
		if (tensor) {
			ret = tensor->quantization.type != kTfLiteNoQuantization;
		}
	}
	return ret;
}

extern "C"
void LLML_SetInputTensorByte(LLML_InterpreterHandle_t handle, int index, const void *src) {
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
		TfLiteTensor *tensor = interpreter->input_tensor(index);
		if (tensor) {
			void *dst = nullptr;
			if (tensor->type == kTfLiteInt8) {
				dst = (void *)interpreter->typed_input_tensor<int8_t>(index);
			} else if (tensor->type == kTfLiteUInt8) {
				dst = (void *)interpreter->typed_input_tensor<uint8_t>(index);
			}
			if (dst != nullptr && src != nullptr) {
				memcpy(dst, src, tensor->bytes);
			}
		}
	}
}

extern "C"
void LLML_SetInputTensorInt(LLML_InterpreterHandle_t handle, int index, const void *src) {
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
		TfLiteTensor *tensor = interpreter->input_tensor(index);
		if (tensor) {
			void *dst = nullptr;
			if (tensor->type == kTfLiteInt32) {
				dst = (void *)interpreter->typed_input_tensor<int32_t>(index);
			} else if (tensor->type == kTfLiteUInt32) {
				dst = (void *)interpreter->typed_input_tensor<uint32_t>(index);
			}
			if (dst != nullptr && src != nullptr) {
				memcpy(dst, src, tensor->bytes);
			}
		}
	}
}

extern "C"
void LLML_SetInputTensorFloat(LLML_InterpreterHandle_t handle, int index, const void *src) {
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
		TfLiteTensor *tensor = interpreter->input_tensor(index);
		if (tensor) {
			float *dst = interpreter->typed_input_tensor<float>(index);
			if (dst != nullptr && src != nullptr) {
				memcpy(dst, src, tensor->bytes);
			}
		}
	}
}

extern "C"
void LLML_GetOutputTensorByte(LLML_InterpreterHandle_t handle, int index, void *dst) {
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
		TfLiteTensor *tensor = interpreter->output_tensor(index);
		if (tensor) {
			void *src = nullptr;
			if (tensor->type == kTfLiteInt8) {
				src = (void *)interpreter->typed_output_tensor<int8_t>(index);
			} else if (tensor->type == kTfLiteUInt8) {
				src = (void *)interpreter->typed_output_tensor<uint8_t>(index);
			}
			if (dst != nullptr && src != nullptr) {
				memcpy(dst, src, tensor->bytes);
			}
		}
	}
}

extern "C"
void LLML_GetOutputTensorInt(LLML_InterpreterHandle_t handle, int index, void *dst) {
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
		TfLiteTensor *tensor = interpreter->output_tensor(index);
		if (tensor) {
			void *src = nullptr;
			if (tensor->type == kTfLiteInt32) {
				src = (void *)interpreter->typed_output_tensor<int32_t>(index);
			} else if (tensor->type == kTfLiteUInt32) {
				src = (void *)interpreter->typed_output_tensor<uint32_t>(index);
			}
			if (dst != nullptr && src != nullptr) {
				memcpy(dst, src, tensor->bytes);
			}
		}
	}
}

extern "C"
void LLML_GetOutputTensorFloat(LLML_InterpreterHandle_t handle, int index, void *dst) {
	TFliteInterpreter *interpreter = getInterpreter(handle);
	if (interpreter) {
		TfLiteTensor *tensor = interpreter->output_tensor(index);
		if (tensor) {
			float *src = interpreter->typed_output_tensor<float>(index);
			if (dst != nullptr && src != nullptr) {
				memcpy(dst, src, tensor->bytes);
			}
		}
	}
}
