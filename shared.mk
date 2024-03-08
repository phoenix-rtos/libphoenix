#
# Makefile for compiling libphoenix as shared lib
#
# Copyright 2024 Phoenix Systems
#
# %LICENSE%
#

# FIXME: In shared versions each library should only have needed parts of libphoenix.
LIBC_INSTALL_NAMES_SO := libc.so libm.so libpthread.so libubsan.so

LIB_TARGETS += $(PREFIX_SO)libphoenix.so
INSTALL_TARGETS += install-shared

# Tell dynamic linker to init libphoenix first.
LIBPHOENIX_SO_FLAGS := $(LDFLAGS_PREFIX)-z,initfirst
# Common shared lib flags
SHARED_LIB_LD_FLAGS := $(TARGET_PIC_FLAG) -shared -nolibc -nostartfiles $(LDFLAGS_PREFIX)--warn-shared-textrel

$(PREFIX_SO)libphoenix.so: LDFLAGS:=$(LDFLAGS) $(SHARED_LIB_LD_FLAGS)

$(PREFIX_SO)libphoenix.so: $(OBJS)
	$(LINK)


LOCAL_INSTALL_PATH := $(or $(LOCAL_INSTALL_PATH),$(DEFAULT_INSTALL_PATH_SO))

install-shared: $(PREFIX_SO)libphoenix.so $(PREFIX_ROOTFS)$(LOCAL_INSTALL_PATH)/libphoenix.so install-shared-libs

$(PREFIX_ROOTFS)$(LOCAL_INSTALL_PATH)/libphoenix.so: $(PREFIX_SO)libphoenix.so
	$(INSTALL_FS)
	$(SIL)(cd $(PREFIX_ROOTFS)$(LOCAL_INSTALL_PATH) && \
		for lib in $(LIBC_INSTALL_NAMES_SO); do \
			if [ ! -e "$$lib" ]; then \
				cp "libphoenix.so" "$$lib"; \
			fi \
		done \
	)

install-shared-libs: $(PREFIX_SO)libphoenix.so install-libs
	$(SIL)(cd $(LIBC_INSTALL_DIR) && \
		for lib in $(LIBC_INSTALL_NAMES_SO); do \
			if [ ! -e "$$lib" ]; then \
				ln -sf "libphoenix.so" "$$lib"; \
			fi \
		done \
	)
