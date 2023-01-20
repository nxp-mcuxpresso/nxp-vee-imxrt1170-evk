/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef NXPVEE_FXOS_H
#define NXPVEE_FXOS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include "fsl_common.h"

#include "fsl_fxos.h"
#include "fsl_debug_console.h"

#include "queue.h"

int nxpvee_fxos_init();
status_t nxpvee_fxos_read(int fxos_handle, float frequency, int16_t *Ax,
    int16_t *Ay, int16_t *Az, int16_t *Mx, int16_t *My, int16_t *Mz);

/* FXOS8700 configuration functions */
status_t nxpvee_fxos_config(float freq);
static status_t nxpvee_fxos_config_registers(uint8_t frequency);
static status_t frequency_value_to_binary(int freq, uint8_t *reg);

#ifdef __cplusplus
}
#endif

#endif
