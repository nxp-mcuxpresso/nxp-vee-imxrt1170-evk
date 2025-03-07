/*
 * C
 *
 * Copyright 2023-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/* Includes ------------------------------------------------------------------*/

#include "sni.h"
#include "LLUI_LED_impl.h"

/* Defines -------------------------------------------------------------------*/

#define LED_IS_ON 1
#define LED_IS_OFF 0

/* Public functions ----------------------------------------------------------*/

/**
 * @brief Initializes the LEDs.
 *
 * @return the number of LEDs in the group. First LED will have the identifier 0
 * and last one the identifier nbLeds - 1.
 */
int32_t LLUI_LED_IMPL_initialize(void)
{
    return 0;
}

/**
 * @brief Get the intensity of the LED. The implementation should be able to return
 * the state of an LED. If it is not able to do so (for example `GPIO` is not accessible
 * in read mode), the BSP has to save the LED state in a global variable.
 *
 * @param[in] ledID the LED identifier.
 *
 * @return the intensity of the LED.
 */
int32_t LLUI_LED_IMPL_getIntensity(int32_t ledID)
{
    (void)SNI_throwNativeException(-1, "Not implemented");

	return 0;
}

/**
 * @brief Set the intensity of the LED.
 *
 * A 0 intensity turns off the LED, intensity of 255 turns on the LED to max power.
 *
 * @param[in] ledID the LED identifier.
 * @param[in] intensity the intensity of the LED.
 */
void LLUI_LED_IMPL_setIntensity(int32_t ledID, int32_t intensity)
{
    (void)SNI_throwNativeException(-1, "Not implemented");
}
