/*
 * C
 *
 * Copyright 2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef  LLML_CONFIGURATION_H
#define  LLML_CONFIGURATION_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * @brief MicroAI implementation over TensorFlow Lite.
 * @author MicroEJ Developer Team
 * @version 2.1.0
 */

#if __has_include(<veeport_configuration.h>)
  #include <veeport_configuration.h>
#endif

/**
 * Configure ML Inference Engine Backend
 */
#define LLML_USE_TFLITE_MICRO 1
#define LLML_USE_TFLITE 2

#define LLML_BACKEND LLML_USE_TFLITE_MICRO

#if !(LLML_BACKEND == LLML_USE_TFLITE_MICRO) && !(LLML_BACKEND == LLML_USE_TFLITE)
#error "LLML: No backend selected -> Chose TensorFlow Lite / TensorFlow Lite for Microcontrollers"
#endif

/**
 * Enable LLML debug trace
 */
//#define LLML_DEBUG

#ifdef LLML_DEBUG

#ifdef MCUXPRESSO_SDK
#include "fsl_debug_console.h"
#define LLML_DEBUG_TRACE PRINTF("[LLML] %s(%d) ", __func__, __LINE__); PRINTF
#else
#define LLML_DEBUG_TRACE printf("[LLML] %s(%d) ", __func__, __LINE__); printf
#endif

#else
#define LLML_DEBUG_TRACE(...) ((void)0)
#endif

#ifdef __cplusplus
}
#endif

#endif // LLML_CONFIGURATION_H
