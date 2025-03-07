/*
 * Copyright 2022 NXP
 * Copyright 2025 MicroEJ Corp. This file has been modified by MicroEJ Corp.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
package com.nxp.example.simplegfx;

/**
 *
 */
public class SimpleGFXNatives {

	/**
	 * Gets the X coordinate of the orange rectangle.
	 *
	 * @param t the animation step.
	 * @param rectangleLength the length of the rectangle.
	 * @param rectangleHeight the height of the rectangle.
	 * @param displaySize the display size.
	 * @return the X coordinate of the rectangle.
	 */
	public static native int getOrangeRectangleX(int t, int rectangleLength, int rectangleHeight, int displaySize);

	/**
	 * Gets the Y coordinate of the orange rectangle.
	 *
	 * @param t the animation step.
	 * @param rectangleLength the length of the rectangle.
	 * @param rectangleHeight the height of the rectangle.
	 * @param displaySize the display size.
	 * @return the Y coordinate of the rectangle.
	 */
	public static native int getOrangeRectangleY(int t, int rectangleLength, int rectangleHeight, int displaySize);

	/**
	 * Gets the X coordinate of the blue rectangle.
	 *
	 * @param t the animation step.
	 * @param rectangleLength the length of the rectangle.
	 * @param rectangleHeight the height of the rectangle.
	 * @param displaySize the display size.
	 * @return the X coordinate of the rectangle.
	 */
	public static native int getBlueRectangleX(int t, int rectangleLength, int rectangleHeight, int displaySize);

	/**
	 * Gets the Y coordinate of the blue rectangle.
	 *
	 * @param t the animation step.
	 * @param rectangleLength the length of the rectangle.
	 * @param rectangleHeight the height of the rectangle.
	 * @param displaySize the display size.
	 * @return the Y coordinate of the rectangle.
	 */
	public static native int getBlueRectangleY(int t, int rectangleLength, int rectangleHeight, int displaySize);

	/**
	 * Gets the X coordinate of the green rectangle.
	 *
	 * @param t the animation step.
	 * @param rectangleLength the length of the rectangle.
	 * @param rectangleHeight the height of the rectangle.
	 * @param displaySize the display size.
	 * @return the X coordinate of the rectangle.
	 */
	public static native int getGreenRectangleX(int t, int rectangleLength, int rectangleHeight, int displaySize);

	/**
	 * Gets the Y coordinate of the green rectangle.
	 *
	 * @param t the animation step.
	 * @param rectangleLength the length of the rectangle.
	 * @param rectangleHeight the height of the rectangle.
	 * @param displaySize the display size.
	 * @return the Y coordinate of the rectangle.
	 */
	public static native int getGreenRectangleY(int t, int rectangleLength, int rectangleHeight, int displaySize);

	private SimpleGFXNatives() {
		// Prevent instantiation.
	}
}
