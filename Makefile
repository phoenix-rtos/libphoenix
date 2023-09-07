#
# Makefile for phoenix-rtos-devices
#
# Copyright 2018, 2019 Phoenix Systems
#
# %LICENSE%
#

SIL ?= @
MAKEFLAGS += --no-print-directory

include ../phoenix-rtos-build/Makefile.common

# establish sysroot
SYSROOT := $(shell $(CC) $(CFLAGS) -print-sysroot)
MULTILIB_DIR := $(shell $(CC) $(CFLAGS) -print-multi-directory)
LIBC_INSTALL_DIR := $(SYSROOT)/lib/$(MULTILIB_DIR)
LIBC_INSTALL_NAMES := libc.a libm.a crt0.o libg.a libpthread.a
HEADERS_INSTALL_DIR := $(SYSROOT)/usr/include
LIBNAME := libphoenix.a

ifeq (,$(filter-out /,$(SYSROOT)))
$(error SYSROOT is not supported by the toolchain. Use cross-toolchain to compile.)
endif

# libphoenix shoudl be linked only with GCC library therefore LDLIBS is overloaded
LDLIBS = $(GCCLIB)
CFLAGS += -Iinclude -fno-builtin-malloc


OBJS = $(PREFIX_O)_startc.o


all: $(PREFIX_A)libphoenix.a


include arch/$(TARGET_SUFF)/Makefile
include ctype/Makefile
include err/Makefile
include errno/Makefile
include locale/Makefile
include math/Makefile
include misc/Makefile
include net/Makefile
include netinet/Makefile
include posix/Makefile
include pthread/Makefile
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
include wchar/Makefile

#include test/Makefile

$(PREFIX_A)libphoenix.a: $(OBJS)
	$(ARCH)


SRCHEADERS := $(shell find include -name \*.h)

#HEADERS := $(patsubst include/%,$(HEADERS_INSTALL_DIR)%,$(SRCHEADERS))
install: $(PREFIX_A)libphoenix.a $(SRCHEADERS)
	@echo INSTALL "$(LIBC_INSTALL_DIR)/*"; \
	mkdir -p "$(LIBC_INSTALL_DIR)"; \
	cp -a "$<" "$(LIBC_INSTALL_DIR)"; \
	for lib in $(LIBC_INSTALL_NAMES); do \
		if [ ! -e "$(LIBC_INSTALL_DIR)/$$lib" ]; then \
			ln -sf "$(LIBC_INSTALL_DIR)/libphoenix.a" "$(LIBC_INSTALL_DIR)/$$lib"; \
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
