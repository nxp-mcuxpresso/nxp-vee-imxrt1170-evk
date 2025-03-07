/*
 * Copyright 2024 NXP
 * Copyright 2025 MicroEJ Corp. This file has been modified by MicroEJ Corp.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

package com.nxp.example.simplegfx;


import ej.microui.display.GraphicsContext;
import ej.microui.display.Painter;
import ej.mwt.Widget;
import ej.mwt.animation.Animation;
import ej.mwt.util.Size;

/**
 *
 */
public class SimpleGFXWidget extends Widget implements Animation {

    private static final int NXP_ORANGE = 0xF9B500;
    private static final int NXP_BLUE = 0x0EAFE0;
    private static final int NXP_GREEN = 0x69CA00;

    private static final int RECTANGLE_WIDTH = 100;
    private static final int RECTANGLE_HEIGHT = 75;

    private final int widgetSize;

    private int grey = 0xFFFFFF;
    private boolean decreaseGrey = true;

    private int orangeRectangleX;
    private int orangeRectangleY;

    private int blueRectangleX;
    private int blueRectangleY;

    private int greenRectangleX;
    private int greenRectangleY;

    private int currentAnimationStep;

    /**
     * SimpleGFX widget constructor.
     * @param widgetSize the widget size.
     */
    public SimpleGFXWidget(int widgetSize) {
        this.currentAnimationStep = 0;
        this.widgetSize = widgetSize;
    }

    @Override
    public boolean tick(long platformTimeMillis) {
        // update widget state
        this.currentAnimationStep++;

        // update color of the circle
        if (decreaseGrey) {
            this.grey -= 0x010101;
            if (this.grey == 0x808080) {
                this.decreaseGrey = false;
            }
        } else {
            this.grey += 0x010101;
            if (this.grey == 0xffffff) {
                this.decreaseGrey = true;
            }
        }

        // Update position of the rectangles
        this.orangeRectangleX = SimpleGFXNatives.getOrangeRectangleX(this.currentAnimationStep, RECTANGLE_WIDTH, RECTANGLE_HEIGHT, this.widgetSize);
        this.orangeRectangleY = SimpleGFXNatives.getOrangeRectangleY(this.currentAnimationStep, RECTANGLE_WIDTH, RECTANGLE_HEIGHT, this.widgetSize);

        this.blueRectangleX = SimpleGFXNatives.getBlueRectangleX(this.currentAnimationStep, RECTANGLE_WIDTH, RECTANGLE_HEIGHT, this.widgetSize);
        this.blueRectangleY = SimpleGFXNatives.getBlueRectangleY(this.currentAnimationStep, RECTANGLE_WIDTH, RECTANGLE_HEIGHT, this.widgetSize);

        this.greenRectangleX = SimpleGFXNatives.getGreenRectangleX(this.currentAnimationStep, RECTANGLE_WIDTH, RECTANGLE_HEIGHT, this.widgetSize);
        this.greenRectangleY = SimpleGFXNatives.getGreenRectangleY(this.currentAnimationStep, RECTANGLE_WIDTH, RECTANGLE_HEIGHT, this.widgetSize);

        // request new render
        this.requestRender();

        // return whether to continue or to stop the animation
        return true;
    }

    @Override
    protected void onShown() {
        // start animation
        getDesktop().getAnimator().startAnimation(this);
    }

    @Override
    protected void onHidden() {
        // stop animation
        getDesktop().getAnimator().stopAnimation(this);
    }

    @Override
    protected void computeContentOptimalSize(Size size) {
        // Do nothing
    }

    @Override
    protected void renderContent(GraphicsContext g, int contentWidth, int contentHeight) {
        g.setColor(grey);
        Painter.fillCircle(g, 0, 0, this.widgetSize);

        g.setColor(NXP_ORANGE);
        Painter.fillRectangle(g, this.orangeRectangleX, this.orangeRectangleY, RECTANGLE_WIDTH, RECTANGLE_HEIGHT);

        g.setColor(NXP_BLUE);
        Painter.fillRectangle(g, this.blueRectangleX, this.blueRectangleY, RECTANGLE_WIDTH, RECTANGLE_HEIGHT);

        g.setColor(NXP_GREEN);
        Painter.fillRectangle(g, this.greenRectangleX, this.greenRectangleY, RECTANGLE_WIDTH, RECTANGLE_HEIGHT);
    }
}
