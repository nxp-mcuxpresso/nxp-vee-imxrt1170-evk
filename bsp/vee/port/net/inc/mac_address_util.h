/*
 * C
 *
 * Copyright 2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef __MAC_ADDRESS_UTIL_H
#define __MAC_ADDRESS_UTIL_H

#include <stdint.h>
#include "fsl_common.h"

/** 
 * Workaround for boards having the same MAC address with SILICONID_ConvertToMacAddr.
 * Takes the first 3 bytes of the hash of the whole SILICON ID
 * instead of the first 3 bytes of the SILICON ID that are identical on some boards
 * 
 * @param macAddr The mac address output
 * 
 * @return kStatus_Success on success, kStatus_Fail on error
 */
status_t workaround_SILICONID_ConvertToMacAddr(uint8_t (*macAddr)[6]);

#endif // __MAC_ADDRESS_UTIL_H
