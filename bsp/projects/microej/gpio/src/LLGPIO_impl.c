/**
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief NXP GPIO low level API
 * @author Julien Jouan
 * @version 1.0.0
 */

#include <LLGPIO_impl.h>
#include "LLGPIO_mux.h"
#include "sni.h"
#include "fsl_gpio.h"

static int8_t check_port_and_pin(uint32_t port, uint32_t pin)
{
    int8_t ret = kPinError;

    switch (port)
    {
        case GPIO01:
        case GPIO02:
        case GPIO03:
        case GPIO04:
        case GPIO07:
        case GPIO08:
        case GPIO09:
        case GPIO010:
            ret = kSuccess;
            break;
        case GPIO05:
        case GPIO011:
            if (pin <= GPIO05_MAX_PIN) ret = kSuccess;
            break;
        case GPIO06:
        case GPIO012:
            if (pin <= GPIO06_MAX_PIN) ret = kSuccess;
            break;
        case GPIO013:
            if (pin <= GPIO013_MAX_PIN) ret = kSuccess;
            break;
        default:
            ret = kPortError;
            break;
    }
    return ret;
}

int32_t LLGPIO_IMPL_init_gpio_pin(uint32_t pin, uint32_t direction, uint32_t pullConfig)
{
    uint32_t port = pin / GPIO_BANK_SIZE;
    pin %= GPIO_BANK_SIZE;

    gpio_pin_config_t pin_config = {(gpio_pin_direction_t)direction, 0, kGPIO_NoIntmode};

    uint32_t muxRegister;
    uint32_t muxMode;
    uint32_t inputRegister = 0;
    uint32_t inputDaisy = 0;
    uint32_t configRegister;
    uint32_t inputOnfield = 0;
    uint32_t configValue = 0;

    int8_t ret = kSuccess;

    /* Configure registers address according to GPIO port and pin. */
    switch(port)
    {
    case GPIO01:
    case GPIO07:
        muxMode = (port < GPIO07) ? GPIO_FUNCTION_5 : GPIO_FUNCTION_10;
        muxRegister = GPIO_1_7_MUX_REGISTER_BASE + 4*pin;
        configRegister = GPIO_1_7_CONFIG_REGISTER_BASE + 4*pin;
        /* Configure pull up/down according to pin */
        if (pullConfig)
        {
            configValue = IOMUXC_SW_PAD_CTL_PAD_PULL(pullConfig);
        }
        break;
    case GPIO02:
    case GPIO08:
        muxMode = (port < GPIO07) ? GPIO_FUNCTION_5 : GPIO_FUNCTION_10;
        muxRegister = GPIO_2_8_MUX_REGISTER_BASE + 4*pin;
        configRegister = GPIO_2_8_CONFIG_REGISTER_BASE + 4*pin;
        /* Configure pull up/down according to pin */
        if (pullConfig)
        {
            if(pin > 30)
            {
                configValue = IOMUXC_SW_PAD_CTL_PAD_PUE(1) | IOMUXC_SW_PAD_CTL_PAD_PUS(pullConfig % 2);
            }
            else
            {
                configValue = IOMUXC_SW_PAD_CTL_PAD_PULL(pullConfig);
            }
        }
        break;
    case GPIO03:
    case GPIO09:
        muxMode = (port < GPIO07) ? GPIO_FUNCTION_5 : GPIO_FUNCTION_10;
        muxRegister = GPIO_3_9_MUX_REGISTER_BASE + 4*pin;
        configRegister = GPIO_3_9_CONFIG_REGISTER_BASE + 4*pin;
        /* Configure pull up/down according to pin */
        if (pullConfig)
        {
            configValue = IOMUXC_SW_PAD_CTL_PAD_PUE(1) | IOMUXC_SW_PAD_CTL_PAD_PUS(pullConfig % 2);
        }
        break;
    case GPIO04:
    case GPIO010:
        muxMode = (port < GPIO07) ? GPIO_FUNCTION_5 : GPIO_FUNCTION_10;
        muxRegister = GPIO_4_10_MUX_REGISTER_BASE + 4*pin;
        configRegister = GPIO_4_10_CONFIG_REGISTER_BASE + 4*pin;
        /* Configure pull up/down according to pin */
        if (pullConfig)
        {
            if(pin > 2)
            {
                configValue = IOMUXC_SW_PAD_CTL_PAD_PULL(pullConfig);
            }
            else
            {
                configValue = IOMUXC_SW_PAD_CTL_PAD_PUE(1) | IOMUXC_SW_PAD_CTL_PAD_PUS(pullConfig % 2);
            }
        }
        break;
    case GPIO05:
    case GPIO011:
        // Check pin is available
        if (pin > GPIO05_MAX_PIN)
        {
            ret = kPinError;
            break;
        }
        muxMode = (port < GPIO07) ? GPIO_FUNCTION_5 : GPIO_FUNCTION_10;
        muxRegister = GPIO_5_11_MUX_REGISTER_BASE + 4*pin;
        configRegister = GPIO_5_11_CONFIG_REGISTER_BASE + 4*pin;
        /* Configure pull up/down according to pin */
        if (pullConfig)
        {
            if(pin > 0)
            {
                configValue = IOMUXC_SW_PAD_CTL_PAD_PUE(1) | IOMUXC_SW_PAD_CTL_PAD_PUS(pullConfig % 2);
            }
            else
            {
                configValue = IOMUXC_SW_PAD_CTL_PAD_PULL(pullConfig);
            }
        }
        break;
    case GPIO06:
    case GPIO012:
        // Check pin is available
        if (pin > GPIO06_MAX_PIN)
        {
            ret = kPinError;
            break;
        }
        muxMode = (port < GPIO07) ? GPIO_FUNCTION_5 : GPIO_FUNCTION_10;
        muxRegister = GPIO_6_12_MUX_REGISTER_BASE + 4*pin;
        configRegister = GPIO_6_12_CONFIG_REGISTER_BASE + 4*pin;
        /* Configure pull up/down according to pin */
        if (pullConfig)
        {
            configValue = IOMUXC_SW_PAD_CTL_PAD_PUE(1) | IOMUXC_SW_PAD_CTL_PAD_PUS(pullConfig % 2);
        }
        break;
    case GPIO013:
        /* Check pin is available */
        if (pin > GPIO013_MAX_PIN)
        {
            ret = kPinError;
            break;
        }
        muxMode = GPIO_FUNCTION_5;
        muxRegister = GPIO_13_MUX_REGISTER_BASE + 4*pin;
        configRegister = GPIO_13_CONFIG_REGISTER_BASE + 4*pin;
        /* Configure pull up/down according to pin */
        if (pullConfig)
        {
            configValue = IOMUXC_SW_PAD_CTL_PAD_PUE(1) | IOMUXC_SW_PAD_CTL_PAD_PUS(pullConfig % 2);
        }
        break;

    /* If GPIO port is not catch, it is not available */
    default:
        ret = kPortError;
        break;
    }

    if (ret == kSuccess)
    {
        CLOCK_EnableClock(kCLOCK_Iomuxc);

        IOMUXC_SetPinMux(muxRegister, muxMode, inputRegister, inputDaisy, configRegister, inputOnfield);
        IOMUXC_SetPinConfig(muxRegister, muxMode, inputRegister, inputDaisy, configRegister, configValue);

        GPIO_PinInit(GPIO_ARRAY[port-1], pin, &pin_config);
    }

    return ret;
}

int32_t LLGPIO_IMPL_read_gpio_pin(uint32_t pin)
{
    uint32_t port = pin / GPIO_BANK_SIZE;
    pin %= GPIO_BANK_SIZE;

    /* Check that GPIO port and pin used is existing */
    int8_t portPinStatus = check_port_and_pin(port, pin);
    int8_t ret;
    if (portPinStatus == kSuccess)
    {
        ret = GPIO_PinRead(GPIO_ARRAY[port-1], pin);
    }
    else
    {
        ret = portPinStatus;
    }

    return ret;
}

int32_t LLGPIO_IMPL_write_gpio_pin(uint32_t pin, uint32_t level)
{
    uint32_t port = pin / GPIO_BANK_SIZE;
    pin %= GPIO_BANK_SIZE;

    /* Check that GPIO port and pin used is existing */
    int8_t portPinStatus = check_port_and_pin(port, pin);
    int8_t ret;
    if (portPinStatus == kSuccess)
    {
        GPIO_PinWrite(GPIO_ARRAY[port-1], pin, level);
        ret = GPIO_PinRead(GPIO_ARRAY[port-1], pin);
    }
    else
    {
        ret = portPinStatus;
    }

    return ret;
}

int32_t LLGPIO_IMPL_toggle_gpio_pin(uint32_t pin)
{
    uint32_t port = pin / GPIO_BANK_SIZE;
    pin %= GPIO_BANK_SIZE;

    /* Check that GPIO port and pin used is existing */
    int8_t portPinStatus = check_port_and_pin(port, pin);
    int8_t ret;
    if (portPinStatus == kSuccess)
    {
        GPIO_PortToggle(GPIO_ARRAY[port-1], (uint32_t) (1 << pin));
        ret = GPIO_PinRead(GPIO_ARRAY[port-1], pin);
    }
    else
    {
        ret = portPinStatus;
    }

    return ret;
}
