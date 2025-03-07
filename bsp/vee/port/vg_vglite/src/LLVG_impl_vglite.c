/*
 * C
 *
 * Copyright 2022-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief MicroVG library low level API over VGLite.
 * @author MicroEJ Developer Team
 * @version 8.0.1
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <math.h>
#include <string.h>

#include <LLVG_impl.h>
#include <LLVG_GRADIENT_impl.h>
#include <LLVG_MATRIX_impl.h>

#include "vg_helper.h"
#include "vg_vglite_helper.h"
#include "vg_lite.h"
#include "ui_color.h"
#include "bsp_util.h"
#include "ui_vglite.h"
#include "vg_bvi_vglite.h"

// -----------------------------------------------------------------------------
// Defines
// -----------------------------------------------------------------------------

#if defined(VG_FEATURE_GRADIENT) && defined(VG_FEATURE_GRADIENT_FULL) && \
	(VG_FEATURE_GRADIENT != VG_FEATURE_GRADIENT_FULL)
#error "This implementation is only compatible with VG_FEATURE_GRADIENT_FULL"
#endif

// -----------------------------------------------------------------------------
// LLVG_impl.h functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
void LLVG_IMPL_initialize(void) {
	UI_VGLITE_initialize();
	VG_HELPER_initialize();
	VG_BVI_VGLITE_initialize();
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------
