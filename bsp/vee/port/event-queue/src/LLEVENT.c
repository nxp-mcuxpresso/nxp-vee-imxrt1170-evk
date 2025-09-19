/*
 * C
 *
 * Copyright 2023-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#include <LLEVENT.h>
#include <LLEVENT_impl.h>

// -----------------------------------------------------------------------------
// Macros and Defines
// -----------------------------------------------------------------------------

#define DATA_LENGTH_MASK        0xFFFFFF
#define MAX_TYPE_ID             128

// -----------------------------------------------------------------------------
// Public function definition
// -----------------------------------------------------------------------------

int32_t LLEVENT_offerEvent(int32_t type, int32_t data) {
	// Check the validity of the arguments.
	bool check_parameters = (type >= (int32_t)0) && (type < (int32_t)MAX_TYPE_ID) &&
	                        ((data & (int32_t)DATA_LENGTH_MASK) == data);

	bool event_sent = false;

	if (check_parameters) {
		// Try to offer the event.
		event_sent = LLEVENT_IMPL_offer_event(type, data);
	}

	// Set the status to return.
	int32_t status;
	if (!check_parameters) {
		status = ERR_WRONG_ARGS;
	} else if (!event_sent) {
		status = ERR_FIFO_FULL;
	} else {
		status = NO_ERR;
	}

	return status;
}

int32_t LLEVENT_offerExtendedEvent(int32_t type, void *data, int32_t data_length) {
	//Check the validity of the arguments.
	bool check_parameters = (type >= (int32_t)0) && (type < (int32_t)MAX_TYPE_ID) &&
	                        ((data_length & (int32_t)DATA_LENGTH_MASK) == data_length);

	bool event_sent = false;

	if (check_parameters) {
		// Try to offer the extended event.
		event_sent = LLEVENT_IMPL_offer_extended_event(type, data, data_length);
	}

	// Set the status to return.
	int32_t status;
	if (!check_parameters) {
		status = ERR_WRONG_ARGS;
	} else if (!event_sent) {
		status = ERR_FIFO_FULL;
	} else {
		status = NO_ERR;
	}

	return status;
}
