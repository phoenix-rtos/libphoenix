### simple utility/shortcut functions ###


# quick template for adding tests (makes xxx binary from xxx.c with optional libs/dependencies)
# $(eval $(call add_test, NAME [, LIBS][, DEP_LIBS]))
define add_test
NAME := $(1)
LOCAL_SRCS := $(1).c
LIBS := $(2)
DEP_LIBS := $(3)
include $(binary.mk)
endef
