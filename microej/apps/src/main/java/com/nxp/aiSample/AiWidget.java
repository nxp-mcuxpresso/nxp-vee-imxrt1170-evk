/**
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
package com.nxp.aiSample;

import ej.drawing.TransformPainter;
import ej.microui.display.Font;
import ej.microui.display.GraphicsContext;
import ej.microui.display.Painter;
import ej.mwt.Widget;
import ej.mwt.animation.Animation;
import ej.mwt.util.Size;

/**
 *
 */
public class AiWidget extends Widget implements Animation {

    final static int IMG_SIZE = 32;
    private final Font LABEL_FONT;
    private int t;

    /**
     * Public constructor.
     */
    public AiWidget() {
        this.t = 0;
        this.LABEL_FONT = Font.getFont("/fonts/SourceSansPro_15px-600.ejf");
    }

    @Override
    protected void computeContentOptimalSize(Size size) {
        // Do nothing
    }

    @Override
    protected void onShown() {
        // start animation
        getDesktop().getAnimator().startAnimation(this);
    }

    @Override
    public boolean tick(long platformTimeMillis) {
        // update widget state
        this.t++;
        // return whether to continue or to stop the animation
        return true;
    }

    @Override
    protected void renderContent(GraphicsContext g, int contentWidth, int contentHeight) {
        if ((AiMain.image == null) || (AiMain.label == null)) {
            return;
        }

        TransformPainter.drawScaledImageNearestNeighbor(g, AiMain.image, 0, 0, 5.f, 5.f);

        int magnify_factor = 5;
        String confidenceLabel = "Confidence: " + AiMain.confidence + " %";
        int offset_label_y = IMG_SIZE * magnify_factor + 10;
        g.setColor(0xFF0000);
        Painter.drawString(g, AiMain.label, this.LABEL_FONT,
                60 + (AiMain.image.getWidth() - this.LABEL_FONT.stringWidth(AiMain.label)) / 2, offset_label_y);
        Painter.drawString(g, confidenceLabel, this.LABEL_FONT,
                60 + (AiMain.image.getWidth() - this.LABEL_FONT.stringWidth(confidenceLabel)) / 2, offset_label_y + 20);
    }
}
