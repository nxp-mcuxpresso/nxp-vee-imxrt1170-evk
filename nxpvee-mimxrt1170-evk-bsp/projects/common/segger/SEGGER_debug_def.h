/*
 * Copyright 2022 NXP
 * Copyright 2023 MicroEJ Corp. This file has been modified by MicroEJ Corp.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef ENABLE_SYSTEM_VIEW
#include "SEGGER_SYSVIEW.h"
#endif

#define MARKER_SEEKER 0xdead0000
#define MARKER_SPOT   0xdead0001
#define MARKER_MP3    0xdead0002
#define MARKER_OGG    0xdead0003
#define MARKER_DISPLAY 0xdead0004
#define MARKER_DMA    0xdead0005
#define MARKER_SD    0xdead0006
#define MARKER_SD_SYNC    0xdead0007
