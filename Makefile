#
# Makefile for libphoenix
#
# Copyright 2018, 2019, 2024 Phoenix Systems
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
LIBC_INSTALL_NAMES_A := libc.a libm.a libg.a libpthread.a libubsan.a
HEADERS_INSTALL_DIR := $(SYSROOT)/usr/include

ifeq (,$(filter-out /,$(SYSROOT)))
$(error SYSROOT is not supported by the toolchain. Use cross-toolchain to compile.)
endif

CFLAGS += -Iinclude -fno-builtin-malloc

OBJS :=
# crt0.o should have all necessary initialization + call to main()
CRT0_OBJS := $(PREFIX_O)crt0-common.o


ifeq ($(TARGET_SUBFAMILY),multilib)
  LIBPHOENIX_MULTILIB := y
  CPPFLAGS += -DLIBPHOENIX_MULTILIB
else
  LIBPHOENIX_MULTILIB := n
endif

ifneq (,$(findstring arm,$(TARGET_SUFF)))
  include arch/arm/Makefile
else ifneq (,$(findstring aarch64,$(TARGET_SUFF)))
  include arch/aarch64/Makefile
else
  include arch/$(TARGET_SUFF)/Makefile
endif

include ctype/Makefile
include err/Makefile
include errno/Makefile
include locale/Makefile
include math/Makefile
include misc/Makefile
include net/Makefile
include netinet/Makefile
include posix/Makefile
include phoenix/Makefile
include pthread/Makefile
include regex/Makefile
include rtld/Makefile
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
include ubsan/Makefile


LIB_TARGETS := $(PREFIX_A)libphoenix.a $(PREFIX_A)crt0.o
INSTALL_TARGETS := install-headers install-libs

ifeq ($(LIBPHOENIX_PIC), y)
CFLAGS += $(TARGET_PIC_FLAG)
ifeq ($(LIBPHOENIX_SHARED), y)
include shared.mk
endif
endif


all: $(LIB_TARGETS)


$(PREFIX_A)libphoenix.a: $(OBJS)
	$(ARCH)

$(PREFIX_A)crt0.o: $(CRT0_OBJS)
	$(ARCH)


SRCHEADERS := $(shell find include -name \*.h)


install: $(INSTALL_TARGETS)

install-headers: $(SRCHEADERS)
	@echo INSTALL "$(HEADERS_INSTALL_DIR)/*"; \
	mkdir -p "$(HEADERS_INSTALL_DIR)"; \
	cp -a include/* "$(HEADERS_INSTALL_DIR)";

# TODO: remove `rm crt0.o` when we will be sure it's not a symlink to libphoenix.a anymore
install-libs: $(LIB_TARGETS)
	$(SIL)echo INSTALL "$(LIBC_INSTALL_DIR)/*"; \
	mkdir -p "$(LIBC_INSTALL_DIR)"; \
	rm -rf "$(LIBC_INSTALL_DIR)/crt0.o"; \
	cp -a $^ "$(LIBC_INSTALL_DIR)"; \
	(cd $(LIBC_INSTALL_DIR) && \
		for lib in $(LIBC_INSTALL_NAMES_A); do \
			if [ ! -e "$$lib" ]; then \
				ln -sf "libphoenix.a" "$$lib"; \
			fi \
		done)

.PHONY: clean install install-headers install-libs
clean:
	@echo "rm -rf $(BUILD_DIR)"

ifneq ($(filter clean,$(MAKECMDGOALS)),)
	$(shell rm -rf $(BUILD_DIR))
	$(shell rm -f string/*.inc)
endif
