/*
 * Copyright 2022 NXP
 * Copyright 2023 MicroEJ Corp. This file has been modified by MicroEJ Corp.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
package com.nxp.animatedMascot;

import java.util.ArrayList;

import ej.bon.Util;
import ej.microui.MicroUI;
import ej.microui.display.Colors;
import ej.microui.display.Display;
import ej.microui.display.GraphicsContext;
import ej.microui.display.Image;
import ej.microui.display.Painter;
import ej.microui.display.ResourceImage;
import ej.microui.event.EventGenerator;
import ej.microui.event.generator.Pointer;
import ej.microvg.Matrix;
import ej.microvg.VectorGraphicsPainter;
import ej.microvg.VectorImage;
import ej.mwt.animation.Animation;
import ej.mwt.animation.Animator;

/**
 * AnimatedMAscot application starts an animation of the MicroEJ mascot and NXP logo. When the user touch the screen, a
 * touch animation is launched.
 */
public class AnimatedMascot {
	public static AnimatedMascot demo;

	private long startTime;

	// List of the touch animations that are running.
	private final ArrayList<TouchAnimation> listTouchAnimations = new ArrayList<TouchAnimation>();
	final VectorImage touchImage = VectorImage.getImage("/images/touchAnimation.xml"); //$NON-NLS-1$

	/**
	 * Simple main.
	 *
	 * @param args
	 *            command line arguments.
	 */
	public static void main(String[] args) {
		// Initialize the demo components.
		MicroUI.start();
		demo = new AnimatedMascot();
		demo.initializeEventHandler();
		demo.start();
	}

	private void initializeEventHandler() {
		EventHandlerImpl eventHandler = new EventHandlerImpl();

		// Get Pointer event generators.
		EventGenerator[] generators = EventGenerator.get(Pointer.class);
		// set the event handler for each EventGenerator
		for (EventGenerator gen : generators) {
			gen.setEventHandler(eventHandler);
		}
	}

	private void start() {
		final Display display = Display.getDisplay();
		final GraphicsContext g = display.getGraphicsContext();

		final Matrix matrix = new Matrix();

		final VectorImage mascot = VectorImage.getImage("/images/mascot.xml"); //$NON-NLS-1$
		final Image imageHalo = ResourceImage.loadImage("/images/halo_green.png"); //$NON-NLS-1$

		// Prepare matrix to scale the mascot to the display size
		matrix.setScale(display.getWidth() / mascot.getWidth(), display.getHeight() / mascot.getHeight());

		Animator animator = new Animator();
		this.startTime = Util.platformTimeMillis();
		animator.startAnimation(new Animation() {

			@Override
			public boolean tick(long currentTimeMillis) {
				// Clear screen between each frame
				g.setColor(Colors.WHITE);
				Painter.fillRectangle(g, 0, 0, display.getWidth(), display.getHeight());

				// Calculate current elapsed time
				int elapsed = (int) (currentTimeMillis - demo.startTime);

				// Draw the imageHalo
				Painter.drawImage(g, imageHalo, 0, 844);

				// Draw the mascot
				VectorGraphicsPainter.drawAnimatedImage(g, mascot, matrix, elapsed);

				// Check the status of the running TouchAnimations
				for (int i = 0; i < demo.listTouchAnimations.size(); i++) {
					int elapsedTouch = (int) (currentTimeMillis - demo.listTouchAnimations.get(i).getStartTime());
					// If the touchAnimation is over, remove it
					if (demo.touchImage.getDuration() < elapsedTouch) {
						demo.listTouchAnimations.remove(i);
					} else {
						// Draw the TouchAnimation
						VectorGraphicsPainter.drawAnimatedImage(g, demo.touchImage,
								demo.listTouchAnimations.get(i).getMatrix(), elapsedTouch);
					}
				}

				// Flush the display
				display.flush();

				// If the mascot animation is over, re-launch it.
				if (mascot.getDuration() < (elapsed)) {
					demo.startTime = Util.platformTimeMillis();
				}
				return true;
			}
		});
	}

	/**
	 * Add a new ToucAnimation at the (x,y) coordinates.
	 *
	 * @param x
	 *            the x coordinate of the animation.
	 * @param y
	 *            the y coordinate of the animation.
	 */
	public void addNewTouchAnimation(int x, int y) {
		this.listTouchAnimations.add(new TouchAnimation(x, y));
	}

}
