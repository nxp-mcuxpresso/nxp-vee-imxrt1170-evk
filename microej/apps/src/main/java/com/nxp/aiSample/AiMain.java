/**
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
package com.nxp.aiSample;

import java.io.IOException;
import java.util.ArrayList;

import com.microej.partial.support.PartialRenderPolicy;

import ej.microui.MicroUI;
import ej.microui.display.Display;
import ej.microui.display.Image;
import ej.mwt.Desktop;
import ej.mwt.render.RenderPolicy;
import ej.widget.container.Canvas;
import com.microej.partial.support.PartialRenderPolicy;

import ej.bon.Constants;
import ej.microui.MicroUI;
import ej.microui.display.Display;
import ej.microui.event.Event;
import ej.microui.event.generator.Buttons;
import ej.microui.event.generator.Pointer;
import ej.mwt.Desktop;
import ej.mwt.render.DefaultRenderPolicy;
import ej.mwt.render.RenderPolicy;
import ej.util.Device;
import ej.widget.container.Canvas;

/**
 * Main class
 */
public class AiMain {

    private final AiWidget aiWidget;
    private static AiMain aiMain;
    Canvas container = null;

    private int index;
    private CifarModel cifarModel;
    private static ArrayList<String> labels = null;
    public static Image image = null;

    public static String label = null;
    public static int confidence = 0;

    public AiMain() {
        // Init AI thread.
        String model_path = "/model/cifarnet_quant_int8.tflite";
        String labels_path = "/model/cifar_labels.txt";
        int arenaSize = 180 * 1024;
        this.index = 0;
        try {
            this.cifarModel = new CifarModel(model_path, labels_path, arenaSize);
            this.labels = this.cifarModel.getLabels();
        } catch (IOException e) {
            this.cifarModel = null;
            e.printStackTrace();
        }

        // Init UI
        Display display = Display.getDisplay();

        Desktop desktop = new Desktop() {
            @Override
            protected RenderPolicy createRenderPolicy() {
              //  if (Constants.getBoolean("com.microej.partial.support.isEnabled")) {
              //      System.out.println("Using PartialRenderPolicy");
              //      return new PartialRenderPolicy(this, false);
              //  } else {
                    System.out.println("Using DefaultRenderPolicy");
                    return new DefaultRenderPolicy(this);
               // }
            }
        };
        this.container = new Canvas();

        this.aiWidget = new AiWidget();
        int offset = 60;
        this.container.addChild(this.aiWidget, offset, offset, 200, 250);
        desktop.setWidget(this.container);

        desktop.requestShow();
        run();
    }

    private void run() {
        boolean running = true;
        while (running) {
            System.out.println("Thread is running...");
            String image_path = "/images/" + String.valueOf(this.index) + ".png";
            System.out.println("image_path: " + image_path);

            CifarModel.ImgRes imgres = this.cifarModel.get_image(image_path);
            this.image = imgres.image;
            byte[] image_norm = imgres.image_norm;

            float[] outputData = this.cifarModel.RunInference(image_norm);

            /* Get max value of output tensor */
            float max = 0;
            int maxIndex = 0;
            for (int i = 0; i < outputData.length; i++) {
                if (outputData[i] > max) {
                    max = outputData[i];
                    maxIndex = i;
                }
            }
            this.confidence = (int) (max * 100);
            this.label = this.labels.get(maxIndex);
            /* Print results */
            System.out.println("Predicted label: " + this.label);
            System.out.println("With confidence: " + this.confidence + "%");

            this.index++;
            this.index %= 10;

            this.container.requestRender();

            try {
                Thread.sleep(2000);
            } catch (InterruptedException e) {
                e.printStackTrace();
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
        aiMain = new AiMain();
    }
}
