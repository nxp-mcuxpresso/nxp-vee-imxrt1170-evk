/*
 * C
 *
 * Copyright 2023-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */
#ifndef TEST_C_NATIVES
#define TEST_C_NATIVES

#include <stdint.h>

/**
 * Offer an event to the EventQueue throw the C API.
 *
 * @param type: the type of the event.
 * @param data: the data of the event.
 * @return 0 -> success, -1 -> failed: illegal arguments, -2 -> failed: the FIFO is full
 */
int32_t Java_ej_event_utils_TestCNatives_offerEventCApi(uint32_t type, int32_t data);


/**
* Offer an event to the EventQueue throw the C API.
*
* @param type
*            the type of the event.
* @param data
*            the data of the event.
* @param data_length
*            the length of the data.
* @return 0 -> success, -1 -> failed: illegal arguments, -2 -> failed: the FIFO is full
*/
int32_t Java_ej_event_utils_TestCNatives_offerExtendedEventCApi(int32_t type, int8_t* data, uint32_t data_length);

#endif // TEST_C_NATIVES
