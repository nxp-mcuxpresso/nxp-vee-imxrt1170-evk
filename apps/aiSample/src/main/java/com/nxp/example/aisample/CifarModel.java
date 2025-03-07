/*
 * Copyright 2024 NXP
 * Copyright 2025 MicroEJ Corp. This file has been modified by MicroEJ Corp.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
package com.nxp.example.aisample;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.logging.Logger;

import com.nxp.api.ai.AI;

import ej.bon.Constants;
import ej.microui.display.Image;

/**
 * The <code>CifarModel</code> class offers services to manipulate a Cifar 10 Model.
 * <p>
 * Constructor takes the model path, the label list path and the arena size as input.
 * <p>
 * To run an inference on the model, it takes a 32x32 pixels image as input.
 * The output gives the label of the image recognized and the model confidence.
 */
public class CifarModel {
    private static final Logger LOGGER = Logger.getLogger("[CifarModel]");
    private static final String MICROUI_PROPERTY_RUN_ON_S3 = "com.microej.library.microui.onS3"; //$NON-NLS-1$

    private final int modelHandle;
    private ArrayList<String> labels;

    private int modelInputLength = 1;
    private byte[] modelInputData;

    private int modelOutputLength = 1;
    private final float[] modelOutputData;

    private int confidence;
    private String imageLabel;

    /**
     * CifarModel constructor.
     *
     * @param modelPath the cifar model path.
     * @param labelsPath the label list path.
     * @param arenaSize the arena size.
     * @throws IOException if the model initialization failed or if label list can not be processed.
     */
    public CifarModel(String modelPath, String labelsPath, int arenaSize) throws IOException {
        // Initialize the model.
        if (Constants.getBoolean(MICROUI_PROPERTY_RUN_ON_S3)) {
            // If the model runs on the Simulator, initialize it with mocked tensors.
            this.modelHandle = AI.TfLiteMicroSetUpSimu(CifarTestVectors.MOCK_INPUT_TENSOR_DIMENSIONS, AI.kTfLiteMicroInt8,
                    CifarTestVectors.MOCK_OUTPUT_TENSOR_DIMENSIONS, AI.kTfLiteMicroFloat32, null, null, CifarTestVectors.MOCK_RESULTS_VALUES);
        } else {
            this.modelHandle = AI.TfLiteMicroSetUp(modelPath.getBytes(), modelPath.getBytes().length, arenaSize);
        }
        if (this.modelHandle == -1) {
            LOGGER.info("Failed to initialize model");
            throw new IOException();
        }

        // Process the label list.
        populateLabels(labelsPath);

        // Get the model characteristics.
        getModelCharacteristics();

        this.modelInputData = new byte[this.modelInputLength];
        this.modelOutputData = new float[this.modelOutputLength];
    }

    /**
     * Runs an inference on the model with an image as input.
     * @param imagePath the input image path.
     */
    public void runInference(String imagePath) {
        // Load input data tensor.
        LOGGER.info("Image path : " + imagePath);
        processImage(imagePath);
        AI.SetInputData_Int8(this.modelHandle, 0, this.modelInputData, this.modelInputLength);

        // Run the inference on the model.
        long startTime = System.currentTimeMillis();
        AI.RunInference(this.modelHandle);
        long stopTime = System.currentTimeMillis();
        LOGGER.info("Inference took: " + (stopTime - startTime) + " ms");

        // Get output data tensor.
        AI.GetOutputData_Float32(this.modelHandle, 0, this.modelOutputData, this.modelOutputLength);

        // Process output data to get the image label and the confidence.
        processOutputTensor();
    }

    /**
     * Gets the confidence.
     * @return the confidence.
     */
    public int getConfidence() {
        return confidence;
    }

    /**
     * Gets the image label.
     * @return the image label.
     */
    public String getImageLabel() {
        return imageLabel;
    }

    /**
     * Process the label list of the mode.
     * @param labelsPath the path of the label list file.
     * @throws IOException if an I/O error occurs.
     */
    private void populateLabels(String labelsPath) throws IOException{
        this.labels = new ArrayList<>();

        try(InputStream inputStreamStrings = getClass().getResourceAsStream(labelsPath)) {

            StringBuilder sb = new StringBuilder();

            if(inputStreamStrings != null) {
                while (inputStreamStrings.available() > 0) {
                    char c = (char) inputStreamStrings.read();

                    if (c == '\n') {
                        labels.add(sb.toString());
                        sb = new StringBuilder();
                    } else {
                        sb.append(c);
                    }
                }
            }
        }
    }

    /**
     * Gets, stores and logs the model characteristics.
     */
    private void getModelCharacteristics() {
        int index = 0;

        /* Get input tensor data */
        int inputSize = AI.GetInputTensorSize(this.modelHandle, index);
        int[] inputDims = new int[inputSize];

        AI.GetInputTensorDims(this.modelHandle, index, inputDims);

        int inputType = AI.GetInputTensorType(this.modelHandle, index);

        for (int i = 0; i < inputSize; i++) {
            this.modelInputLength *= inputDims[i];
        }

        LOGGER.info("Input size: " + inputSize);
        StringBuilder inputDimensions = new StringBuilder("Input dimensions: ");
        for (int i = 0; i < inputSize; i++) {
            inputDimensions.append(inputDims[i]).append(" ");
        }
        LOGGER.info(inputDimensions.toString());
        LOGGER.info("Input type: " + AI.GetTfTypeName(inputType) + "\r\n");

        /* Get output tensor data */
        int outputSize = AI.GetOutputTensorSize(this.modelHandle, index);
        int[] outputDims = new int[outputSize];

        AI.GetOutputTensorDims(this.modelHandle, index, outputDims);

        int outputType = AI.GetOutputTensorType(this.modelHandle, index);

        for (int i = 0; i < outputSize; i++) {
            this.modelOutputLength *= outputDims[i];
        }
        LOGGER.info(inputDimensions.toString());
        LOGGER.info("Input type: " + AI.GetTfTypeName(inputType) + "\r\n");

        LOGGER.info("Output size: " + outputSize);
        StringBuilder outputDimensions = new StringBuilder("Output dimensions: ");
        for (int i = 0; i < outputSize; i++) {
            outputDimensions.append(outputDims[i]).append(" ");
        }
        LOGGER.info(outputDimensions.toString());
        LOGGER.info("Output type: " + AI.GetTfTypeName(outputType) + "\r\n");
    }

    /**
     * Process an image into the tensor input format.
     * @param imagePath the path of the image to process.
     */
    private void processImage(String imagePath) {
        Image image = Image.getImage(imagePath);
        this.modelInputData = new byte[32 * 32 * 3];

        // Stores the image pixels inside the tensor input format.
        int ii = 0;
        for (int i = 0; i < 32; i++) {
            for (int j = 0; j < 32; j++) {
                int f = image.readPixel(j, i);
                this.modelInputData[ii + 2] = (byte) (f & 0xFF);
                this.modelInputData[ii + 1] = (byte) ((f >> 8) & 0xFF);
                this.modelInputData[ii] = (byte) ((f >> 16) & 0xFF);
                ii += 3;
            }
        }
        // Normalize the data.
        for (int i = 0; i < this.modelInputData.length; i++) {
            int a = this.modelInputData[i];
            a = a - 127;
            this.modelInputData[i] = (byte) a;
        }
    }

    /**
     * Process the output tensor to extract the image label and the confidence.
     */
    private void processOutputTensor() {
        /* Get max value of output tensor */
        float max = 0;
        int maxIndex = 0;
        for (int i = 0; i < this.modelOutputData.length; i++) {
            if (this.modelOutputData[i] > max) {
                max = this.modelOutputData[i];
                maxIndex = i;
            }
        }
        this.confidence = (int) (max * 100);
        this.imageLabel = this.labels.get(maxIndex);
    }
}
