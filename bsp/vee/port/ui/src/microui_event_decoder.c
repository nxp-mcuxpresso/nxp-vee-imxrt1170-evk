/*
 * C
 *
 * Copyright 2021-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief This MicroUI Events decoder describes the events to the standard output stream.
 *
 * @see LLUI_INPUT_LOG_impl.c file comment
 * @author MicroEJ Developer Team
 * @version 14.2.0
 * @since MicroEJ UI Pack 13.1.0
 */

#include "ui_configuration.h"

#if defined(UI_FEATURE_EVENT_DECODER)

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// calls Microui events decoder functions
#include "microui_event_decoder.h"

// -----------------------------------------------------------------------------
// Macros and Defines
// -----------------------------------------------------------------------------

/*
 * @brief Default traces.
 */
#define DESCRIBE_EVENT_GENERATOR(event) (UI_DEBUG_PRINT(" (event generator %u)", EVENT_GENERATOR_ID(event)))
#define DESCRIBE_EOL() (UI_DEBUG_PRINT("\n"))

/*
 * @brief Decodes MicroUI event.
 */
#define EVENT_DATA(event) ((uint16_t)(event))
#define EVENT_GENERATOR_ID(event) ((uint8_t)((event) >> 16))

/*
 * @brief Decodes MicroUI Command event.
 */
#define COMMAND_GET(event) EVENT_DATA(event)

/*
 * @brief Decodes MicroUI Buttons event.
 */
#define BUTTON_ACTION_PRESSED 0
#define BUTTON_ACTION_RELEASED 1
#define BUTTON_ACTION_LONG 2
#define BUTTON_ACTION_REPEATED 3
#define BUTTON_ACTION_CLICK 4
#define BUTTON_ACTION_DOUBLECLICK 5
#define BUTTON_ACTION(event) ((uint8_t)((event) >> 8))
#define BUTTON_ID(event) ((uint8_t)(event))

/*
 * @brief Decodes MicroUI Pointer event.
 */
#define POINTER_ACTION_MOVE 6
#define POINTER_ACTION_DRAG 7
#define POINTER_X(data) (((data) >> 16) & 0xfff)
#define POINTER_Y(data) ((data) & 0xfff)
#define POINTER_TYPE(data) (((data) >> 31) & 0x1)

/*
 * @brief Decodes MicroUI user event.
 */
#define USEREVENT_SIZE(event) EVENT_DATA(event)

// -----------------------------------------------------------------------------
// Internal functions
// -----------------------------------------------------------------------------

static void decode_event_command(uint32_t event, uint32_t index,
                                 MICROUI_EVENT_DECODER_decode_event_data *fct_data_decoder) {
	(void)index;
	(void)fct_data_decoder;

	UI_DEBUG_PRINT("Command ");

	uint8_t command = (uint8_t)COMMAND_GET(event);

	switch (command) {
	case 0:
		UI_DEBUG_PRINT("ESC");
		break;

	case 1:
		UI_DEBUG_PRINT("BACK");
		break;

	case 2:
		UI_DEBUG_PRINT("UP");
		break;

	case 3:
		UI_DEBUG_PRINT("DOWN");
		break;

	case 4:
		UI_DEBUG_PRINT("LEFT");
		break;

	case 5:
		UI_DEBUG_PRINT("RIGHT");
		break;

	case 6:
		UI_DEBUG_PRINT("SELECT");
		break;

	case 7:
		UI_DEBUG_PRINT("CANCEL");
		break;

	case 8:
		UI_DEBUG_PRINT("HELP");
		break;

	case 9:
		UI_DEBUG_PRINT("MENU");
		break;

	case 10:
		UI_DEBUG_PRINT("EXIT");
		break;

	case 11:
		UI_DEBUG_PRINT("START");
		break;

	case 12:
		UI_DEBUG_PRINT("STOP");
		break;

	case 13:
		UI_DEBUG_PRINT("PAUSE");
		break;

	case 14:
		UI_DEBUG_PRINT("RESUME");
		break;

	case 15:
		UI_DEBUG_PRINT("COPY");
		break;

	case 16:
		UI_DEBUG_PRINT("CUT");
		break;

	case 17:
		UI_DEBUG_PRINT("PASTE");
		break;

	case 18:
		UI_DEBUG_PRINT("CLOCKWISE");
		break;

	case 19:
		UI_DEBUG_PRINT("ANTICLOCKWISE");
		break;

	case 20:
		UI_DEBUG_PRINT("PREVIOUS");
		break;

	case 21:
		UI_DEBUG_PRINT("NEXT");
		break;

	default:
		UI_DEBUG_PRINT("0x%02x", command);
		break;
	}

	DESCRIBE_EVENT_GENERATOR(event);
	DESCRIBE_EOL();
}

static void decode_event_button(uint32_t event, uint32_t index,
                                MICROUI_EVENT_DECODER_decode_event_data *fct_data_decoder) {
	(void)index;
	(void)fct_data_decoder;

	UI_DEBUG_PRINT("Button %u ", BUTTON_ID(event));
	uint8_t action = BUTTON_ACTION(event);

	switch (action) {
	case BUTTON_ACTION_PRESSED:
		UI_DEBUG_PRINT("pressed");
		break;

	case BUTTON_ACTION_RELEASED:
		UI_DEBUG_PRINT("released");
		break;

	case BUTTON_ACTION_LONG:
		UI_DEBUG_PRINT("long");
		break;

	case BUTTON_ACTION_REPEATED:
		UI_DEBUG_PRINT("repeated");
		break;

	case BUTTON_ACTION_CLICK:
		UI_DEBUG_PRINT("click");
		break;

	case BUTTON_ACTION_DOUBLECLICK:
		UI_DEBUG_PRINT("double-click");
		break;

	default:
		UI_DEBUG_PRINT("unknown action: %u", action);
		break;
	}

	DESCRIBE_EVENT_GENERATOR(event);
	DESCRIBE_EOL();
}

#ifdef UI_EVENTDECODER_EVENTGEN_TOUCH
/*
 * @brief Input pointer event holds a data that contains x and y.
 */
static void decode_event_pointer_data(uint32_t event, uint32_t data, uint32_t index) {
	(void)event;

	UI_DEBUG_PRINT("[%02u: 0x%08x]    at %u,%u (", index, data, POINTER_X(data), POINTER_Y(data));
	if (0 == POINTER_TYPE(data)) {
		UI_DEBUG_PRINT("absolute)");
	} else {
		UI_DEBUG_PRINT("relative)");
	}

	DESCRIBE_EOL();
}

#endif // UI_EVENTDECODER_EVENTGEN_TOUCH

static void decode_event_pointer(uint32_t event, uint32_t index,
                                 MICROUI_EVENT_DECODER_decode_event_data *fct_data_decoder) {
	(void)index;
	(void)fct_data_decoder;

	UI_DEBUG_PRINT("Pointer ");
	uint8_t action = BUTTON_ACTION(event);

	switch (action) {
	case BUTTON_ACTION_PRESSED:
		UI_DEBUG_PRINT("pressed");
		break;

	case BUTTON_ACTION_RELEASED:
		UI_DEBUG_PRINT("released");
		break;

	case BUTTON_ACTION_LONG:
		UI_DEBUG_PRINT("long");
		break;

	case BUTTON_ACTION_REPEATED:
		UI_DEBUG_PRINT("repeated");
		break;

	case BUTTON_ACTION_CLICK:
		UI_DEBUG_PRINT("click");
		break;

	case BUTTON_ACTION_DOUBLECLICK:
		UI_DEBUG_PRINT("double-click");
		break;

	case POINTER_ACTION_MOVE:
		UI_DEBUG_PRINT("moved");
		break;

	case POINTER_ACTION_DRAG:
		UI_DEBUG_PRINT("dragged");
		break;

	default:
		UI_DEBUG_PRINT("unknown action: %u", action);
		break;
	}

	DESCRIBE_EVENT_GENERATOR(event);
	DESCRIBE_EOL();
}

static void decode_event_state(uint32_t event, uint32_t index,
                               MICROUI_EVENT_DECODER_decode_event_data *fct_data_decoder) {
	(void)index;
	(void)fct_data_decoder;

	UI_DEBUG_PRINT("TODO %s 0x%08x\n", __FUNCTION__, event);
}

/*
 * @brief Decodes the input events: the events sent by the BSP. To decode the events,
 * the decoder must know the event generators identifier defined during the MicroEJ
 * platform build. These identifiers are available in microui_constants.h.
 *
 * @see ui_configuration.h
 */
static void decode_event_input(uint32_t event, uint32_t index,
                               MICROUI_EVENT_DECODER_decode_event_data *fct_data_decoder) {
	UI_DEBUG_PRINT("Input event: ");

	uint8_t generator_id = EVENT_GENERATOR_ID(event);

	switch (generator_id) {
#ifdef UI_EVENTDECODER_EVENTGEN_COMMAND
	case MICROUI_EVENTGEN_COMMANDS:
		decode_event_command(event, index, fct_data_decoder);
		break;
#endif // UI_EVENTDECODER_EVENTGEN_COMMAND

#ifdef UI_EVENTDECODER_EVENTGEN_BUTTONS
	case UI_EVENTDECODER_EVENTGEN_BUTTONS:
		decode_event_button(event, index, fct_data_decoder);
		break;
#endif // UI_EVENTDECODER_EVENTGEN_BUTTONS

#ifdef UI_EVENTDECODER_EVENTGEN_TOUCH
	case UI_EVENTDECODER_EVENTGEN_TOUCH:
		*fct_data_decoder = decode_event_pointer_data;
		decode_event_pointer(event, index, fct_data_decoder);
		break;
#endif // UI_EVENTDECODER_EVENTGEN_TOUCH

	default:
		UI_DEBUG_PRINT("unknown ");
		DESCRIBE_EVENT_GENERATOR(event);
		DESCRIBE_EOL();
		break;
	}
}

static void decode_event_user_data(uint32_t event, uint32_t data, uint32_t index) {
	(void)event;
	UI_DEBUG_PRINT("    [%02u] 0x%08x\n", index, data);
}

static void decode_event_user(uint32_t event, uint32_t index,
                              MICROUI_EVENT_DECODER_decode_event_data *fct_data_decoder) {
	(void)index;
	UI_DEBUG_PRINT("User input event");

	uint8_t size = (uint8_t)USEREVENT_SIZE(event);
	if (size > (uint32_t)1) {
		UI_DEBUG_PRINT("s (size = %u)", size);
	}
	DESCRIBE_EVENT_GENERATOR(event);
	UI_DEBUG_PRINT(": ");

	*fct_data_decoder = decode_event_user_data;
}

// -----------------------------------------------------------------------------
// API
// -----------------------------------------------------------------------------

void MICROUI_EVENT_DECODER_describe_dump_start(void) {
	UI_DEBUG_PRINT("============================== MicroUI FIFO Dump ===============================\n");
}

void MICROUI_EVENT_DECODER_describe_dump_past(void) {
	UI_DEBUG_PRINT("---------------------------------- Old Events ----------------------------------\n");
}

void MICROUI_EVENT_DECODER_describe_dump_future(void) {
	UI_DEBUG_PRINT("---------------------------------- New Events ----------------------------------\n");
}

void MICROUI_EVENT_DECODER_describe_dump_events_objects(void) {
	UI_DEBUG_PRINT("--------------------------- New Events' Java objects ---------------------------\n");
}

void MICROUI_EVENT_DECODER_describe_dump_end(void) {
	UI_DEBUG_PRINT("================================================================================\n");
}

void MICROUI_EVENT_DECODER_drop_data(uint32_t data, uint32_t index) {
	UI_DEBUG_PRINT("[%02u: 0x%08x] garbage\n", index, data);
}

void MICROUI_EVENT_DECODER_decode_event(uint32_t event, uint32_t index,
                                        MICROUI_EVENT_DECODER_decode_event_data *fct_data_decoder) {
	UI_DEBUG_PRINT("[%02u: 0x%08x] ", index, event);

	uint8_t event_type = (uint8_t)(event >> 24);

	switch (event_type) {
	case 0x00:
		decode_event_command(event, index, fct_data_decoder);
		break;

	case 0x01:
		decode_event_button(event, index, fct_data_decoder);
		break;

	case 0x02:
		decode_event_pointer(event, index, fct_data_decoder);
		break;

	case 0x03:
		decode_event_state(event, index, fct_data_decoder);
		break;

	case 0x04:
		// not used
		break;

	case 0x05:
		UI_DEBUG_PRINT("Call serially (Runnable index = %u)\n", EVENT_DATA(event));
		break;

	case 0x06:
		UI_DEBUG_PRINT("MicroUI STOP\n");
		break;

	case 0x07:
		decode_event_input(event, index, fct_data_decoder);
		break;

	case 0x08:
		UI_DEBUG_PRINT("Display SHOW Displayable (Displayable index = %u)\n", EVENT_DATA(event));
		break;

	case 0x09:
		UI_DEBUG_PRINT("Display HIDE Displayable (Displayable index = %u)\n", EVENT_DATA(event));
		break;

	case 0x0a:
		// not used
		break;

	case 0x0b:
		UI_DEBUG_PRINT("Display FLUSH\n");
		break;

	case 0x0c:
		UI_DEBUG_PRINT("Display FORCE FLUSH\n");
		break;

	case 0x0d:
		UI_DEBUG_PRINT("Display REPAINT Displayable (Displayable index = %u)\n", EVENT_DATA(event));
		break;

	case 0x0e:
		UI_DEBUG_PRINT("Display REPAINT Current Displayable\n");
		break;

	case 0x0f:
		UI_DEBUG_PRINT("Display KF SWITCH Display\n");
		break;

	default:
		decode_event_user(event, index, fct_data_decoder);
		break;
	}
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#endif // UI_FEATURE_EVENT_DECODER
