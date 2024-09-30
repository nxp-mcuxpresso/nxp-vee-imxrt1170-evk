/*
 * Copyright 2023-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef UI_DISPLAY_BRS_CONFIGURATION_H
#define UI_DISPLAY_BRS_CONFIGURATION_H
#ifdef __cplusplus
extern "C" {
#endif

/*
 * @file
 * @brief Provides some implementations of the LLUI_DISPLAY_impl.h functions relating
 * to the display buffer strategy (BRS).
 *
 * Several BRS are available. This header file allows to select an existing BRS or to
 * let the BSP implements its own strategy. Note that if no strategy is used, the default
 * Graphics Engine's strategy is used that consists to only called LLUI_DISPLAY_IMPL_flush()
 * (always full screen, no restore).
 *
 * @author MicroEJ Developer Team
 * @version 4.1.0
 */

#include "display_configuration.h"

/*
 * @brief Compatibility sanity check value.
 * This define value is checked in the implementation to validate that the version of this configuration
 * is compatible with the implementation.
 *
 * This value must not be changed by the user of the CCO.
 * This value must be incremented by the implementor of the CCO when a configuration define is added, deleted or
 * modified.
 */
#define UI_DISPLAY_BRS_CONFIGURATION_VERSION (1)

// -----------------------------------------------------------------------------
// Display Buffer Refresh Strategies (BRS)
// -----------------------------------------------------------------------------

/*
 * @brief Defines the "legacy" strategy (equivalent to the one available in UI Packs 13.x)
 * dedicated to the buffer policy SWAP (see MicroEJ documentation).
 */
#define UI_DISPLAY_BRS_LEGACY (0u)

/*
 * @brief Defines the "single" strategy dedicated to the display with one buffer on the MCU side
 * and one buffer on the display side (the front buffer is not mapped to the MCU address
 * space). The refresh consists in transmitting the back buffer data to the front buffer. The
 * content to transmit is a list of rectangles that have been modified since last flush or an
 * unique rectangle that encapsulates all regions (see the option UI_DISPLAY_BRS_FLUSH_SINGLE_RECTANGLE).
 *
 * Note: the maximum number of rectangles given as parameter to the function
 * LLUI_DISPLAY_IMPL_flush() is equal to UI_RECT_COLLECTION_MAX_LENGTH.
 */
#define UI_DISPLAY_BRS_SINGLE (1u)

/*
 * @brief Defines the "predraw" strategy that consists in restoring the back buffer with the
 * content of the previous drawings (the past) just before the very first drawing after a
 * flush (and not just after the flush).
 *
 * This strategy requires the available number of back buffers (i.e. the number of buffers
 * where the drawings can be performed):
 *
 * 		#define UI_DISPLAY_BRS_DRAWING_BUFFER_COUNT (2u)
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
 * - This BRS takes in consideration the option UI_DISPLAY_BRS_FLUSH_SINGLE_RECTANGLE.
 */
#define UI_DISPLAY_BRS_PREDRAW (2u)

// -----------------------------------------------------------------------------
// Display BRS Implementation and Options
// -----------------------------------------------------------------------------

/*
 * @brief Defines the display buffer strategy (BRS) to use: one of the strategies above,
 * the Graphics Engine's default flush strategy or a BSP's custom flush strategy.
 *
 * When not set, the BSP has to implement the LLUI_DISPLAY_impl.h's API to define its custom
 * display buffer strategy (otherwise the basic Graphics Engine's strategy will be used, see
 * file comment).
 */
#ifndef UI_DISPLAY_BRS
#define UI_DISPLAY_BRS (UI_DISPLAY_BRS_PREDRAW)
#endif

/*
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
#ifndef UI_DISPLAY_BRS_DRAWING_BUFFER_COUNT
#define UI_DISPLAY_BRS_DRAWING_BUFFER_COUNT (FRAME_BUFFER_COUNT)
#endif

/*
 * @brief Defines the number of rectangles the strategy uses when calling LLUI_DISPLAY_IMPL_flush().
 * When set, the strategy gives only one rectangle that includes all dirty regions (a rectangle that
 * encapsulates all rectangles). When not set, the strategy gives all rectangles.
 *
 * When the implementation of LLUI_DISPLAY_IMPL_flush() only consists to swap the back buffers, the
 * rectangles list is useless.
 *
 * When the implementation of LLUI_DISPLAY_IMPL_flush() consists to transmit an unique portion of the
 * back buffer (for instance: the back buffer is transmitted to the LCD through a DSI bus), the single
 * rectangle mode is useful.
 *
 * When the implementation of LLUI_DISPLAY_IMPL_flush() consists to transmit several portions of the back
 * buffer (for instance: the back buffer is transmitted to the LCD through a SPI bus), the rectangle list
 * is useful.
 *
 * By default, the rectangles list is given as-is (the option is not enabled).
 * @see the strategies comments to have more information on the use of this option.
 */
//#define UI_DISPLAY_BRS_FLUSH_SINGLE_RECTANGLE

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
#endif // UI_DISPLAY_BRS_CONFIGURATION_H
