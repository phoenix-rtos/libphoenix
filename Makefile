#
# Makefile for libphoenix
#
# Copyright 2017, 2018 Phoenix Systems
#
# %LICENSE%
#

SIL ?= @
MAKEFLAGS += --no-print-directory --output-sync

TARGET ?= arm-imx6ull
#TARGET ?= armv7-stm32-tiramisu
#TARGET ?= ia32-qemu
#TARGET ?= riscv64

VERSION = 0.2
TOPDIR := $(CURDIR)
PREFIX_BUILD ?= ../build/$(TARGET)
PREFIX_BUILD := $(abspath $(PREFIX_BUILD))
BUILD_DIR ?= $(PREFIX_BUILD)/$(notdir $(TOPDIR))
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

SYSROOT := $(shell $(CC) $(CFLAGS) -print-sysroot)
MULTILIB_DIR := $(shell $(CC) $(CFLAGS) -print-multi-directory)
LIBC_INSTALL_DIR := $(SYSROOT)/lib/$(MULTILIB_DIR)
LIBC_INSTALL_NAMES := libc.a libm.a crt0.o libg.a
HEADERS_INSTALL_DIR := $(SYSROOT)/usr/include/
ifeq (/,$(SYSROOT))
$(error Sysroot is not supported by Your toolchain. Use cross-toolchain to compile)
endif

export TOPDIR PREFIX_BUILD BUILD_DIR SIL TARGET LIB CC CFLAGS AR ARFLAGS LD LDFLAGS GCCLIB OBJDUMP STRIP HEADERS_INSTALL_DIR


all: subsystems $(OBJS) $(LIB) tests

lib: $(LIB)

$(OBJS): $(filter clean,$(MAKECMDGOALS))

subsystems: $(ARCHS)

$(ARCHS): %.a: .FORCE $(filter clean,$(MAKECMDGOALS))
	@mkdir -p $(@D); \
	$(MAKE) -C "$(subst $(BUILD_DIR)/,,$(@D))" ARCH="$@"

.FORCE:

$(LIB): $(ARCHS) $(OBJS)
	@echo "\033[1;34mLD $@\033[0m"
	
	@(\
	printf "Subsystem                  | text    | rodata  | data\n";\
	printf "=========================================================\n";\
	for f in $(ARCHS) $(OBJS); do\
	 	datasz=0;\
		textsz=0;\
		rodatasz=0;\
		file=$$f;\
		for i in `$(OBJDUMP) -t $$file | grep -e " O " | grep -v ".rodata" | awk '{ print $$1 }'`; do\
			datasz=`echo $$(($$datasz + 0x$$i))`;\
		done;\
		for i in `$(OBJDUMP) -t $$file | grep -e " O " | grep ".rodata" | awk '{ print $$1 }'`; do \
			rodatasz=`echo $$(($$rodatasz + 0x$$i))`;\
		done; \
		for i in `$(OBJDUMP) -t $$file | grep -e " F " | awk '{ print $$5 }'`; do \
			textsz=`echo $$(($$textsz + 0x$$i))`;\
		done;\
		n=`dirname $$f`;\
		n=`basename $$n | sed "s/libphoenix/./"`;\
		f=`basename $$f`;\
		printf "%-26s | %-7d | %-7d | %-7d\n" $$n/$$f $$textsz $$rodatasz $$datasz;\
	done;)

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
		base=`dirname $${file}`; \
		mkdir -p "$(HEADERS_INSTALL_DIR)/$${base}"; \
		install -p -m 644 $${file} $(HEADERS_INSTALL_DIR)/$${file};\
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
