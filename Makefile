#
# Makefile for phoenix-rtos-devices
#
# Copyright 2018, 2019 Phoenix Systems
#
# %LICENSE%
#

SIL ?= @
MAKEFLAGS += --no-print-directory

#TARGET ?= armv7m3-stm32l152xd
#TARGET ?= armv7m3-stm32l152xe
#TARGET ?= armv7m4-stm32l4x6
#TARGET ?= armv7m7-imxrt105x
#TARGET ?= armv7m7-imxrt106x
#TARGET ?= armv7m7-imxrt117x
#TARGET ?= armv7a7-imx6ull
TARGET ?= ia32-generic
#TARGET ?= riscv64-spike

include ../phoenix-rtos-build/Makefile.common
include ../phoenix-rtos-build/Makefile.$(TARGET_SUFF)

# establish sysroot
SYSROOT := $(shell $(CC) $(CFLAGS) -print-sysroot)
MULTILIB_DIR := $(shell $(CC) $(CFLAGS) -print-multi-directory)
LIBC_INSTALL_DIR := $(SYSROOT)/lib/$(MULTILIB_DIR)
LIBC_INSTALL_NAMES := libc.a libm.a crt0.o libg.a
HEADERS_INSTALL_DIR := $(SYSROOT)/usr/include

ifeq (,$(filter-out /,$(SYSROOT)))
$(error SYSROOT is not supported by the toolchain. Use cross-toolchain to compile.)
endif

# libphoenix shoudl be linked only with GCC library therefore LDLIBS is overloaded
LDLIBS = $(GCCLIB)
CFLAGS += -Iinclude


OBJS = $(PREFIX_O)_startc.o


all: $(PREFIX_A)libphoenix.a headers


include arch/$(TARGET_SUFF)/Makefile
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


SRCHEADERS := $(shell find include -name \*.h)
headers: $(SRCHEADERS) $(PREFIX_A)libphoenix.a
	@echo "HEADERS $(PREFIX_H)*"; \
	mkdir -p "$(PREFIX_H)"; \
	cp -a include/* "$(PREFIX_H)";


#HEADERS := $(patsubst include/%,$(HEADERS_INSTALL_DIR)%,$(SRCHEADERS))
install: $(PREFIX_A)libphoenix.a headers
	@echo INSTALL "$(LIBC_INSTALL_DIR)/*"; \
	mkdir -p "$(LIBC_INSTALL_DIR)"; \
	cp -a "$<" "$(LIBC_INSTALL_DIR)"; \
	for lib in $(LIBC_INSTALL_NAMES); do \
		if [ ! -e "$(LIBC_INSTALL_DIR)/$$lib" ]; then \
			ln -sf "$(LIBC_INSTALL_DIR)/$(LIBNAME)" "$(LIBC_INSTALL_DIR)/$$lib"; \
		fi \
	done; \
	echo INSTALL "$(HEADERS_INSTALL_DIR)/*"; \
	mkdir -p "$(HEADERS_INSTALL_DIR)"; \
	cp -a include/* "$(HEADERS_INSTALL_DIR)";


.PHONY: clean headers install
clean:
	@echo "rm -rf $(BUILD_DIR)"

ifneq ($(filter clean,$(MAKECMDGOALS)),)
	$(shell rm -rf $(BUILD_DIR))
	$(shell rm -f string/*.inc)
endif
