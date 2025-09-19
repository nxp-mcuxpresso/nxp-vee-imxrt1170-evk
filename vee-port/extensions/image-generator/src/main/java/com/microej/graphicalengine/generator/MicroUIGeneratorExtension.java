/*
 * Java
 *
 * Copyright 2020-2022 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.

 */
package com.microej.graphicalengine.generator;

import com.microej.tool.ui.generator.BufferedImageLoader;

public class MicroUIGeneratorExtension extends BufferedImageLoader{

	/**
	 * VGLite library requires a stride on 16 pixels: 64 bytes for 32bpp, 32 bytes
	 * for 16bpp and 16 bytes for 8bpp. Exception 8 bytes for <= 4bpp.
	 */
	private static final int ALIGNMENT_PIXELS = 16;

	@Override
	public int getStride(int defaultStride) {
		return (getWidth() + ALIGNMENT_PIXELS - 1) & ~(ALIGNMENT_PIXELS - 1);
	}
}
