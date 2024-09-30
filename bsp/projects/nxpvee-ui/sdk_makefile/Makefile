#
# Copyright 2023-2024 NXP
#
# SPDX-License-Identifier: BSD-3-Clause
#
TARGET=nxpvee_ui
DEVICE=MIMXRT1176xxxA_M7
ADDRESS=0x30000400
FLAVOUR=flexspi_nor_sdram_debug

ifeq ($(strip $(RELEASE)), 1)
	FLAVOUR=flexspi_nor_sdram_release
endif

ifeq ("$(strip $(BOARD))", "evk")
	BOARD=evk
	BUILD_DIR=../armgcc/$(FLAVOUR)_evk
	LS_DEVICE=MIMXRT1176xxxxx:MIMXRT1170-EVK
else
	BOARD=evkb
	BUILD_DIR=../armgcc/$(FLAVOUR)_evkb
	LS_DEVICE=MIMXRT1176xxxxx:MIMXRT1170-EVKB
endif

include ../../common/sdk_makefile/Makefile
