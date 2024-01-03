/*
 * C
 *
 * Copyright 2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/* Includes ------------------------------------------------------------------*/

#include <string.h>
#include "fsl_silicon_id.h"
#include "sni.h"
#include "LLDEVICE_impl.h"

/* Defines -------------------------------------------------------------------*/

#define DEVICE_ARCHITECTURE_NAME "NXP-IMX-RT"

/* Public functions ----------------------------------------------------------*/

/**
 * Fills the given buffer with the name of the architecture. The buffer is filled with a null-terminated string.
 * @param buffer the buffer to fill with the name of the architecture.
 * @param length the size of the buffer.
 * @return 1 on success or 0 on error.
 */
uint8_t LLDEVICE_IMPL_getArchitecture(uint8_t* buffer, int32_t length) {
	uint8_t ret = 0;

	if (length > strlen((DEVICE_ARCHITECTURE_NAME))) {
		(void)strncpy((char *)buffer, (DEVICE_ARCHITECTURE_NAME), strlen((DEVICE_ARCHITECTURE_NAME)) + 1);
		ret = 1;
	}

	return ret;
}

/**
 * Fills the given buffer with the ID of the device.
 * @param buffer the buffer to fill with the ID.
 * @param length the size of the buffer.
 * @return the number of bytes filled or 0 on error.
 */
uint32_t LLDEVICE_IMPL_getId(uint8_t* buffer, int32_t length) {
	uint8_t id[SILICONID_MAX_LENGTH];
	uint32_t id_length = 0;
	status_t status;

	if (length >= (SILICONID_MAX_LENGTH)) {
		id_length = (SILICONID_MAX_LENGTH);
		status = SILICONID_GetID(id, &id_length);
		if (kStatus_Success == status) {
			(void)memcpy(buffer, id, id_length);
		} else {
			id_length = 0;
		}
	}

	return id_length;
}

/**
 * Reboots the device.
 */
void LLDEVICE_IMPL_reboot(void)
{
	(void)SNI_throwNativeException(-1, "Not implemented");
}
