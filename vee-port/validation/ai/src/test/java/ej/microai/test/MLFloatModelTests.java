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
import ej.microai.util.TestUtilities;
import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;

import static org.junit.Assert.assertTrue;

public class MLFloatModelTests {
    private static final String FLOAT_MODEL = "/model/sine_model_float.tflite";
    private static final int MEMORY_POOL = 2048;
    private static final float SINUS_TOLERANCE = 0.05f;
    private static final float TEST_INPUT_1 = (float) (Math.PI / 2);
    private static final float TEST_OUTPUT_1 = 1;

    private static MLInferenceEngine mlInferenceEngine;
    private static InputTensor inputTensor;
    private static OutputTensor outputTensor;

    @BeforeClass
    public static void beforeClass() {
        mlInferenceEngine = new MLInferenceEngine(FLOAT_MODEL, MEMORY_POOL);
        inputTensor = mlInferenceEngine.getInputTensor(0);
        outputTensor = mlInferenceEngine.getOutputTensor(0);
    }

    @AfterClass
    public static void afterClass() {
        mlInferenceEngine.close();
    }

    @Test
    public void testRunInference() {
        float[] inputData = new float[inputTensor.getNumberElements()];
        float[] outputData = new float[outputTensor.getNumberElements()];
        inputData[0] = TEST_INPUT_1;
        inputTensor.setInputData(inputData);
        mlInferenceEngine.run();
        outputTensor.getOutputData(outputData);

        assertTrue("Sine model inference failed with " + TEST_INPUT_1 + " input, expected " + TEST_OUTPUT_1 + " but got " + outputData[0], TestUtilities.compareFloat(TEST_OUTPUT_1, outputData[0], SINUS_TOLERANCE));
    }
}
