/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "ffconf.h"

#if defined DEMO_SDCARD
#include "ff.h"
#include "diskio.h"
#include "fsl_sd.h"
#include "sdmmc_config.h"
#include "fsl_debug_console.h"
/*!
 * @brief wait card insert function.
 */

#ifdef SD_ENABLED
extern sd_card_t g_sd; /* sd card descriptor */

/* static values for fatfs */
AT_NONCACHEABLE_SECTION(FATFS g_fileSystem); /* File system object */
AT_NONCACHEABLE_SECTION(FIL g_fileObject);   /* File object */
AT_NONCACHEABLE_SECTION(BYTE work[FF_MAX_SS]);


#define DEMO_RECORD_PATH                                           \
    {                                                              \
        SDDISK + '0', ':', '/', 'r', 'e', 'c', 'o', 'r', 'd', '\0' \
    }
#define DEMO_RECORD_WAV_PATH                                                                                         \
    {                                                                                                                \
        SDDISK + '0', ':', '/', 'r', 'e', 'c', 'o', 'r', 'd', '/', 'm', 'u', 's', 'i', 'c', '1', '.', 'w', 'a', 'v', \
            '\0'                                                                                                     \
    }
#endif


#define SDCARD_TASK_STACK_SIZE    (512)

/* SD card management */
static SemaphoreHandle_t sdcardSem;
static volatile bool sdcardInserted;
static volatile bool sdcardInsertedPrev;
static FATFS fileSystem;
static FIL fileObject;

static volatile bool card_ready = 0;

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

static void APP_SDCARD_DetectCallBack(bool isInserted, void *userData)
{
    sdcardInserted = isInserted;
    xSemaphoreGiveFromISR(sdcardSem, NULL);
}

bool SDCARD_inserted(void)
{
    return (sdcardInserted);
}

void SDCARD_write_file()
{
    while (card_ready == 0)
    {
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
    PRINTF("microej_task\r\n");

    PRINTF("Open file\r\n");
    static const char wavpathBuffer[] = DEMO_RECORD_WAV_PATH;
    int error = f_open(&g_fileObject, (char const *)wavpathBuffer, (FA_WRITE | FA_READ | FA_CREATE_ALWAYS));
    PRINTF("Open file %d\r\n", error);

    uint32_t bytesWritten = 0;
    const char p[] = "ciao\0";
    error = f_write(&g_fileObject, (void *)p, sizeof(p), (UINT *)&bytesWritten);
    PRINTF("Written %d erro %d\r\n", bytesWritten, error);

    f_close(&g_fileObject);

}

void APP_SDCARD_Task(void *param)
{
    const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};
    FRESULT error;

    sdcardSem = xSemaphoreCreateBinary();

    BOARD_SD_Config(&g_sd, APP_SDCARD_DetectCallBack, BOARD_SDMMC_SD_HOST_IRQ_PRIORITY, NULL);

    PRINTF("[APP_SDCARD_Task] start\r\n");

    /* SD host init function */
    if (SD_HostInit(&g_sd) != kStatus_Success)
    {
        PRINTF("[APP_SDCARD_Task] SD host init failed.\r\n");
        vTaskSuspend(NULL);
    }

    /* Small delay for SD card detection logic to process */
    vTaskDelay(100 / portTICK_PERIOD_MS);

    while (1)
    {
        /* Block waiting for SDcard detect interrupt */
        xSemaphoreTake(sdcardSem, portMAX_DELAY);

        if (sdcardInserted != sdcardInsertedPrev)
        {
            sdcardInsertedPrev = sdcardInserted;

            SD_SetCardPower(&g_sd, false);

            if (sdcardInserted)
            {
                /* power on the card */
                SD_SetCardPower(&g_sd, true);
                if (f_mount(&fileSystem, driverNumberBuffer, 0U))
                {
                    PRINTF("[APP_SDCARD_Task] Mount volume failed.\r\n");
                    continue;
                }

#if (FF_FS_RPATH >= 2U)
                error = f_chdrive((char const *)&driverNumberBuffer[0U]);
                if (error)
                {
                    PRINTF("[APP_SDCARD_Task] Change drive failed.\r\n");
                    continue;
                }
#endif

                PRINTF("[APP_SDCARD_Task] SD card drive mounted\r\n");

                xSemaphoreGive(sdcardSem);
                card_ready = 1;
            }
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void START_SDCARD_Task(void *param)
{
    PRINTF("START_SDCARD_Task\r\n");
    if (xTaskCreate(APP_SDCARD_Task, "SDCard Task", SDCARD_TASK_STACK_SIZE, NULL, configMAX_PRIORITIES - 5, NULL) !=
        pdPASS)
    {
        PRINTF("ERROR initializing audio\r\n");
        PRINTF("Failed to create application task\r\n");
        while (1)
            ;
    }
}
#endif
