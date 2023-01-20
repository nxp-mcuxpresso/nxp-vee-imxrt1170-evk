/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SDCARD_HELPER_H
#define SDCARD_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif
  void START_SDCARD_Task(void *param);
  void SDCARD_write_file();
  bool SDCARD_isCardReady();
#ifdef __cplusplus
}
#endif

#endif
