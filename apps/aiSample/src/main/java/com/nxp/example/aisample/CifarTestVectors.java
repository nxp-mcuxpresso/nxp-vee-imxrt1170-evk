/*
 * Copyright 2024 NXP
 * Copyright 2025 MicroEJ Corp. This file has been modified by MicroEJ Corp.
 *
 * SPDX-License-IdentifierBSD-3-Clause
 */
package com.nxp.example.aisample;

/**
 *
 */
public class CifarTestVectors {

    /**
     * Input tensor dimensions used when running on the Simulator.
     */
    protected static final int[] MOCK_INPUT_TENSOR_DIMENSIONS = new int[] { 1, 32, 32, 3 };

    /**
     * Output tensor dimensions used when running on the Simulator.
     */
    protected static final int[] MOCK_OUTPUT_TENSOR_DIMENSIONS = new int[] { 1, 10 };

    /**
     * Mock result values used when running on the Simulator.
     */
    protected static final float[] MOCK_RESULTS_VALUES = new float[] { 0.0f, 0.0f, 0.0f, 0.99609375f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, // 0.png
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.99609375f, 0.0f, // /1.png
        0.01171875f, 0.0234375f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.9609375f, 0.0f, // 2.png
        0.8828125f, 0.0f, 0.0f, 0.0078125f, 0.00390625f, 0.0f, 0.0f, 0.0f, 0.10546875f, 0.0f, // 3.png
        0.0f, 0.0f, 0.0f, 0.0f, 0.10546875f, 0.0f, 0.89453125f, 0.0f, 0.0f, 0.0f, // 4.png
        0.0f, 0.0f, 0.0f, 0.61328125f, 0.0f, 0.015625f, 0.37109375f, 0.0f, 0.0f, 0.0f, // 5.png
        0.0f, 0.91796875f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.08203125f, // 6.png
        0.0f, 0.0f, 0.25f, 0.01171875f, 0.53515625f, 0.00390625f, 0.1953125f, 0.00390625f, 0.0f, 0.0f, // 7.png
        0.0f, 0.0f, 0.0f, 0.99609375f, 0.0f, 0.00390625f, 0.0f, 0.0f, 0.0f, 0.0f, // 8.png
        0.0f, 0.7734375f, 0.00390625f, 0.00390625f, 0.0f, 0.0f, 0.078125f, 0.0f, 0.0390625f, 0.1015625f, // 9.png
    };


    private CifarTestVectors() {
        // Prevent instantiation.
    }

}
