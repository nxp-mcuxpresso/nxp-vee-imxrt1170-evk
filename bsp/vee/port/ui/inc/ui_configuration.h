/*
 * Copyright 2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef UI_CONFIGURATION_H
#define UI_CONFIGURATION_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * @brief MicroEJ MicroUI library low level API: enable some features according to
 * the hardware capabilities.
 * @author MicroEJ Developer Team
 */

#include "display_configuration.h"
#include "fsl_debug_console.h"

/**
 * @brief Compatibility sanity check value.
 * This define value is checked in the implementation to validate that the version of this configuration
 * is compatible with the implementation.
 *
 * This value must not be changed by the user of this C module.
 * This value must be incremented by the implementor of this C module when a configuration define is added, deleted or
 * modified.
 */
#define UI_CONFIGURATION_VERSION (1)

// -----------------------------------------------------------------------------
// MicroUI's Allocator Options
// -----------------------------------------------------------------------------

/**
 * @brief Value of "UI_FEATURE_ALLOCATOR" to use the allocator of the file LLUI_DISPLAY_HEAP_impl.c. This implementation
 * uses a best fit allocator and features some additional APIs (see microui_heap.h) to analyse the allocation in the
 * MicroUI image heap.
 */
#define UI_FEATURE_ALLOCATOR_BESTFIT (1u)

// -----------------------------------------------------------------------------
// Display Buffer Refresh Strategy Options
// -----------------------------------------------------------------------------

/**
 * @brief Value of "UI_FEATURE_BRS". Defines the "legacy" strategy (equivalent to the one available in UI Packs 13.x)
 * dedicated to the buffer policy SWAP (see MicroEJ documentation).
 */
#define UI_FEATURE_BRS_LEGACY (0u)

/**
 * @brief Value of "UI_FEATURE_BRS". Defines the "single" strategy dedicated to the display with one buffer on the MCU
 * side and one buffer on the display side (the front buffer is not mapped to the MCU address
 * space). The refresh consists in transmitting the back buffer data to the front buffer. The
 * content to transmit is a list of rectangles that have been modified since last flush or an
 * unique rectangle that encapsulates all regions (see the option UI_FEATURE_BRS_FLUSH_SINGLE_RECTANGLE).
 *
 * Note: the maximum number of rectangles given as parameter to the function
 * LLUI_DISPLAY_IMPL_flush() is equal to UI_RECT_COLLECTION_MAX_LENGTH.
 */
#define UI_FEATURE_BRS_SINGLE (1u)

/**
 * @brief Value of "UI_FEATURE_BRS". Defines the "predraw" strategy that consists in restoring the back buffer with the
 * content of the previous drawings (the past) just before the very first drawing after a
 * flush (and not just after the flush).
 *
 * This strategy requires the available number of back buffers (i.e. the number of buffers
 * where the drawings can be performed) to be defined:
 *
 * 		#define UI_FEATURE_BRS_DRAWING_BUFFER_COUNT (2u)
 *
 * This strategy manages, by default, two or three back buffers. Its implementation can be
 * easily modified to manage more back buffers. An error is thrown if the code is not modified
 * and the number of back buffers is higher than three.
 *
 * A warning is thrown if there is only one back buffer because this strategy is optimized
 * for two or more back buffers. However, this strategy works with only one buffer but it
 * is a little bit too complex for this use case.
 *
 * Notes:
 * - The maximum number of rectangles given as parameter to the function LLUI_DISPLAY_IMPL_flush()
 * is equal to UI_RECT_COLLECTION_MAX_LENGTH.
 * - This BRS takes in consideration the option UI_FEATURE_BRS_FLUSH_SINGLE_RECTANGLE.
 */
#define UI_FEATURE_BRS_PREDRAW (2u)

// -----------------------------------------------------------------------------
// MicroUI's Features Implementation
// -----------------------------------------------------------------------------

/**
 * @brief Standard "printf" indirection.
 */
#ifndef UI_DEBUG_PRINT
#define UI_DEBUG_PRINT (void)PRINTF
#endif

/*
 * @brief Defines the number of rectangles that collections can contain.
 */
#ifndef UI_RECT_COLLECTION_MAX_LENGTH
#define UI_RECT_COLLECTION_MAX_LENGTH (8u)
#endif

/**
 * @brief Uncomment this define to use the allocator "BESTFIT".
 *
 * When unset, the Graphics Engine uses its internal allocator (a best fit allocator that does not provide any functions
 * to analyse its content).
 *
 * The default graphics engine's allocator can be replaced by a 3rd-party allocator by implementing the functions
 * LLUI_DISPLAY_IMPL_imageHeap*().
 */
//#define UI_FEATURE_ALLOCATOR UI_FEATURE_ALLOCATOR_BESTFIT

/**
 * @brief When defined, the logger is enabled. The call to LLUI_INPUT_dump()
 * has no effect when the logger is disabled.
 *
 * By default the logger is not enabled.
 */
#define UI_FEATURE_EVENT_DECODER

#if defined(UI_FEATURE_EVENT_DECODER)

// header file created by MicroEJ Platform builder.
#include "microui_constants.h"

/**
 * @brief When defined, the MicroUI event decoder is able to decode the *input*
 * "Command" events. The define's value is the MicroUI Event Generator
 * "Command" fixed in the microui.xml file and used to build the MicroEJ Platform.
 * Most of time the MicroUI Event Generator "Command" is "MICROUI_EVENTGEN_COMMANDS":
 *
 *   #define UI_EVENTDECODER_EVENTGEN_COMMAND MICROUI_EVENTGEN_COMMANDS
 *
 * When not defined, the MicroUI event decoder does not try to decode the MicroUI
 * events "Command".
 */
#ifndef UI_EVENTDECODER_EVENTGEN_COMMAND
#define UI_EVENTDECODER_EVENTGEN_COMMAND MICROUI_EVENTGEN_COMMANDS
#endif

/**
 * @brief When defined, the MicroUI event decoder is able to decode the *input*
 * "Buttons" events. The define's value is the MicroUI Event Generator
 * "Buttons" fixed in the microui.xml file and used to build the MicroEJ Platform.
 * Most of time the MicroUI Event Generator "Buttons" is "MICROUI_EVENTGEN_BUTTONS":
 *
 *   #define UI_EVENTDECODER_EVENTGEN_BUTTONS MICROUI_EVENTGEN_BUTTONS
 *
 * When not defined, the MicroUI event decoder does not try to decode the MicroUI
 * events "Buttons".
 */
#ifndef UI_EVENTDECODER_EVENTGEN_BUTTONS
#define UI_EVENTDECODER_EVENTGEN_BUTTONS MICROUI_EVENTGEN_BUTTONS
#endif

/**
 * @brief When defined, the MicroUI event decoder is able to decode the *input*
 * "Touch" events. The define's value is the MicroUI Event Generator
 * "Touch" fixed in the microui.xml file and used to build the MicroEJ Platform.
 * Most of time the MicroUI Event Generator "Touch" is "MICROUI_EVENTGEN_TOUCH":
 *
 *   #define UI_EVENTDECODER_EVENTGEN_TOUCH MICROUI_EVENTGEN_TOUCH
 *
 * When not defined, the MicroUI event decoder does not try to decode the MicroUI
 * events "Touch".
 */
#ifndef UI_EVENTDECODER_EVENTGEN_TOUCH
#define UI_EVENTDECODER_EVENTGEN_TOUCH MICROUI_EVENTGEN_TOUCH
#endif

#endif // UI_FEATURE_EVENT_DECODER

/**
 * @brief Defines the display buffer refresh strategy (BRS) to use: one of the strategies above, the Graphics Engine's
 * default refresh strategy or a BSP's custom refresh strategy.
 *
 * When not set, the BSP has to implement the LLUI_DISPLAY_impl.h's API to define its custom
 * display buffer refresh strategy. If no strategy is set, the default Graphics Engine's strategy is
 * used that consists to only called LLUI_DISPLAY_IMPL_flush()(always full screen, no restore).
 */
#ifndef UI_FEATURE_BRS
#define UI_FEATURE_BRS (UI_FEATURE_BRS_PREDRAW)
#endif

/**
 * @brief Defines the available number of drawing buffers; in other words, the number of buffers the
 * Graphics Engine can use to draw into. According to the display BRS and the LCD connection, a drawing
 * buffer can be alternatively a back buffer (the buffer currently used by the Graphics Engine), a
 * front buffer (the buffer currently used by the LCD driver to map the LCD device), a transmission
 * buffer (the buffer currently used by the LCD driver to transmit the data to the LCD device) or a free
 * buffer (a buffer currently unused).
 *
 * Warning: This counter only defines the buffers the Graphics Engine can use and not the buffer reserved
 * to the LCD device (mapped or or not on the MCU address space).
 */
#ifndef UI_FEATURE_BRS_DRAWING_BUFFER_COUNT
#define UI_FEATURE_BRS_DRAWING_BUFFER_COUNT (FRAME_BUFFER_COUNT)
#endif

/**
 * @brief Defines the number of rectangles the strategy uses when calling LLUI_DISPLAY_IMPL_flush().
 * When set, the strategy sends only one rectangle that includes all dirty regions (a rectangle that
 * encapsulates all rectangles). When not set, the strategy sends all rectangles.
 *
 * When the implementation of LLUI_DISPLAY_IMPL_flush() only consists in swapping the back buffers, the
 * rectangles list is useless.
 *
 * When the implementation of LLUI_DISPLAY_IMPL_flush() consists in transmitting an unique portion of the
 * back buffer (for instance: the back buffer is transmitted to the LCD through a DSI bus), the single
 * rectangle mode is useful.
 *
 * When the implementation of LLUI_DISPLAY_IMPL_flush() consists in transmitting several portions of the back
 * buffer (for instance: the back buffer is transmitted to the LCD through a SPI bus), the rectangle list
 * is useful.
 *
 * By default, the rectangle list is given as-is (the option is not enabled).
 * @see the strategies' comments to have more information on the use of this option.
 */
//#define UI_FEATURE_BRS_FLUSH_SINGLE_RECTANGLE

/**
 * @brief Defines the number of supported destination formats. When not set or smaller than
 * "2", the file ui_drawing.c considers only one destination format is available: the same format as
 * the buffer of the display.
 * @see ui_drawing.c for more information.
 */
#ifndef UI_GC_SUPPORTED_FORMATS
#define UI_GC_SUPPORTED_FORMATS (2u)
#endif

/**
 * @brief When defined, in addition to the standard image formats (ARGB8888, A8, etc), the VEE Port can
 * support one or several custom formats.
 * @see ui_image_drawing.c for more information.
 */
//#define UI_FEATURE_IMAGE_CUSTOM_FORMATS

/**
 * @brief When defined, in addition to the graphics engine's internal font format, the VEE Port can
 * support one or several custom formats.
 * @see ui_font_drawing.c for more information.
 */
//#define UI_FEATURE_FONT_CUSTOM_FORMATS

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // UI_CONFIGURATION_H
