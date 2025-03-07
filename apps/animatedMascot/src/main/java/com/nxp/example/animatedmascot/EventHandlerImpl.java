/*
 * Copyright 2023-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */
package com.nxp.example.animatedmascot;

import ej.microui.event.Event;
import ej.microui.event.EventHandler;
import ej.microui.event.generator.Buttons;
import ej.microui.event.generator.Pointer;

/**
 * EventHandlerImpl handles the touch events to create new touch animations.
 */
public class EventHandlerImpl implements EventHandler {

	@Override
	public boolean handleEvent(int event) {
		int type = Event.getType(event);

		if (type == Pointer.EVENT_TYPE) {
			int action = Buttons.getAction(event);
			if (action == Buttons.PRESSED) {
				Pointer pointer = (Pointer) Event.getGenerator(event);

				int x = pointer.getX();
				int y = pointer.getY();

				AnimatedMascot.getInstance().addNewTouchAnimation(x, y);
			}
		}

		return true;
	}

}
