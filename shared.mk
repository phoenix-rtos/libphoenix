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

# During toolchain build shared toolchain libs are not yet ready.
ifneq ($(TOOLCHAIN_BUILD),y)

install-shared: rootfs-install-shared-toolchain-libs install-shared-toolchain-libs

TOOLCHAIN_LIBS_PATH := $(shell $(CC) --print-sysroot)/lib/$(MULTILIB_DIR)

rootfs-install-shared-toolchain-libs install-shared-toolchain-libs: TOOLCHAIN_LIBS_PATH := $(TOOLCHAIN_LIBS_PATH)


# libstdc++ version differs depending on compilers version.
LIBSTDCXX=$(shell find $(TOOLCHAIN_LIBS_PATH) -regex '.*/libstdc\+\+\.so\.[0-9]+\.[0-9]+' -exec basename {} \; | head -n 1)
LIBSTDCXX_MAJOR=$(shell echo $(LIBSTDCXX) | sed "s/\.[0-9]\+$//")


rootfs-install-shared-toolchain-libs: $(PREFIX_ROOTFS)$(LOCAL_INSTALL_PATH)/$(REALNAME)
	$(SIL)cp $(TOOLCHAIN_LIBS_PATH)/$(LIBSTDCXX) $(PREFIX_ROOTFS)$(LOCAL_INSTALL_PATH)
	$(SIL)cp $(TOOLCHAIN_LIBS_PATH)/libgcc_s.so.1 $(PREFIX_ROOTFS)$(LOCAL_INSTALL_PATH)


install-shared-toolchain-libs: install-shared-libs
	$(SIL)ln -sf $(TOOLCHAIN_LIBS_PATH)/$(LIBSTDCXX) $(LIBC_INSTALL_DIR)
	$(SIL)ln -sf $(TOOLCHAIN_LIBS_PATH)/libgcc_s.so.1 $(LIBC_INSTALL_DIR)
	$(SIL)(cd $(LIBC_INSTALL_DIR) && \
		ln -sf $(LIBSTDCXX) $(LIBSTDCXX_MAJOR) && ln -sf $(LIBSTDCXX) libstdc++.so && \
		ln -sf libgcc_s.so.1 libgcc_s.so \
	)

endif
