/*
 * Copyright 2023-2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * Copyright 2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "ffconf.h"

#if defined SD_ENABLED
#include "ff.h"
#include "diskio.h"
#include "fsl_sd.h"
#include "sdmmc_config.h"
#include "fsl_debug_console.h"
/*!
 * @brief wait card insert function.
 */

extern sd_card_t g_sd; /* sd card descriptor */

// #define SD_CARD_LOG_DEBUG
#ifdef SD_CARD_LOG_DEBUG
#define SD_CARD_LOG PRINTF("[SD Card] ");PRINTF
#else
#define SD_CARD_LOG(...) ((void)0)
#endif // SD_CARD_LOG_DEBUG
#define SD_CARD_LOG_ERROR PRINTF("[SD Card][ERROR] ");PRINTF

#define SDCARD_TASK_STACK_SIZE    (512)

/* SD card management */
static FATFS fileSystem;
static bool card_ready = false;
static volatile bool card_inserted = false;

bool SDCARD_isCardReady()
{
    return card_ready;
}

bool SDCARD_waitCardReady()
{
    while (SDCARD_isCardReady() == 0)
    {
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
    return true;
}

static void SDCARD_detect_callback(bool is_inserted, void *user_data) {
    (void)user_data;
    card_inserted = is_inserted;
}

void APP_SDCARD_Task(void *param)
{
    const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};

    BOARD_SD_Config(&g_sd, SDCARD_detect_callback, BOARD_SDMMC_SD_HOST_IRQ_PRIORITY, NULL);

    SD_CARD_LOG("start\n");

    /* SD host init function */
    if (SD_HostInit(&g_sd) != kStatus_Success)
    {
        SD_CARD_LOG_ERROR("SD host init failed.\n");
        vTaskDelete(NULL);
    }

    /* Small delay for SD card detection logic to process */
    vTaskDelay(100 / portTICK_PERIOD_MS);

    while (1) {
        if (!card_ready && card_inserted) {

            SD_SetCardPower(&g_sd, false);
            /* power on the card */
            SD_SetCardPower(&g_sd, true);
            if (FR_OK != f_mount(&fileSystem, driverNumberBuffer, 0U))
            {
                SD_CARD_LOG("Mount volume failed.\n");
                continue;
            }

#if (FF_FS_RPATH >= 2U)
            if (FR_OK != f_chdrive((char const *)&driverNumberBuffer[0U]))
            {
                SD_CARD_LOG("Change drive failed.\n");
                continue;
            }
#endif

            SD_CARD_LOG("SD card drive mounted\n");
            card_ready = true;
            vTaskDelete(NULL);
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void START_SDCARD_Task(void *param)
{
    SD_CARD_LOG("create task\n");
    if (xTaskCreate(APP_SDCARD_Task, "SDCard Task", SDCARD_TASK_STACK_SIZE, NULL, configMAX_PRIORITIES - 5, NULL) !=
        pdPASS)
    {
        SD_CARD_LOG("ERROR initializing audio\n");
        SD_CARD_LOG("Failed to create application task\n");
        while (1)
            ;
    }
}
#endif // SD_ENABLED
