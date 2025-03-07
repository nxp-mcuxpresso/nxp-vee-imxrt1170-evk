/*
 * Copyright 2023-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */
package com.nxp.example.animatedmascot;

import ej.bon.Util;
import ej.microui.display.Display;
import ej.microvg.Matrix;

/**
 * TouchAnimation is the animation that is launched when the user touch the screen.
 */
public class TouchAnimation {

	private final Matrix matrix;
	private final long startTime;

	/**
	 * TouchAnimation constructor.
	 *
	 * @param x
	 *            the x coordinate of the animation.
	 * @param y
	 *            the y coordinate of the animation.
	 */
	public TouchAnimation(int x, int y) {
		Display d = Display.getDisplay();

		/*
		 * Set the starting point coordinates of the animation and her size depending on where the user touches the
		 * screen.
		 */
		float scale = 1.4f - ( y / (float) d.getHeight());
		this.matrix = new Matrix();
		this.matrix.setScale(scale, scale);
		this.matrix.postTranslate(x - (scale * AnimatedMascot.TOUCH_IMAGE.getWidth() / 2),
				y - (scale * AnimatedMascot.TOUCH_IMAGE.getHeight() / 2));

		this.startTime = Util.platformTimeMillis();
	}

	/**
	 * Gets the matrix.
	 *
	 * @return the matrix.
	 */
	public Matrix getMatrix() {
		return this.matrix;
	}

	/**
	 * Gets the startTime.
	 *
	 * @return the startTime.
	 */
	public long getStartTime() {
		return this.startTime;
	}

}
