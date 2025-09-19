/*
 * Java
 *
 * Copyright 2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */
package ej.microai.test;

import ej.microai.InputTensor;
import ej.microai.MLInferenceEngine;
import ej.microai.OutputTensor;
import ej.microai.Tensor;
import ej.microai.util.TestUtilities;
import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

public class MLIntModelTests {
    private static final String INT_MODEL = "/model/multiply_model_int32.tflite";
    private static final int MEMORY_POOL = 2048;
    private static final int TEST_INPUT_1 = 9;
    private static final int TEST_OUTPUT_1 = 90;

    private static MLInferenceEngine mlInferenceEngine;
    private static InputTensor inputTensor;
    private static OutputTensor outputTensor;

    @BeforeClass
    public static void beforeClass() {
        mlInferenceEngine = new MLInferenceEngine(INT_MODEL, MEMORY_POOL);
        inputTensor = mlInferenceEngine.getInputTensor(0);
        outputTensor = mlInferenceEngine.getOutputTensor(0);
    }

    @AfterClass
    public static void afterClass() {
        mlInferenceEngine.close();
    }

    @Test
    public void testRunInference() {
        int[] inputData = new int[inputTensor.getNumberElements()];
        int[] outputData = new int[outputTensor.getNumberElements()];

        inputData[0] = TEST_INPUT_1;

        inputTensor.setInputData(inputData);
        mlInferenceEngine.run();
        outputTensor.getOutputData(outputData);

        assertEquals("Multiply model inference failed with " + TEST_INPUT_1 + " input, expected " + TEST_OUTPUT_1 + " but got " + outputData[0],TEST_OUTPUT_1, outputData[0]);

    }
}
