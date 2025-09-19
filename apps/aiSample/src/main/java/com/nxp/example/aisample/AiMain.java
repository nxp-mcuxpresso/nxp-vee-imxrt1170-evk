/*
 * Copyright 2024 NXP
 * Copyright 2025 MicroEJ Corp. This file has been modified by MicroEJ Corp.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
package com.nxp.example.aisample;

import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;


import ej.microui.MicroUI;
import ej.mwt.Desktop;
import ej.widget.container.Canvas;


/**
 * The <code>AiMain</code> class creates a cifar model and runs inference on it.
 */
public class AiMain {
    private static final Logger LOGGER = Logger.getLogger("[AiMain]");

    private static final String MODEL_PATH = "/model/cifarnet_quant_int8.tflite";
    private static final String LABELS_PATH = "/model/cifar_labels.txt";
    private static final int ARENA_SIZE = 45 * 1024; // must fit into microai.heap.size
    private static final int AI_WIDGET_X = 60;
    private static final int AI_WIDGET_Y = 60;
    private static final int AI_WIDGET_WIDTH = 200;
    private static final int AI_WIDGET_HEIGHT = 250;

    private AiWidget aiWidget;

    private int index;
    private CifarModel cifarModel;

    /**
     * AiMain constructor.
     */
    public AiMain() {
        initAI();
        initUI();
        run();
    }

    /**
     * Initializes the AI of the application.
     */
    private void initAI() {
        this.index = 0;
        try {
            this.cifarModel = new CifarModel(MODEL_PATH, LABELS_PATH, ARENA_SIZE);
        } catch (IOException e) {
            this.cifarModel = null;
            LOGGER.log(Level.SEVERE, "Issue while initiating the model or processing the label list, terminate the program", e);
            System.exit(-1);
        }
    }

    /**
     * Initializes the UI of the application.
     */
    private void initUI() {
        Desktop desktop = new Desktop();

        Canvas container = new Canvas();

        this.aiWidget = new AiWidget();
        container.addChild(this.aiWidget, AI_WIDGET_X, AI_WIDGET_Y, AI_WIDGET_WIDTH, AI_WIDGET_HEIGHT);
        desktop.setWidget(container);

        desktop.requestShow();
    }

    private void run() {
        boolean running = true;
        while (running) {
            LOGGER.info("Thread is running...");
            String imagePath = "/images/" + this.index + ".png";

            this.cifarModel.runInference(imagePath);

            // Log results
            LOGGER.info("Predicted label: " + this.cifarModel.getImageLabel());
            LOGGER.info("With confidence: " + this.cifarModel.getConfidence() + "% \r\n");

            this.index++;
            this.index %= 10;

            this.aiWidget.setImage(imagePath);
            this.aiWidget.setConfidence(this.cifarModel.getConfidence());
            this.aiWidget.setLabel(this.cifarModel.getImageLabel());

            this.aiWidget.requestRender();

            try {
                Thread.sleep(2000);
            } catch (InterruptedException e) {
                LOGGER.log(Level.SEVERE, "Thread interrupted", e);
                running = false;
                Thread.currentThread().interrupt();
            }
        }
    }

    /**
     * Simple main.
     *
     * @param args
     *            command line arguments.
     */
    public static void main(String[] args) {
        MicroUI.start();
        new AiMain();
    }
}
