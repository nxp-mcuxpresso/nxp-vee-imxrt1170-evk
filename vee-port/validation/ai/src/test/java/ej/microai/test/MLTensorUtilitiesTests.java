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

import java.util.Arrays;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

public class MLTensorUtilitiesTests {
    private static final String TEST_MODEL = "/model/sine_model_int8_input_float_output.tflite";
    private static final int MEMORY_POOL = 2048;

    /*
     * sine_model_int8_input_float_output.tflite input/output tensor information.
     */
    private static final int INPUT_DATA_TYPE = Tensor.DataType.INT8;
    private static final int OUTPUT_DATA_TYPE = Tensor.DataType.FLOAT32;
    private static final int INPUT_NUMBER_BYTES = 1;
    private static final int OUTPUT_NUMBER_BYTES = 4;
    private static final int INPUT_NUMBER_DIMENSIONS = 2;
    private static final int OUTPUT_NUMBER_DIMENSIONS = 2;
    private static final int INPUT_NUMBER_ELEMENTS = 1;
    private static final int OUTPUT_NUMBER_ELEMENTS = 1;
    private static final float SCALE_TOLERANCE = 0.005f;
    private static final float INPUT_QUANTIZATION_SCALE = 0.02463994361460209f;
    private static final float OUTPUT_QUANTIZATION_SCALE = 0.0f;
    private static final int INPUT_QUANTIZATION_ZERO_POINT = -128;
    private static final int OUTPUT_QUANTIZATION_ZERO_POINT = 0;
    private static final int[] INPUT_SHAPE = new int[] {1, 1};
    private static final int[] OUTPUT_SHAPE = new int[] {1, 1};


    private static MLInferenceEngine mlInferenceEngine;
    private static InputTensor inputTensor;
    private static OutputTensor outputTensor;

    @BeforeClass
    public static void beforeClass() {
        mlInferenceEngine = new MLInferenceEngine(TEST_MODEL, MEMORY_POOL);
        inputTensor = mlInferenceEngine.getInputTensor(0);
        outputTensor = mlInferenceEngine.getOutputTensor(0);
    }

    @AfterClass
    public static void afterClass() {
        mlInferenceEngine.close();
    }

    @Test
    public void testGetDataType() {
        int dataType = inputTensor.getDataType();
        assertEquals("Wrong input data type", INPUT_DATA_TYPE, dataType);
        dataType = outputTensor.getDataType();
        assertEquals("Wrong output data type", OUTPUT_DATA_TYPE, dataType);
    }

    @Test
    public void testNumberBytes() {
        int numberBytes = inputTensor.getNumberBytes();
        assertEquals("Wrong input number of bytes", INPUT_NUMBER_BYTES, numberBytes);
        numberBytes = outputTensor.getNumberBytes();
        assertEquals("Wrong output number of bytes", OUTPUT_NUMBER_BYTES, numberBytes);
    }

    @Test
    public void testNumberDimensions() {
        int numberDimensions = inputTensor.getNumberDimensions();
        assertEquals("Wrong input number of dimensions", INPUT_NUMBER_DIMENSIONS, numberDimensions);
        numberDimensions = outputTensor.getNumberDimensions();
        assertEquals("Wrong output number of dimensions", OUTPUT_NUMBER_DIMENSIONS, numberDimensions);
    }

    @Test
    public void testNumberElements() {
        int numberElements = inputTensor.getNumberElements();
        assertEquals("Wrong input number of elements", INPUT_NUMBER_ELEMENTS, numberElements);
        numberElements = outputTensor.getNumberElements();
        assertEquals("Wrong output number of elements", OUTPUT_NUMBER_ELEMENTS, numberElements);
    }

    @Test
    public void testQuantizationStatus() {
        assertTrue("Wrong input quantized status, expected true but got false", inputTensor.isQuantized());
        assertFalse("Wrong output quantized status, expected false but got true", outputTensor.isQuantized());
    }

    @Test
    public void testQuantizationParameters() {
        Tensor.QuantizationParameters quantizationParameters = inputTensor.getQuantizationParams();
        assertTrue("Wrong input scale quantization parameter, expected: " + INPUT_QUANTIZATION_SCALE + " but got " + quantizationParameters.getScale(), TestUtilities.compareFloat(INPUT_QUANTIZATION_SCALE, quantizationParameters.getScale(), SCALE_TOLERANCE));
        assertEquals("Wrong input zero point quantization parameter", INPUT_QUANTIZATION_ZERO_POINT, quantizationParameters.getZeroPoint());

        quantizationParameters = outputTensor.getQuantizationParams();
        assertTrue("Wrong output scale quantization parameter, expected: " + OUTPUT_QUANTIZATION_SCALE + " but got " + quantizationParameters.getScale(), TestUtilities.compareFloat(OUTPUT_QUANTIZATION_SCALE, quantizationParameters.getScale(), SCALE_TOLERANCE));
        assertEquals("Wrong output zero point quantization parameter", OUTPUT_QUANTIZATION_ZERO_POINT, quantizationParameters.getZeroPoint());
    }

    @Test
    public void testShape() {
        int[] inputSizes = new int[inputTensor.getNumberDimensions()];
        inputTensor.getShape(inputSizes);
        assertEquals("Wrong input shape", Arrays.toString(INPUT_SHAPE), Arrays.toString(inputSizes));


        int[] outputSizes = new int[outputTensor.getNumberDimensions()];
        outputTensor.getShape(outputSizes);
        System.out.println(Arrays.toString(outputSizes));
        assertEquals("Wrong output shape", Arrays.toString(OUTPUT_SHAPE), Arrays.toString(outputSizes));

    }
}
