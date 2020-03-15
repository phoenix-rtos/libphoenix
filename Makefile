#
# Makefile for phoenix-rtos-devices
#
# Copyright 2018, 2019 Phoenix Systems
#
# %LICENSE%
#

SIL ?= @
MAKEFLAGS += --no-print-directory

#TARGET ?= ia32-qemu
#TARGET ?= armv7-stm32l4
#TARGET ?= arm-imx6ull
TARGET ?= riscv64-spike

# Establish sed tool
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	SED += gsed
else
	SED = sed;
endif

TOPDIR := $(CURDIR)
PREFIX_BUILD ?= ../_build/$(TARGET)
PREFIX_BUILD := $(abspath $(PREFIX_BUILD))
BUILD_DIR ?= $(PREFIX_BUILD)/$(notdir $(TOPDIR))
BUILD_DIR := $(abspath $(BUILD_DIR))

# Compliation options for various architectures
TARGET_FAMILY ?= $(firstword $(subst -, ,$(TARGET)-))
TARGET_SUBFAMILY ?= $(TARGET_FAMILY)-$(word 2,$(subst -, ,$(TARGET)-))
include Makefile.$(TARGET_FAMILY)

# build artifacts dir
CURR_SUFFIX := $(patsubst $(TOPDIR)/%,%,$(abspath $(CURDIR))/)
PREFIX_O := $(BUILD_DIR)/$(CURR_SUFFIX)

# target install paths, can be provided exterally
PREFIX_A ?= $(PREFIX_BUILD)/lib/
PREFIX_H ?= $(PREFIX_BUILD)/include/
PREFIX_PROG ?= $(PREFIX_BUILD)/prog/
PREFIX_PROG_STRIPPED ?= $(PREFIX_BUILD)/prog.stripped/

CFLAGS += -I"$(PREFIX_H)" -I/usr/local/include

# add include path for auto-generated files
CFLAGS += -I"$(BUILD_DIR)/$(CURR_SUFFIX)" -Iinclude/

ARCH =  $(SIL)@mkdir -p $(@D); \
	(printf "AR  %-24s\n" "$(@F)"); \
	$(AR) $(ARFLAGS) $@ $^ 2>/dev/null

LINK = $(SIL)mkdir -p $(@D); \
	(printf "LD  %-24s\n" "$(@F)"); \
	$(LD) $(LDFLAGS) -o "$@"  $^ $(LDLIBS)
	
HEADER = $(SIL)mkdir -p $(@D); \
	(printf "HEADER %-24s\n" "$<"); \
	cp -pR "$<" "$@"

$(PREFIX_O)%.o: %.c
	@mkdir -p $(@D)
	$(SIL)(printf "CC  %-24s\n" "$<")
	$(SIL)$(CC) -c $(CFLAGS) "$<" -o "$@"
	$(SIL)$(CC) -M  -MD -MP -MF $(PREFIX_O)$*.c.d -MT "$@" $(CFLAGS) $<

$(PREFIX_O)%.o: %.S
	@mkdir -p $(@D)
	$(SIL)(printf "ASM %-24s\n" "$<")
	$(SIL)$(CC) -c $(CFLAGS) "$<" -o "$@"
	$(SIL)$(CC) -M  -MD -MP -MF $(PREFIX_O)$*.S.d -MT "$@" $(CFLAGS) $<
	
$(PREFIX_PROG_STRIPPED)%: $(PREFIX_PROG)%
	@mkdir -p $(@D)
	@(printf "STR %-24s\n" "$(@F)")
	$(SIL)$(STRIP) -o $@ $<

OBJS = $(PREFIX_O)_startc.o


all: $(PREFIX_A)libphoenix.a


include $(ARCHMAKE)
include ctype/Makefile
include err/Makefile
include errno/Makefile
include locale/Makefile
include math/Makefile
include net/Makefile
include netinet/Makefile
include posix/Makefile
#include pthread/Makefile
include regex/Makefile
include signal/Makefile
include stdio/Makefile
include stdlib/Makefile
include string/Makefile
include sys/Makefile
include syslog/Makefile	
include termios/Makefile
include time/Makefile
include unistd/Makefile

include test/Makefile

$(PREFIX_A)libphoenix.a: $(OBJS)
	$(ARCH)


.PHONY: clean
clean:
	@echo "rm -rf $(BUILD_DIR)"

ifneq ($(filter clean,$(MAKECMDGOALS)),)
	$(shell rm -rf $(BUILD_DIR))
	$(shell rm -f string/*.inc)
endif
