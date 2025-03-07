#
# Copyright 2023-2024 NXP
#
# SPDX-License-Identifier: BSD-3-Clause
#

COMMON_PATH=../../common/sdk_makefile

define del_file
	$(if $(filter $(OS),Windows_NT),$(if $(wildcard $(1)),cmd /c DEL /f /q $(subst /,\\,$(1)),),rm -f $(1))
endef

define del_dir
	$(if $(filter $(OS),Windows_NT),$(if $(wildcard $(1)),cmd /c RD /s /q $(subst /,\\,$(1)),),rm -fr $(1))
endef

ifeq ($(OS),Windows_NT)
JLINKEXE="$(JLINK_INSTALLATION_DIR)/Jlink.exe"
JLINK_GDB="$(COMMON_PATH)/debug/jlink_gdb.bat"
else
JLINKEXE=JLinkExe
JLINK_GDB="$(COMMON_PATH)"/debug/jlink_gdb.sh
endif

JLINK_SCRIPT="jlink_flash.script"

MAKE = ninja
OBJCOPY="$(ARMGCC_DIR)/bin/arm-none-eabi-objcopy"


ifeq ($(OS),Windows_NT)
GDB="$(ARMGCC_DIR)/bin/arm-none-eabi-gdb.exe"
NUM_PROC=
CLEAN_SCRIPT="./clean.bat"
else
GDB=gdb-multiarch
NUM_PROC=$(shell nproc)
CLEAN_SCRIPT="./clean.sh"
endif

ifeq ($(MAKE),make)
MAKE_OP=-j$(NUM_PROC)
endif

all: $(BUILD_DIR) remake

ifeq ($(OS),Windows_NT)
BUILD_SCRIPT="./build_$(FLAVOUR)_$(BOARD).bat"
CMSIS_GDB_TEMP_FOLDER="$(TEMP)"/gdb.cmd
else
BUILD_SCRIPT=/bin/bash build_$(FLAVOUR)_$(BOARD).sh
CMSIS_GDB_TEMP_FOLDER=/tmp/gdb.cmd
endif

$(BUILD_DIR):
	cd ../armgcc/ && $(BUILD_SCRIPT) $(MAKE) $(CMAKE_OPTS)

remake:
	$(MAKE) $(MAKE_OP) -C $(BUILD_DIR)

clean:
	$(call del_dir, $(BUILD_DIR))

distclean:
	cd ../armgcc/ && $(CLEAN_SCRIPT)

flash: $(BUILD_DIR) remake
	$(call del_file,$(JLINK_SCRIPT))
	echo r > $(JLINK_SCRIPT)
	echo loadbin "$(BUILD_DIR)/$(TARGET).bin" $(ADDRESS) >> $(JLINK_SCRIPT)
	echo r >> $(JLINK_SCRIPT)
	echo g >> $(JLINK_SCRIPT)
	echo qc >> $(JLINK_SCRIPT)
	$(JLINKEXE) -NoGui 1 -device $(DEVICE) -If SWD -Speed 10000 -CommanderScript $(JLINK_SCRIPT)
	$(call del_file,$(JLINK_SCRIPT))

flash_cmsisdap: $(BUILD_DIR) remake
	LinkServer flash $(LS_DEVICE) load $(BUILD_DIR)/$(TARGET).elf

gdb_cmsisdap: $(BUILD_DIR) remake
	sed "s+FILE_VALUE+$(BUILD_DIR)/$(TARGET).elf+" $(COMMON_PATH)/cmsisdap_support/gdb.commands > $(CMSIS_GDB_TEMP_FOLDER)
	$(GDB) --command=$(CMSIS_GDB_TEMP_FOLDER)

gdb: $(BUILD_DIR) remake
	$(JLINK_GDB) $(DEVICE)
	$(GDB) --command=$(COMMON_PATH)/debug/jlink.gdb $(BUILD_DIR)/$(TARGET).elf

.PHONY : all remake clean dist_clean gdb
