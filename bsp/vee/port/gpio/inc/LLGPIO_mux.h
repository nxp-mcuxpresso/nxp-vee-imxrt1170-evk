/**
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LLGPIO_MUX
#define LLGPIO_MUX

#include "fsl_iomuxc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GPIO_1_7_MUX_REGISTER_BASE  0x400E8010U
#define GPIO_2_8_MUX_REGISTER_BASE  0x400E8090U
#define GPIO_3_9_MUX_REGISTER_BASE  0x400E8110U
#define GPIO_4_10_MUX_REGISTER_BASE 0x400E8190U
#define GPIO_5_11_MUX_REGISTER_BASE 0x400E8210U
#define GPIO_6_12_MUX_REGISTER_BASE 0x40C08000U
#define GPIO_13_MUX_REGISTER_BASE   0x40C94000U

#define GPIO_1_7_CONFIG_REGISTER_BASE   0x400E8254U
#define GPIO_2_8_CONFIG_REGISTER_BASE   0x400E82D4U
#define GPIO_3_9_CONFIG_REGISTER_BASE   0x400E8354U
#define GPIO_4_10_CONFIG_REGISTER_BASE  0x400E83D4U
#define GPIO_5_11_CONFIG_REGISTER_BASE  0x400E8454U
#define GPIO_6_12_CONFIG_REGISTER_BASE  0x40C08040U
#define GPIO_13_CONFIG_REGISTER_BASE    0x40C94040U

#define MAX_PORT_NUMBER             13
#define GPIO_BANK_SIZE              32
#define GPIO01                      1
#define GPIO02                      2
#define GPIO03                      3
#define GPIO04                      4
#define GPIO05                      5
#define GPIO06                      6
#define GPIO07                      7
#define GPIO08                      8
#define GPIO09                      9
#define GPIO010                     10
#define GPIO011                     11
#define GPIO012                     12
#define GPIO013                     13
#define GPIO05_MAX_PIN              16
#define GPIO06_MAX_PIN              15
#define GPIO013_MAX_PIN             12

#define GPIO_FUNCTION_5     0x05U
#define GPIO_FUNCTION_10    0x0AU

GPIO_Type *GPIO_ARRAY[13] = {
    GPIO1,
    GPIO2,
    GPIO3,
    GPIO4,
    GPIO5,
    GPIO6,
    GPIO7,
    GPIO8,
    GPIO9,
    GPIO10,
    GPIO11,
    GPIO12,
    GPIO13
};

typedef enum
{
    kSuccess = 0,
    kPortError = -1,
    kPinError = -2,
} gpio_init_status;

#ifdef __cplusplus
}
#endif
#endif
