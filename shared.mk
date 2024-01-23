#
# Makefile for compiling libphoenix as shared lib
#
# Copyright 2024 Phoenix Systems
#
# %LICENSE%
#

# FIXME: In shared versions each library should only have needed parts of libphoenix.
LIBC_INSTALL_NAMES_SO := libc.so libm.so libpthread.so libubsan.so

# TODO: automatically get the version
VERSION=3.2.1
MAJOR_VERSION=$(word 1, $(subst ., ,$(VERSION)))

LIBNAME=libphoenix.so
SONAME=$(LIBNAME).$(MAJOR_VERSION)
REALNAME=$(LIBNAME).$(VERSION)

LIB_TARGETS += $(PREFIX_SO)$(REALNAME)
INSTALL_TARGETS += install-shared


SHARED_LDFLAGS := -nolibc
SHARED_LDFLAGS += -z initfirst               # Tell dynamic linker to init libphoenix first.
SHARED_LDFLAGS += -static-libgcc             # Use static libgcc to prevent each program from including it separately.
SHARED_LDFLAGS += $(TARGET_PIC_FLAG) -shared # Common shared lib flags
SHARED_LDFLAGS += -z text                    # Tell linker to abort if .text contains relocations
SHARED_LDFLAGS += $(LDFLAGS_PREFIX)-soname,$(SONAME)


$(PREFIX_SO)$(REALNAME): LDFLAGS:=$(LDFLAGS) $(SHARED_LDFLAGS)

$(PREFIX_SO)$(REALNAME): $(OBJS)
	$(LINK)


LOCAL_INSTALL_PATH := $(or $(LOCAL_INSTALL_PATH),$(DEFAULT_INSTALL_PATH_SO))

install-shared: $(PREFIX_SO)$(REALNAME) $(PREFIX_ROOTFS)$(LOCAL_INSTALL_PATH)/$(REALNAME) install-shared-libs

$(PREFIX_ROOTFS)$(LOCAL_INSTALL_PATH)/$(REALNAME): $(PREFIX_SO)$(REALNAME)
	$(INSTALL_FS)

install-shared-libs: $(PREFIX_SO)$(REALNAME) install-libs
	$(SIL)(cd $(LIBC_INSTALL_DIR) && \
		for lib in $(LIBC_INSTALL_NAMES_SO); do \
			if [ ! -e "$$lib.$(VERSION)" ]; then \
				ln -sf "$(REALNAME)" "$$lib.$(VERSION)"; \
			fi \
		done && \
		for lib in $(LIBC_INSTALL_NAMES_SO) $(LIBNAME); do \
			if [ ! -e "$$lib.$(MAJOR_VERSION)" ]; then \
				ln -sf "$$lib.$(VERSION)" "$$lib.$(MAJOR_VERSION)"; \
			fi \
		done && \
		for lib in $(LIBC_INSTALL_NAMES_SO) $(LIBNAME); do \
			if [ ! -e "$$lib" ]; then \
				ln -sf "$$lib.$(MAJOR_VERSION)" "$$lib"; \
			fi \
		done \
	)
