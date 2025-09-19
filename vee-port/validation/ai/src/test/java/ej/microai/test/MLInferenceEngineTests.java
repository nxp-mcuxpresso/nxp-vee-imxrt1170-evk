/*
 * Java
 *
 * Copyright 2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */
package ej.microai.test;

import ej.bon.ResourceBuffer;
import ej.microai.InputTensor;
import ej.microai.MLInferenceEngine;
import ej.microai.OutputTensor;
import ej.microai.util.TestUtilities;
import org.junit.Test;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;

import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

public class MLInferenceEngineTests {

    private static final String TEST_MODEL = "/model/sine_model_float.tflite";
    private static final int MEMORY_POOL = 2048;
    private static final float SINUS_TOLERANCE = 0.05f;

    private static final float TEST_INPUT_1 = (float) (Math.PI / 2);
    private static final float TEST_OUTPUT_1 = 1;

    /**
     * Creates a {@link MLInferenceEngine} using {@link MLInferenceEngine#MLInferenceEngine(String)} constructor.
     * Validate the creation by running an inference on it.
     */
    @Test
    public void testCreateModelFromResources() {
        try(MLInferenceEngine mlInferenceEngine = new MLInferenceEngine(TEST_MODEL, MEMORY_POOL)) {
            testInference(mlInferenceEngine);
        }
    }

    /**
     * Creates a {@link MLInferenceEngine} using {@link MLInferenceEngine#MLInferenceEngine(InputStream)} constructor.
     * Validate the creation by running an inference on it.
     */
    @Test
    public void testCreateModelFromInputStream() {
        // Create an inputStream from ResourceBuffer containing the model data.
        try(ResourceBuffer buffer = new ResourceBuffer(TEST_MODEL)) {

            byte[] byteArray = new byte[buffer.available()];
            for (int i = 0; i < byteArray.length; i++) {
                byteArray[i] = buffer.readByte();
            }

            ByteArrayInputStream is = new ByteArrayInputStream(byteArray);

            MLInferenceEngine mlInferenceEngine = new MLInferenceEngine(is, MEMORY_POOL);

            is.close();

            testInference(mlInferenceEngine);
        } catch (IOException e) {
            e.printStackTrace();
            fail(TEST_MODEL + " resource is not available in application classpath");
        }

    }

    /**
     * Runs inferences on sine model and test the output data.
     *
     * @param mlInferenceEngine the inference engine.
     */
    private void testInference(MLInferenceEngine mlInferenceEngine) {
        InputTensor inputTensor = mlInferenceEngine.getInputTensor(0);
        OutputTensor outputTensor = mlInferenceEngine.getOutputTensor(0);

        float[] inputData = new float[inputTensor.getNumberElements()];
        float[] outputData = new float[outputTensor.getNumberElements()];
        inputData[0] = TEST_INPUT_1;
        inputTensor.setInputData(inputData);
        mlInferenceEngine.run();
        outputTensor.getOutputData(outputData);
        mlInferenceEngine.reset();

        assertTrue("Sine model inference failed with " + TEST_INPUT_1 + " input, expected " + TEST_OUTPUT_1 + " but got " + outputData[0], TestUtilities.compareFloat(TEST_OUTPUT_1, outputData[0], SINUS_TOLERANCE));
    }
}
