/*
 * Copyright 2022-2023 NXP
 * Copyright 2025 MicroEJ Corp. This file has been modified by MicroEJ Corp.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
package com.nxp.example.simplegfx;

import ej.microui.MicroUI;
import ej.microui.display.Display;
import ej.mwt.Desktop;
import ej.util.Device;
import ej.widget.container.Canvas;

import java.util.logging.Logger;

/**
 * Main class
 */
public class SimpleGFX {
	private static final Logger LOGGER = Logger.getLogger("[SimpleGFX]");
	private static final char[] HEX_CHAR_ARRAY = "0123456789ABCDEF".toCharArray();

	/**
	 * Simple main.
	 *
	 * @param args
	 *            command line arguments.
	 */
	public static void main(String[] args) {
		LOGGER.info("NXP Platform Accelerator VM running on " + Device.getArchitecture() +
				" device with ID 0x" + bytesToHexString(Device.getId()));
		MicroUI.start();
		Desktop desktop = new Desktop();

		int displayMinimumSize = Math.min(Display.getDisplay().getWidth(), Display.getDisplay().getHeight());

		Canvas container = new Canvas();
		container.addChild(new SimpleGFXWidget(displayMinimumSize), 0, 0, displayMinimumSize, displayMinimumSize);
		desktop.setWidget(container);
		desktop.requestShow();
	}

	private static String bytesToHexString(byte[] bytes) {
		char[] hexChars = new char[bytes.length * 2];
		for (int i = 0; i < bytes.length; i++) {
			int b = bytes[i] & 0xFF;
			hexChars[i * 2] = HEX_CHAR_ARRAY[b >>> 4];
			hexChars[i * 2 + 1] = HEX_CHAR_ARRAY[b & 0x0F];
		}
		return new String(hexChars);
	}
}
