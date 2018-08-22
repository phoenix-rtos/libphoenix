#
# Makefile for libphoenix
#
# Copyright 2017 Phoenix Systems
#
# %LICENSE%
#

SIL ?= @
MAKEFLAGS += --no-print-directory --output-sync

#TARGET ?= arm-imx
#TARGET ?= armv7-stm32-tiramisu
#TARGET ?= ia32-qemu
TARGET ?= riscv64

VERSION = 0.2
TOPDIR := $(CURDIR)
BUILD_DIR ?= build/$(TARGET)
BUILD_DIR := $(abspath $(BUILD_DIR))

SUBSYSTEMS := math stdio stdlib string sys ctype time unistd errno signal termios posix err locale regex net syslog
EXTRA_HEADER_DIRS := net netinet arpa

LIBNAME := libphoenix.a
LIB := $(BUILD_DIR)/$(LIBNAME)

include Makefile.targets

POSIXSRV = $(TOPDIR)/../phoenix-rtos-posixsrv
CFLAGS += -I"$(TOPDIR)"
CFLAGS += -DVERSION=\"$(VERSION)\"
CFLAGS += -fdata-sections -ffunction-sections
LDFLAGS += --gc-sections

ARCH = code.a
ARCHS := $(patsubst %,$(BUILD_DIR)/%/$(ARCH),$(SUBDIRS))
HEADERS := $(shell find . $(EXTRA_HEADER_DIRS) $(SUBDIRS) -maxdepth 1  -name \*.h)

# compiling files from CWD
SRCS := _startc.c
OBJS := $(patsubst $(TOPDIR)/%,$(BUILD_DIR)/%,$(abspath $(SRCS:.c=.o)))

SYSROOT 		:= $(shell $(CC) $(CFLAGS) -print-sysroot)
MULTILIB_DIR 	:= $(shell $(CC) $(CFLAGS) -print-multi-directory)
LIBC_INSTALL_DIR := $(SYSROOT)/lib/$(MULTILIB_DIR)
LIBC_INSTALL_NAMES := libc.a libm.a crt0.o libg.a
HEADERS_INSTALL_DIR := $(SYSROOT)/usr/include/
ifeq (/,$(SYSROOT))
$(error Sysroot is not supported by Your toolchain. Use cross-toolchain to compile)
endif

export TOPDIR BUILD_DIR SIL TARGET LIB CC CFLAGS MKDEP MKDEPFLAGS AR ARFLAGS LD LDFLAGS GCCLIB OBJDUMP STRIP HEADERS_INSTALL_DIR


all: subsystems $(OBJS) $(LIB) tests

lib: $(LIB)

$(OBJS): $(filter clean,$(MAKECMDGOALS))

subsystems: $(ARCHS)

$(ARCHS): %.a: .FORCE $(filter clean,$(MAKECMDGOALS))
	@+echo "\033[1;32mCOMPILE $(subst $(BUILD_DIR)/,,$(@D))\033[0m";\
	mkdir -p $(@D); \
	$(MAKE) -C "$(subst $(BUILD_DIR)/,,$(@D))" ARCH="$@"

.FORCE:

$(LIB): $(ARCHS) $(OBJS)
	@echo "\033[1;34mLD $@\033[0m"

	@rm -rf "$@"
	$(SIL)$(AR) cqT -o $@ $(abspath $^) && echo "create $@\naddlib $@\nsave\nend" | $(AR) -M

	@(echo "";\
	echo "=> libphoenix for [$(TARGET)] has been created";\
	echo "")


tests: test


test: $(LIB) .FORCE
	@echo "\033[1;32mCOMPILE $@\033[0m";\
	$(MAKE) -C "$@"


install: $(LIB) $(HEADERS)
	@echo "Installing into: $(LIBC_INSTALL_DIR)"; \
	mkdir -p "$(LIBC_INSTALL_DIR)" "$(HEADERS_INSTALL_DIR)"; \
	cp -a "$<" "$(LIBC_INSTALL_DIR)"; \
	for lib in $(LIBC_INSTALL_NAMES); do \
		if [ ! -e "$(LIBC_INSTALL_DIR)/$$lib" ]; then \
			ln -sf "$(LIBC_INSTALL_DIR)/$(LIBNAME)" "$(LIBC_INSTALL_DIR)/$$lib"; \
		fi \
	done; \
	for file in $(HEADERS); do\
		install -p -m 644 -D $${file} $(HEADERS_INSTALL_DIR)/$${file};\
	done

uninstall:
	rm -rf "$(LIBC_INSTALL_DIR)/$(LIBNAME)"
	@for lib in $(LIBC_INSTALL_NAMES); do \
		rm -rf "$(LIBC_INSTALL_DIR)/$$lib"; \
	done
	@for file in $(HEADERS); do \
		rm -rf "$(HEADERS_INSTALL_DIR)/$${file}"; \
	done

clean:
	@rm -rf $(BUILD_DIR)


# include after all dependencies are set
include $(TOPDIR)/Makefile.rules

.PHONY: clean install uninstall
# DO NOT DELETE
