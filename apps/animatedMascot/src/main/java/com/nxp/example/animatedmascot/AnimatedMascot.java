/*
 * Copyright 2022 NXP
 * Copyright 2023- 2025 MicroEJ Corp. This file has been modified by MicroEJ Corp.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
package com.nxp.example.animatedmascot;

import java.util.ArrayList;

import ej.annotation.Nullable;
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

	/**
	 * Touch vector graphic animation.
	 */
	public static final VectorImage TOUCH_IMAGE = VectorImage.getImage("/images/touchAnimation.xml");
	@Nullable
	private static AnimatedMascot animatedMascot = null;

	private long startTime;

	// List of the touch animations that are running.
	private final ArrayList<TouchAnimation> listTouchAnimations = new ArrayList<>();

	/**
	 * Simple main.
	 *
	 * @param args
	 *            command line arguments.
	 */
	public static void main(String[] args) {
		// Initialize the demo components.
		MicroUI.start();
		AnimatedMascot demo = AnimatedMascot.getInstance();
		demo.initializeEventHandler();
		demo.start();
	}

	/**
	 * Method to retrieve the singleton instance
	 *
	 * @return an instance of {@link AnimatedMascot}
	 */
	public static AnimatedMascot getInstance() {
		if (AnimatedMascot.animatedMascot == null) {
			AnimatedMascot.animatedMascot = new AnimatedMascot();
		}

		return AnimatedMascot.animatedMascot;
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
				int elapsed = (int) (currentTimeMillis - startTime);

				// Draw the imageHalo
				Painter.drawImage(g, imageHalo, 0, 844);

				// Draw the mascot
				VectorGraphicsPainter.drawAnimatedImage(g, mascot, matrix, elapsed);

				// Check the status of the running TouchAnimations
				int i = 0;
				while(i < listTouchAnimations.size()) {
					int elapsedTouch = (int) (currentTimeMillis - listTouchAnimations.get(i).getStartTime());
					// If the touchAnimation is over, remove it
					if (TOUCH_IMAGE.getDuration() < elapsedTouch) {
						listTouchAnimations.remove(i);
					} else {
						// Draw the TouchAnimation
						VectorGraphicsPainter.drawAnimatedImage(g, TOUCH_IMAGE,
								listTouchAnimations.get(i).getMatrix(), elapsedTouch);
						i++;
					}
				}

				// Flush the display
				display.flush();

				// If the mascot animation is over, re-launch it.
				if (mascot.getDuration() < (elapsed)) {
					startTime = Util.platformTimeMillis();
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
