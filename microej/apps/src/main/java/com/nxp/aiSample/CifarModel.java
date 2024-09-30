/**
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
package com.nxp.aiSample;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;

import com.nxp.api.ai.AI;

import ej.bon.Constants;
import ej.microui.display.Image;

/**
 *
 */
public class CifarModel {
    private static final String MICROUI_PROPERTY_RUN_ON_S3 = "com.microej.library.microui.onS3"; //$NON-NLS-1$

    int model_handle = -1;
    ArrayList<String> labels;

    int modelInputLength = 1;
    byte modelInputData[];

    int modelOutputLength = 1;
    float modelOutputData[];

    class ImgRes {
        Image image = null;
        byte[] image_norm = null;
    };

    public CifarModel(String model_path, String labels_path, int arenaSize) throws IOException {
        /* Initialize model */
        if (Constants.getBoolean(MICROUI_PROPERTY_RUN_ON_S3)) {
            this.model_handle = AI.TfLiteMicroSetUpSimu(CifarTestVectors.mockInDim, AI.kTfLiteMicroInt8,
                    CifarTestVectors.mockOutDim, AI.kTfLiteMicroFloat32, null, null, CifarTestVectors.mockResultsValues);
        } else {
            this.model_handle = AI.TfLiteMicroSetUp(model_path.getBytes(), model_path.getBytes().length, arenaSize);
        }
        if (this.model_handle == -1) {
            System.out.println("Failed to initialize model");
            throw new RuntimeException();
        }

        this.labels = getLabels(labels_path);

        getModelCharacteristics();

        this.modelOutputData = new float[this.modelOutputLength];
    }

    void getModelCharacteristics() {
        int idx = 0;
        /* Get input tensor data */
        int inputSize = AI.GetInputTensorSize(this.model_handle, idx);
        int[] inputDims = new int[inputSize];

        AI.GetInputTensorDims(this.model_handle, idx, inputDims);

        int inputType = AI.GetInputTensorType(this.model_handle, idx);

        for (int i = 0; i < inputSize; i++) {
            this.modelInputLength *= inputDims[i];
        }

        System.out.println("Input size: " + inputSize);
        System.out.print("Input dimensions: ");
        for (int i = 0; i < inputSize; i++) {
            System.out.print(inputDims[i] + " ");
        }
        System.out.println("\r\nInput type: " + AI.GetTfTypeName(inputType) + "\r\n");

        /* Get output tensor data */
        int outputSize = AI.GetOutputTensorSize(this.model_handle, idx);
        int[] outputDims = new int[outputSize];

        AI.GetOutputTensorDims(this.model_handle, idx, outputDims);

        int outputType = AI.GetOutputTensorType(this.model_handle, idx);

        for (int i = 0; i < outputSize; i++) {
            this.modelOutputLength *= outputDims[i];
        }

        System.out.println("Output size: " + outputSize);
        System.out.print("Output dimensions: ");
        for (int i = 0; i < outputSize; i++) {
            System.out.print(outputDims[i] + " ");
        }
        System.out.println("\r\nOutput type: " + AI.GetTfTypeName(outputType) + "\r\n");
    }

    public float[] RunInference(byte[] image) {
        setModelInputData(image);

        long startTime = System.currentTimeMillis();
        AI.RunInference(this.model_handle);
        long stopTime = System.currentTimeMillis();
        System.out.println("Inference took: " + (stopTime - startTime) + " ms");

        return getModelOutputData();
    }

    void setModelInputData(byte[] image) {
        AI.SetInputData_Int8(this.model_handle, 0, image, this.modelInputLength);
    }

    float[] getModelOutputData() {
        AI.GetOutputData_Float32(this.model_handle, 0, this.modelOutputData, this.modelOutputLength);

        return this.modelOutputData;
    }

    public ArrayList<String> getLabels() {
        return this.labels;
    }

    private ArrayList<String> getLabels(String labels_path) throws IOException {
        ArrayList<String> labels = new ArrayList<String>();

        InputStream inputStreamStrings = getClass().getResourceAsStream(labels_path);

        StringBuffer sb = new StringBuffer();

        while (inputStreamStrings.available() > 0) {
            char c = (char) inputStreamStrings.read();

            if (c == '\n') {
                labels.add(sb.toString());
                sb = new StringBuffer();
            } else {
                sb.append(c);
            }
        }
        return labels;
    }

    ImgRes get_image(String imagePath) {

        ImgRes imgres = new ImgRes();
        imgres.image = Image.getImage(imagePath);
        imgres.image_norm = new byte[32 * 32 * 3];

        int ii = 0;
        for (int i = 0; i < 32; i++) {
            for (int j = 0; j < 32; j++) {
                int f = imgres.image.readPixel(j, i);
                imgres.image_norm[ii + 2] = (byte) (f & 0xFF);
                imgres.image_norm[ii + 1] = (byte) ((f >> 8) & 0xFF);
                imgres.image_norm[ii] = (byte) ((f >> 16) & 0xFF);
                ii += 3;
            }
        }
        for (int i = 0; i < imgres.image_norm.length; i++) {
            int a = imgres.image_norm[i];
            a = a - 127;
            imgres.image_norm[i] = (byte) a;
        }
        return imgres;
    }
}
