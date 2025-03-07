/*
 * Copyright 2022 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

package com.nxp.example.simplegfx;

/**
 *
 */
public class SimpleGFXNatives {

	private static final int AMP_ORANGE = 30;
	private static final int AMP_BLUE = 75;
	private static final int AMP_GREEN = 30;

	/**
	 * Gets the X coordinate of the orange rectangle.
	 *
	 * @param t the animation step.
	 * @param rectangleLength the length of the rectangle.
	 * @param rectangleHeight the height of the rectangle.
	 * @param displaySize the display size.
	 * @return the X coordinate of the rectangle.
	 */
	public static int getOrangeRectangleX(int t, int rectangleLength, int rectangleHeight, int displaySize) {
		int centre = displaySize / 2;
		double sineOrange = AMP_ORANGE * Math.cos(t * Math.PI / 45.0);
		return centre - rectangleLength - rectangleLength / 2 - AMP_ORANGE - (int) sineOrange;
	}

	/**
	 * Gets the Y coordinate of the orange rectangle.
	 *
	 * @param t the animation step.
	 * @param rectangleLength the length of the rectangle.
	 * @param rectangleHeight the height of the rectangle.
	 * @param displaySize the display size.
	 * @return the Y coordinate of the rectangle.
	 */
	public static int getOrangeRectangleY(int t, int rectangleLength, int rectangleHeight, int displaySize) {
		int centre = displaySize / 2;
		return centre - rectangleHeight / 2;
	}

	/**
	 * Gets the X coordinate of the blue rectangle.
	 *
	 * @param t the animation step.
	 * @param rectangleLength the length of the rectangle.
	 * @param rectangleHeight the height of the rectangle.
	 * @param displaySize the display size.
	 * @return the X coordinate of the rectangle.
	 */
	public static int getBlueRectangleX(int t, int rectangleLength, int rectangleHeight, int displaySize) {
		int centre = displaySize / 2;
		return centre - rectangleLength / 2;
	}

	/**
	 * Gets the Y coordinate of the blue rectangle.
	 *
	 * @param t the animation step.
	 * @param rectangleLength the length of the rectangle.
	 * @param rectangleHeight the height of the rectangle.
	 * @param displaySize the display size.
	 * @return the Y coordinate of the rectangle.
	 */
	public static int getBlueRectangleY(int t, int rectangleLength, int rectangleHeight, int displaySize) {
		int centre = displaySize / 2;
		double sineBlue = AMP_BLUE * Math.sin(t * Math.PI / 45.0);
		return centre - rectangleHeight / 2 + (int) sineBlue;
	}

	/**
	 * Gets the X coordinate of the green rectangle.
	 *
	 * @param t the animation step.
	 * @param rectangleLength the length of the rectangle.
	 * @param rectangleHeight the height of the rectangle.
	 * @param displaySize the display size.
	 * @return the X coordinate of the rectangle.
	 */
	public static int getGreenRectangleX(int t, int rectangleLength, int rectangleHeight, int displaySize) {
		int centre = displaySize / 2;
		double sineGreen = AMP_GREEN * Math.cos(t * Math.PI / 45.0);
		return centre + rectangleLength - rectangleLength / 2 + AMP_GREEN + (int) sineGreen;
	}

	/**
	 * Gets the Y coordinate of the green rectangle.
	 *
	 * @param t the animation step.
	 * @param rectangleLength the length of the rectangle.
	 * @param rectangleHeight the height of the rectangle.
	 * @param displaySize the display size.
	 * @return the Y coordinate of the rectangle.
	 */
	public static int getGreenRectangleY(int t, int rectangleLength, int rectangleHeight, int displaySize) {
		int centre = displaySize / 2;
		return centre - rectangleHeight / 2;
	}

	private SimpleGFXNatives() {
		// Prevent instantiation.
	}
}
