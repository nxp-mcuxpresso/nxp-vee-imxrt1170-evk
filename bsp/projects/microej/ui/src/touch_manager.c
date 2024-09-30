/*
 * C
 *
 * Copyright 2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "display_support.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "board.h"

#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "pin_mux.h"

#include "fsl_gpio.h"
#include "fsl_adapter_gpio.h"
#include "fsl_debug_console.h"

#include "fsl_gt911.h"

#include "touch_helper.h"

#include "mej_log.h"

// RTOS
#include "FreeRTOS.h"
#include "semphr.h"

// -----------------------------------------------------------------------------
// Macros and Defines
// -----------------------------------------------------------------------------

/*
 * @brief Task delay (in ms) between detecting a multiple pressed events
 */
#define TOUCH_DELAY 15

/*
 * @brief Task stack size
 */
#define TOUCH_STACK_SIZE  (256)

/*
 * @brief Task priority.
 */
#define TOUCH_PRIORITY (5)

#define CM7_GPIO2_TOUCH_PIN (31)

// -----------------------------------------------------------------------------
// Internal function definitions
// -----------------------------------------------------------------------------

/**
 * Retrieves the touch state and coordinates
 */
static status_t __touch_manager_read(void);

/**
 * Touch thread routine
 */
static void __touch_manager_task(void *pvParameters);

/*
 * @brief Pulls the reset pin of the touch controller
 */
void __touch_manager_pull_reset_pin(bool pullUp);

static void BOARD_PullMIPIPanelTouchResetPin(bool pullUp);
static void BOARD_ConfigMIPIPanelTouchIntPin(gt911_int_pin_mode_t mode);
static void custom_time_delay(uint32_t delayMs);

// -----------------------------------------------------------------------------
// Static Constants
// -----------------------------------------------------------------------------

static gt911_handle_t s_touchHandle;
static const gt911_config_t s_touchConfig = { .I2C_SendFunc = BOARD_MIPIPanelTouch_I2C_Send, .I2C_ReceiveFunc = BOARD_MIPIPanelTouch_I2C_Receive,
		.pullResetPinFunc = BOARD_PullMIPIPanelTouchResetPin, .intPinFunc = BOARD_ConfigMIPIPanelTouchIntPin, .timeDelayMsFunc = custom_time_delay, .touchPointNum =
				1, .i2cAddrMode = kGT911_I2cAddrAny, .intTrigMode = kGT911_IntRisingEdge, };
static int s_touchResolutionX;
static int s_touchResolutionY;

// -----------------------------------------------------------------------------
// Static Variables
// -----------------------------------------------------------------------------

static volatile bool s_touchEvent = false;

/*
 * @brief vglite operation semaphore
 */
static SemaphoreHandle_t touch_interrupt_sem;

// -----------------------------------------------------------------------------
// Public functions
// -----------------------------------------------------------------------------
static void custom_time_delay(uint32_t ms)
{
#if defined(SDK_OS_FREE_RTOS)
    TickType_t tick;

    tick = ms * configTICK_RATE_HZ / 1000U;

    tick = (0U == tick) ? 1U : tick;

    vTaskDelay(tick);
#else
    while (0U != (ms--))
    {
        SDK_DelayAtLeastUs(1000U, SystemCoreClock);
    }
#endif
}

static void BOARD_PullMIPIPanelTouchResetPin(bool pullUp) {
	if (pullUp) {
		GPIO_PinWrite(BOARD_MIPI_PANEL_TOUCH_RST_GPIO, BOARD_MIPI_PANEL_TOUCH_RST_PIN, 1);
	} else {
		GPIO_PinWrite(BOARD_MIPI_PANEL_TOUCH_RST_GPIO, BOARD_MIPI_PANEL_TOUCH_RST_PIN, 0);
	}
}

static void BOARD_ConfigMIPIPanelTouchIntPin(gt911_int_pin_mode_t mode) {
	if (mode == kGT911_IntPinInput) {
		BOARD_MIPI_PANEL_TOUCH_INT_GPIO->GDIR &= ~(1UL << BOARD_MIPI_PANEL_TOUCH_INT_PIN);
	} else {
		if (mode == kGT911_IntPinPullDown) {
			GPIO_PinWrite(BOARD_MIPI_PANEL_TOUCH_INT_GPIO, BOARD_MIPI_PANEL_TOUCH_INT_PIN, 0);
		} else {
			GPIO_PinWrite(BOARD_MIPI_PANEL_TOUCH_INT_GPIO, BOARD_MIPI_PANEL_TOUCH_INT_PIN, 1);
		}

		BOARD_MIPI_PANEL_TOUCH_INT_GPIO->GDIR |= (1UL << BOARD_MIPI_PANEL_TOUCH_INT_PIN);
	}
}

// See the header file for the function documentation
void TOUCH_MANAGER_initialize(void) {
	status_t status;
	const gpio_pin_config_t resetPinConfig = { .direction = kGPIO_DigitalOutput, .outputLogic = 0 };

	GPIO_PinInit(BOARD_MIPI_PANEL_TOUCH_RST_GPIO, BOARD_MIPI_PANEL_TOUCH_RST_PIN, &resetPinConfig);
	GPIO_PinInit(BOARD_MIPI_PANEL_TOUCH_INT_GPIO, BOARD_MIPI_PANEL_TOUCH_INT_PIN, &resetPinConfig);

	status = GT911_Init(&s_touchHandle, &s_touchConfig);

	// set pinmux after GT911 config
        IOMUXC_SetPinMux(
                         IOMUXC_GPIO_AD_00_GPIO_MUX2_IO31,       /* GPIO_AD_00 is configured as GPIO_MUX2_IO31 */
                         1U);                                    /* Software Input On Field: Force input path of pad GPIO_AD_00 */

	if (kStatus_Success != status) {
		PRINTF("Touch IC initialization failed waiting\r\n");
        while (1)
            ;
	}

	GT911_GetResolution(&s_touchHandle, &s_touchResolutionX, &s_touchResolutionY);

	touch_interrupt_sem = xSemaphoreCreateBinary();

	gpio_pin_config_t dsi_int_config = {
		kGPIO_DigitalInput,
		0,
		kGPIO_IntRisingEdge,
	};

	NVIC_SetPriority(CM7_GPIO2_3_IRQn, HAL_GPIO_ISR_PRIORITY);
	(void)EnableIRQ(CM7_GPIO2_3_IRQn);
	GPIO_PinInit(CM7_GPIO2, CM7_GPIO2_TOUCH_PIN, &dsi_int_config);

	GPIO_PortEnableInterrupts(CM7_GPIO2, (1U << CM7_GPIO2_TOUCH_PIN));

	/* Create the touch screen task */
	if (pdPASS != xTaskCreate(__touch_manager_task, "Touch screen task", TOUCH_STACK_SIZE, NULL, TOUCH_PRIORITY, NULL)) {
		PRINTF("Touch task initialization failed\r\n");
		assert(false);
	}
}

void TOUCH_MANAGER_interrupt(void) {
		
	uint32_t intStat = GPIO_GetPinsInterruptFlags(CM7_GPIO2);
	if (intStat & (1UL << CM7_GPIO2_TOUCH_PIN)) {
		// Disable interrupt
		GPIO_DisableInterrupts(CM7_GPIO2, (1UL << CM7_GPIO2_TOUCH_PIN));

		GPIO_PortClearInterruptFlags(CM7_GPIO2, (1UL << CM7_GPIO2_TOUCH_PIN));

		portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR(touch_interrupt_sem, &xHigherPriorityTaskWoken);
		if (xHigherPriorityTaskWoken != pdFALSE) {
			// Force a context switch here.
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
	}
}

void CM7_GPIO2_3_IRQHandler(void) {
	TOUCH_MANAGER_interrupt();
}

// -----------------------------------------------------------------------------
// Internal functions
// -----------------------------------------------------------------------------

// See the section 'Internal function definitions' for the function documentation
static status_t __touch_manager_read(void) {
	int touch_x;
	int touch_y;
	status_t status;

	status = GT911_GetSingleTouch(&s_touchHandle, &touch_x, &touch_y);
	if (kStatus_Success == status) {
		TOUCH_HELPER_pressed(touch_x, touch_y);
	} else if (kStatus_TOUCHPANEL_NotTouched == status) {
		TOUCH_HELPER_released();
	}
	return status;
}

// See the section 'Internal function definitions' for the function documentation
static void __touch_manager_task(void *pvParameters) {
	while (1) {
		/* Suspend ourselves */
		// Workaround to read touch data without IRQ handler.
		xSemaphoreTake(touch_interrupt_sem, portMAX_DELAY);
		status_t status;
		/* We have been woken up, lets work ! */
		do {
			vTaskDelay(TOUCH_DELAY / portTICK_PERIOD_MS);
			status = __touch_manager_read();
		} while (kStatus_Success == status);

		/* Reenable interrupt for next event */
		GPIO_EnableInterrupts(CM7_GPIO2, (1UL << CM7_GPIO2_TOUCH_PIN));
	}
}

// See the section 'Internal function definitions' for the function documentation
void __touch_manager_pull_reset_pin(bool pullUp) {
	if (pullUp) {
		GPIO_PinWrite(BOARD_MIPI_PANEL_TOUCH_RST_GPIO, BOARD_MIPI_PANEL_TOUCH_RST_PIN, 1);
	} else {
		GPIO_PinWrite(BOARD_MIPI_PANEL_TOUCH_RST_GPIO, BOARD_MIPI_PANEL_TOUCH_RST_PIN, 0);
	}
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

