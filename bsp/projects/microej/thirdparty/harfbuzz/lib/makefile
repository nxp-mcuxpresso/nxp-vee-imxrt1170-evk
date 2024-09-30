# Copyright 2022-2024 MicroEJ Corp. This file has been modified and/or created by MicroEJ Corp.
#
#  This is part of HarfBuzz, a text shaping library.
#
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the
# above copyright notice and the following two paragraphs appear in
# all copies of this software.
#
# IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR
# DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
# ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
# IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
# DAMAGE.
#
# THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
# ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATION TO
# PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

FREETYPE_INCLUDE_DIR = ../../freetype/inc

ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
    detected_OS := Windows
    CC := arm-none-eabi-c++.exe
    AR := arm-none-eabi-ar.exe
    SHELL := bash.exe
else
    detected_OS := $(shell uname)  # same as "uname -s"
    CC := arm-none-eabi-c++
    AR := arm-none-eabi-ar
endif

$(info $$os is [${detected_OS}])

# Misc checks
#############

# Check CC is in PATH environment variable
CC_CHECK := $(shell $(CC) --version > /dev/null; echo $$?)
ifneq ($(CC_CHECK),0)
    $(error "Can not find $(CC) in PATH environment variable")
endif

# Check FREETYPE_INCLUDE_DIR is correct
FT2BUILD = $(FREETYPE_INCLUDE_DIR)/ft2build.h
ifeq ("$(wildcard ${FT2BUILD})","")
    $(error ${FT2BUILD} does not exist, FREETYPE_INCLUDE_DIR variable must be define to freetype header include dir)
    
endif

# Source files
##############

CC_SRCS = $(wildcard ../src/harfbuzz.cc) 

INC_DIRS = -I ../inc \
           -I ${FREETYPE_INCLUDE_DIR}
           
OBJDIR = .
OBJS  := $(CC_SRCS:.cc=.o)
OBJS  := $(foreach file,$(OBJS),$(OBJDIR)/$(notdir $(file))) 
vpath %c $(foreach file,$(CC_SRCS),$(dir $(file)))

# Compilation flags
###################

DEFINES := -D HB_CUSTOM_MALLOC

ARCHFLAGS := -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb  

CFLAGS := $(ARCHFLAGS)
CFLAGS +=  -D__NEWLIB__ -O3 -g3 -Wall -fmessage-length=0
CFLAGS +=  -ffunction-sections -fdata-sections -ffreestanding -fno-builtin
CFLAGS += -specs=nano.specs -fpermissive

# Some LIBC and LIBGCC files are needed in the harfbuzz.a library
LIBC_OBJS = \
lib_a-errno.o \
lib_a-impure.o

LIBGCC_OBJS = \
_ctzdi2.o \
_popcountdi2.o \
_popcountsi2.o 

LIBC=$(shell $(CC) $(ARCHFLAGS) -print-file-name=libc.a)
LIBGCC=$(shell $(CC) $(ARCHFLAGS) -print-file-name=libgcc.a)

# Targets
#########

all: harfbuzz.a

$(OBJS):$(OBJDIR)/%.o: %.cc
	$(CC) -c $(CFLAGS) $(INC_DIRS) $(DEFINES) -o "$@" $<

harfbuzz.a: $(OBJS)
	@echo '-- Building target: $@'
	
	@echo '-- Extract objects from libc.a and libgcc.a'
	$(AR) x "$(LIBC)" $(LIBC_OBJS)
	$(AR) x "$(LIBGCC)" $(LIBGCC_OBJS)

	@echo '-- Archive objects to $@'
	$(AR) rcs $@ $(OBJS) $(LIBC_OBJS) $(LIBGCC_OBJS)
	
	@echo '-- Cleanup objects files'
	rm -f $(OBJS) $(LIBC_OBJS) $(LIBGCC_OBJS)
	@echo '-- Finished building target: $@'
	@echo ' '



