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
import ej.microai.util.RawBmpReader;

import java.io.IOException;

import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;

import static org.junit.Assert.assertTrue;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.fail;


public class MLInferenceEngineLSTMTests {

    private static final String MNIST_LSTM_MODEL = "/model/trained_lstm_int8.tflite";
    private static final int MEMORY_POOL = 6000;
    private static MLInferenceEngine mlInferenceEngine;
    private static InputTensor inputTensor;
    private static OutputTensor outputTensor;

    @BeforeClass
    public static void beforeTest() {
        mlInferenceEngine = new MLInferenceEngine(MNIST_LSTM_MODEL, MEMORY_POOL);
        inputTensor = mlInferenceEngine.getInputTensor(0);
        outputTensor = mlInferenceEngine.getOutputTensor(0);
    }

    @AfterClass
    public static void afterTest() {
        mlInferenceEngine.close();
    }

    /**
     * Compares the inference results on the LSTM MNIST Model from tflite examples.
     * https://github.com/tensorflow/tflite-micro/tree/main/tensorflow/lite/micro/examples/mnist_lstm
     * Without resetting the model, tests show that the digit 9 is predicted incorrectly as 3.
     * When resetting the model, tests show that the digit 9 is predicted correctly as 9.
     * So this test just runs the inferences twice and compares the results, there must be a different set of results.
     */
    @Test
    public void testRunInferencesOnStatefulModel() {
        boolean difference_found = false;
        int[] results_without_reset = new int[10];
        for (int i = 0; i<10; i++) {
            results_without_reset[i] = runInference("/samples/sample" + i + ".bmp");
        }

        for (int i = 0; i<10; i++) {
            int result_with_reset = runInference("/samples/sample" + i + ".bmp");
            mlInferenceEngine.reset();
            if (result_with_reset != results_without_reset[i]) {
                difference_found = true;
            }
        }
        assertTrue("The reset() API had no effect", difference_found);
    }

    /**
     * Runs an inference and returns the digit prediction.
     *
     * @param imagePath the input image.
     * @return the digit prediction
     */
    private int runInference(String imagePath) {
        byte[] inputData = null;
        byte[] outputData = new byte[outputTensor.getNumberElements()];

        try {
            RawBmpReader BmpImage = new RawBmpReader(imagePath);
            inputData = BmpImage.getPixelData();
        } catch (IOException e) {
            fail("Could not load image " + imagePath + " with exception: " + e);
        }
        assertNotNull(inputData);

        // Quantize input data
        Tensor.QuantizationParameters inputParameters = inputTensor.getQuantizationParams();
        for (int i = 0; i < inputTensor.getNumberElements(); i++) {
            inputData[i] = (byte)(inputData[i] + inputParameters.getZeroPoint());
        }

        inputTensor.setInputData(inputData);
        mlInferenceEngine.run();
        outputTensor.getOutputData(outputData);

        // Get the max value, i.e. the digit prediction.
        float maxConfidence = 0;
        int maxIndex = 0;
        for (int i = 0; i < outputTensor.getNumberElements(); i++) {
            if (outputData[i] > maxConfidence) {
                maxConfidence = outputData[i];
                maxIndex = i;
            }
        }

        return maxIndex;
    }
}
