/*
 * Copyright 2020 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */
package com.microej.partial.support;

/**
 * Provides the natives related to partial buffer support.
 */
public class PartialBufferNatives {

	private PartialBufferNatives() {
		// private constructor
	}

	/**
	 * Returns the height of the partial buffer allocated by the platform.
	 *
	 * @return the height of the partial buffer (in pixels).
	 */
	public static native int getBufferHeight();

	/**
	 * Sets the bounds of the rectangle area that should be flushed during the following
	 * {@link ej.microui.display.Display#flush()} calls.
	 * <p>
	 * The x and y offsets indicate at which position the back buffer should be flushed within the frame buffer.
	 * <p>
	 * The platform driver implementation of <code>LLUI_DISPLAY_IMPL_flush</code> function has to add an offset (managed
	 * by the policy) to <code>xmin/ymin/xmax/ymax</code> for the <b>destination buffer</b> (frame buffer). The source
	 * buffer bounds stay cropped to <code>xmin/ymin/xmax/ymax</code>.
	 *
	 * @param xOffset
	 *            the x offset of the area.
	 * @param yOffset
	 *            the y offset of the area.
	 * @param width
	 *            the width of the area.
	 * @param height
	 *            the height of the area.
	 */
	public static native void setFlushLimits(int xOffset, int yOffset, int width, int height);
}
