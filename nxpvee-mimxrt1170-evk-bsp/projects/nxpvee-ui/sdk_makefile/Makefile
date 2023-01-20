#
# Copyright 2023 NXP
#
# SPDX-License-Identifier: BSD-3-Clause
#
TARGET=nxpvee_ui
DEVICE=MIMXRT1176xxxA_M7
ADDRESS=0x30000400
FLAVOUR=flexspi_nor_sdram_debug

ifeq ($(strip $(RELEASE)),1)
	FLAVOUR=flexspi_nor_sdram_release
endif

include ../../common/sdk_makefile/Makefile
