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
import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;

import java.util.Arrays;

import static org.junit.Assert.assertEquals;

public class MLByteModelTests {
    private static final String BYTE_MODEL = "/model/mean_model_int8.tflite";
    private static final int MEMORY_POOL = 2048;
    private static final byte[] TEST_INPUT = new byte[] {10, 20, 30, 40, 50};
    private static final byte TEST_OUTPUT = 30;

    private static MLInferenceEngine mlInferenceEngine;
    private static InputTensor inputTensor;
    private static OutputTensor outputTensor;

    @BeforeClass
    public static void beforeClass() {
        mlInferenceEngine = new MLInferenceEngine(BYTE_MODEL, MEMORY_POOL);
        inputTensor = mlInferenceEngine.getInputTensor(0);
        outputTensor = mlInferenceEngine.getOutputTensor(0);
    }

    @AfterClass
    public static void afterClass() {
        mlInferenceEngine.close();
    }

    @Test
    public void testRunInference() {
        byte[] inputData = new byte[inputTensor.getNumberElements()];
        byte[] outputData = new byte[outputTensor.getNumberElements()];

        System.arraycopy(TEST_INPUT, 0, inputData, 0, inputData.length);

        inputTensor.setInputData(inputData);
        mlInferenceEngine.run();
        outputTensor.getOutputData(outputData);

        assertEquals("Mean model inference failed with " + Arrays.toString(TEST_INPUT) + " input, expected " + TEST_OUTPUT + " but got " + outputData[0],TEST_OUTPUT, outputData[0]);
    }
}
