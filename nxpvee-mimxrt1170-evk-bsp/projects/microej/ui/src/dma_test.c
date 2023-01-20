
/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*
 * Copyright 2022-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
*/

#include "dma_test.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_edma.h"
#include "fsl_dmamux.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_DMA                 DMA0
#define EXAMPLE_DMAMUX              DMAMUX0
#define DMA0_DMA16_DriverIRQHandler DMA_CH_0_16_DriverIRQHandler
#define BUFF_LENGTH 4U

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
edma_handle_t g_EDMA_Handle0;
edma_handle_t g_EDMA_Handle4;
volatile bool g_Transfer_Done0 = false;
volatile bool g_Transfer_Done4 = false;

/*******************************************************************************
 * Code
 ******************************************************************************/

/* User callback function for EDMA transfer. */
void EDMA_Callback0(edma_handle_t *handle, void *param, bool transferDone, uint32_t tcds)
{
    if (transferDone)
    {
        g_Transfer_Done0 = true;
    }
}
void EDMA_Callback4(edma_handle_t *handle, void *param, bool transferDone, uint32_t tcds)
{
    if (transferDone)
    {
        g_Transfer_Done4 = true;
    }
}

AT_NONCACHEABLE_SECTION_INIT(uint32_t srcAddr[BUFF_LENGTH])   = {0x01, 0x02, 0x03, 0x04};
AT_NONCACHEABLE_SECTION_INIT(uint32_t destAddr0[BUFF_LENGTH]) = {0x00, 0x00, 0x00, 0x00};
AT_NONCACHEABLE_SECTION_INIT(uint32_t destAddr4[BUFF_LENGTH]) = {0x00, 0x00, 0x00, 0x00};

/*!
 * @brief Main function
 */
#define CHANNEL0 0
#define CHANNEL4 4

void make_transfer0()
{
    edma_transfer_config_t transferConfig;


#if defined(FSL_FEATURE_DMAMUX_HAS_A_ON) && FSL_FEATURE_DMAMUX_HAS_A_ON
    DMAMUX_EnableAlwaysOn(EXAMPLE_DMAMUX, CHANNEL0, true);
#else
    DMAMUX_SetSource(EXAMPLE_DMAMUX, CHANNEL0, 63);
#endif /* FSL_FEATURE_DMAMUX_HAS_A_ON */
    DMAMUX_EnableChannel(EXAMPLE_DMAMUX, CHANNEL0);
    /* Configure EDMA one shot transfer */
    /*
     * userConfig.enableRoundRobinArbitration = false;
     * userConfig.enableHaltOnError = true;
     * userConfig.enableContinuousLinkMode = false;
     * userConfig.enableDebugMode = false;
     */

    EDMA_CreateHandle(&g_EDMA_Handle0, EXAMPLE_DMA, CHANNEL0);
    EDMA_SetCallback(&g_EDMA_Handle0, EDMA_Callback0, NULL);
    EDMA_PrepareTransfer(&transferConfig, srcAddr, sizeof(srcAddr[0]), destAddr0, sizeof(destAddr4[0]),
                         sizeof(srcAddr[0]), sizeof(srcAddr), kEDMA_MemoryToMemory);
    EDMA_SubmitTransfer(&g_EDMA_Handle0, &transferConfig);
    EDMA_StartTransfer(&g_EDMA_Handle0);

}


void make_transfer4()
{
    edma_transfer_config_t transferConfig;


#if defined(FSL_FEATURE_DMAMUX_HAS_A_ON) && FSL_FEATURE_DMAMUX_HAS_A_ON
    DMAMUX_EnableAlwaysOn(EXAMPLE_DMAMUX, CHANNEL4, true);
#else
    DMAMUX_SetSource(EXAMPLE_DMAMUX, CHANNEL4, 63);
#endif /* FSL_FEATURE_DMAMUX_HAS_A_ON */
    DMAMUX_EnableChannel(EXAMPLE_DMAMUX, CHANNEL4);
    /* Configure EDMA one shot transfer */
    /*
     * userConfig.enableRoundRobinArbitration = false;
     * userConfig.enableHaltOnError = true;
     * userConfig.enableContinuousLinkMode = false;
     * userConfig.enableDebugMode = false;
     */

    EDMA_CreateHandle(&g_EDMA_Handle4, EXAMPLE_DMA, CHANNEL4);
    EDMA_SetCallback(&g_EDMA_Handle4, EDMA_Callback4, NULL);
    EDMA_PrepareTransfer(&transferConfig, srcAddr, sizeof(srcAddr[0]), destAddr4, sizeof(destAddr4[0]),
                         sizeof(srcAddr[0]), sizeof(srcAddr), kEDMA_MemoryToMemory);
    EDMA_SubmitTransfer(&g_EDMA_Handle4, &transferConfig);
    EDMA_StartTransfer(&g_EDMA_Handle4);

}

void DMA_test(void)
{
    uint32_t i = 0;
    /* Print source buffer */
    PRINTF("==============================\r\n");
    PRINTF("EDMA memory to memory transfer example begin.\r\n");
    PRINTF("Destination Buffer0:\t");
    for (i = 0; i < BUFF_LENGTH; i++)
    {
        PRINTF("%d\t", destAddr0[i]);
    }
    PRINTF("\r\nDestination Buffer4:\t");
    for (i = 0; i < BUFF_LENGTH; i++)
    {
        PRINTF("%d\t", destAddr4[i]);
    }

    make_transfer4();

    /* Wait for EDMA transfer finish */
    while (g_Transfer_Done4 != true)
    {
    }
    PRINTF("Destination Buffer4: ");
    for (i = 0; i < BUFF_LENGTH; i++)
    {
        PRINTF("%d\t", destAddr4[i]);
    }
    PRINTF("\r\n");
}

