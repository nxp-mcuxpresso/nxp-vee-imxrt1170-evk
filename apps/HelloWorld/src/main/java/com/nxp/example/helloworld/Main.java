/*
 * Copyright 2024 NXP
 * Copyright 2025 MicroEJ Corp. This file has been modified by MicroEJ Corp.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
package com.nxp.example.helloworld;
import ej.util.Device;

import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Main class of the Hello World application.
 */
public class Main {

	private static final Logger LOGGER = Logger.getLogger("[Main]");
	private static final char[] HEX_CHAR_ARRAY = "0123456789ABCDEF".toCharArray();

	/**
	 * Simple main.
	 *
	 * @param args
	 *            command line arguments.
	 */
	public static void main(String[] args) {
		String id = bytesToHexString(Device.getId());
		String architecture = Device.getArchitecture();

		boolean running = true;
		while (running) {
			LOGGER.info("Hello World!");
			LOGGER.info("NXP Platform Accelerator VM running on " + architecture +
					" device with ID 0x" + id);

			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				LOGGER.log(Level.SEVERE, "Thread interrupted", e);
				running = false;
				Thread.currentThread().interrupt();
			}
		}
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
