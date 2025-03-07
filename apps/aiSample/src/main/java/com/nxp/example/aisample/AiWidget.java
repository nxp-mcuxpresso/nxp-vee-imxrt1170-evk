/*
 * Copyright 2024 NXP
 * Copyright 2025 MicroEJ Corp. This file has been modified by MicroEJ Corp.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
package com.nxp.example.aisample;

import ej.drawing.TransformPainter;
import ej.microui.display.Colors;
import ej.microui.display.Font;
import ej.microui.display.GraphicsContext;
import ej.microui.display.Image;
import ej.microui.display.Painter;
import ej.mwt.Widget;
import ej.mwt.util.Size;

/**
 * The <code>AiWidget</code> class is a {@link Widget} that displays the image, the label and the confidence.
 */
public class AiWidget extends Widget {

    private static final String DEFAULT_FONT = "/fonts/SourceSansPro_15px-600.ejf";
    private static final String DEFAULT_IMAGE = "/images/0.png";
    private static final String DEFAULT_LABEL = "0 !";
    private static final int DEFAULT_CONFIDENCE = 0;

    private static final int IMAGE_SIZE = 32;
    private static final int MAGNIFY_FACTOR = 5;
    private static final int IMAGE_OFFSET_Y = 10;

    private final Font labelFont;
    private Image image;
    private int confidence;
    private String label;

    /**
     * Public constructor.
     */
    public AiWidget() {
        this.labelFont = Font.getFont(DEFAULT_FONT);
        this.image = Image.getImage(DEFAULT_IMAGE);
        this.confidence = DEFAULT_CONFIDENCE;
        this.label = DEFAULT_LABEL;
    }

    @Override
    protected void computeContentOptimalSize(Size size) {
        // Do nothing
    }

    @Override
    protected void renderContent(GraphicsContext g, int contentWidth, int contentHeight) {

        TransformPainter.drawScaledImageNearestNeighbor(g, this.image, 0, 0, MAGNIFY_FACTOR, MAGNIFY_FACTOR);

        String confidenceLabel = "Confidence: " + this.confidence + " %";
        int offsetLabelY = IMAGE_SIZE * MAGNIFY_FACTOR + IMAGE_OFFSET_Y;
        g.setColor(Colors.RED);
        Painter.drawString(g, this.label, this.labelFont,
                 this.getX() + (this.image.getWidth() - this.labelFont.stringWidth(this.label)) / 2, offsetLabelY);
        Painter.drawString(g, confidenceLabel, this.labelFont,
                this.getX() + (this.image.getWidth() - this.labelFont.stringWidth(confidenceLabel)) / 2, offsetLabelY + this.labelFont.getHeight());
    }

    /**
     * Sets the displayed image.
     * @param imagePath the image path.
     */
    public void setImage(String imagePath) {
        this.image = Image.getImage(imagePath);
    }

    /**
     * Sets the displayed confidence.
     * @param confidence the confidence.
     */
    public void setConfidence(int confidence) {
        this.confidence = confidence;
    }

    /**
     * Sets the displayed label.
     * @param label the label.
     */
    public void setLabel(String label) {
        this.label = label;
    }
}
