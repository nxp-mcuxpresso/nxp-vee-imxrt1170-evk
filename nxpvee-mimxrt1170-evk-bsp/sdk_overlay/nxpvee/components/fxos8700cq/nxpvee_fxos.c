/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "nxpvee_fxos.h"

static fxos_handle_t g_fxosHandle;

#define VALID_HANDLE 0x00


extern status_t BOARD_Accel_I2C_Send(uint8_t deviceAddress, uint32_t subAddress, uint8_t subaddressSize, uint32_t txBuff);

extern status_t BOARD_Accel_I2C_Receive(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subaddressSize, uint8_t *rxBuff, uint8_t rxBuffSize);

extern void BOARD_Accel_I2C_Init(void);

uint8_t g_sensor_address[] = {0x1CU, 0x1EU, 0x1DU, 0x1FU};

int nxpvee_fxos_init()
{
    fxos_config_t config = {0};
    BOARD_Accel_I2C_Init();
    uint8_t array_addr_size = 0;
    status_t result;

    /* Configure the I2C function */
    config.I2C_SendFunc    = BOARD_Accel_I2C_Send;
    config.I2C_ReceiveFunc = BOARD_Accel_I2C_Receive;

    static bool initialized = false;

    if (initialized)
    {
        PRINTF("\r\nSensor already initialized\r\n");
        return VALID_HANDLE;
    }

    initialized = true;

    /* Initialize sensor devices */
    array_addr_size = sizeof(g_sensor_address) / sizeof(g_sensor_address[0]);
    for (int i = 0; i < array_addr_size; i++)
    {
        config.slaveAddress = g_sensor_address[i];

        /* Initialize accelerometer sensor */
        result = FXOS_Init(&g_fxosHandle, &config);
        if (result == kStatus_Success)
        {
            break;
        }
    }

    if (result != kStatus_Success)
    {
        PRINTF("\r\nSensor device initialize failed!\r\n");
        return -1;
    }

    return VALID_HANDLE;
}

static bool check_handle(int fxos_handle)
{
    if (fxos_handle == VALID_HANDLE)
    {
        return true;
    }
    else
    {
        PRINTF("Error: invalid handle \r\n");
        return false;
    }
}

status_t nxpvee_fxos_read(int fxos_handle, float frequency,  int16_t *Ax, int16_t *Ay, int16_t *Az, int16_t *Mx, int16_t *My, int16_t *Mz)
{
    status_t ret = kStatus_Success;
    fxos_data_t fxos_data;

    if(check_handle(fxos_handle))
    {
        if (FXOS_ReadSensorData(&g_fxosHandle, &fxos_data) != kStatus_Success)
        {
            PRINTF("Failed to read acceleration data!\r\n");
            ret = kStatus_Fail;
        }

        /* Get the accel data from the sensor data structure in 14 bit left format data*/
        *Ax = (int16_t)((uint16_t)((uint16_t)fxos_data.accelXMSB << 8) | (uint16_t)fxos_data.accelXLSB) / 4U;
        *Ay = (int16_t)((uint16_t)((uint16_t)fxos_data.accelYMSB << 8) | (uint16_t)fxos_data.accelYLSB) / 4U;
        *Az = (int16_t)((uint16_t)((uint16_t)fxos_data.accelZMSB << 8) | (uint16_t)fxos_data.accelZLSB) / 4U;

        *Mx = (int16_t)((uint16_t)((uint16_t)fxos_data.magXMSB << 8) | (uint16_t)fxos_data.magXLSB);
        *My = (int16_t)((uint16_t)((uint16_t)fxos_data.magYMSB << 8) | (uint16_t)fxos_data.magYLSB);
        *Mz = (int16_t)((uint16_t)((uint16_t)fxos_data.magZMSB << 8) | (uint16_t)fxos_data.magZLSB);

        /* Wait between measurements */
        vTaskDelay(1000/(frequency * portTICK_PERIOD_MS));
    }

    else
    {
        ret = kStatus_Fail;
    }

    return ret;
}

/* Configure the basic parameters of the fxos8700 */
status_t nxpvee_fxos_config(float frequency)
{
    status_t ret = kStatus_Success;
    // Create register frequency value

    uint8_t sampling_frequency = 0;

    if (frequency_value_to_binary((int)(frequency*100), &sampling_frequency) != kStatus_Success)
    {
        PRINTF("\r\nError: invalid frequency, failed to configure.\r\n");
        PRINTF("Accepted frequency values (in Hz): \r\n");
        PRINTF("800,\t400,\t200,\t100,\t50,\t25,\t6.25,\t3.15,\t0.8\r\n\r\n");
        ret = kStatus_Fail;
    }

    if (nxpvee_fxos_config_registers(sampling_frequency) != kStatus_Success)
    {
        ret = kStatus_Fail;
    }

    return ret;
}

/* Write the FXOS configuration registers */
static status_t nxpvee_fxos_config_registers(uint8_t frequency)
{
    uint8_t tmp[1] = {0};

    if (FXOS_WriteReg(&g_fxosHandle, CTRL_REG1, tmp[0] & (uint8_t)~ACTIVE_MASK) != kStatus_Success)
    {
        return kStatus_Fail;
    }

    /* Read again to make sure we are in standby mode. */
    if (FXOS_ReadReg(&g_fxosHandle, CTRL_REG1, tmp, 1) != kStatus_Success)
    {
        return kStatus_Fail;
    }

    /* Configure frequency */
    if (FXOS_WriteReg(&g_fxosHandle, CTRL_REG1,(frequency | ACTIVE_MASK)) != kStatus_Success)
    {
        return kStatus_Fail;
    }

    /* Read Control register again to ensure we are in active mode */
    if (FXOS_ReadReg(&g_fxosHandle, CTRL_REG1, tmp, 1) != kStatus_Success)
    {
        return kStatus_Fail;
    }

    if ((tmp[0] & ACTIVE_MASK) != ACTIVE_MASK)
    {
        return kStatus_Fail;
    }

    return kStatus_Success;
}

/* Convert frequency value to binary */
static status_t frequency_value_to_binary(int freq, uint8_t *reg)
{
    int ret = kStatus_Success;

    switch (freq)
    {
        case 40000U:
            *reg = HYB_DATA_RATE_400HZ;
            break;
        case 20000U:
            *reg = HYB_DATA_RATE_200HZ;
            break;
        case 10000U:
            *reg = HYB_DATA_RATE_100HZ;
            break;
        case 5000U:
            *reg = HYB_DATA_RATE_50HZ;
            break;
        case 2500U:
            *reg = HYB_DATA_RATE_25HZ;
            break;
        case 625U:
            *reg = HYB_DATA_RATE_6_25HZ;
            break;
        case 315U:
            *reg = HYB_DATA_RATE_3_15HZ;
            break;
        case 80U:
            *reg = HYB_DATA_RATE_0_8HZ;
            break;
        default:
          ret = kStatus_Fail;
    }
    return ret;
}

/* Declaration of native Java functions */
int Java_com_nxp_components_Fxos8700_1Natives_InitAccelerometer()
{
    return nxpvee_fxos_init();
}

int Java_com_nxp_components_Fxos8700_1Natives_ReadData(int fxos_handle, float frequency, int16_t *buffer, int size)
{
    int ret = 0;
    if(size != 6)
    {
        ret = -1;
        PRINTF("ERROR: Invalid buffer size.");
    }

    if(nxpvee_fxos_read(fxos_handle, frequency, &buffer[0], &buffer[1], &buffer[2], &buffer[3], &buffer[4], &buffer[5]) != kStatus_Success)
    {
        ret = 1;
    }

    return ret;
}

int Java_com_nxp_components_Fxos8700_1Natives_ConfigAccelerometer(float frequency)
{
    int ret = 0;
    if(nxpvee_fxos_config(frequency) != kStatus_Success)
    {
      ret = 1;
    }

    return 0;
}
