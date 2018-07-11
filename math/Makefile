#
# Makefile for Phoenix-RTOS kernel (test)
#
# Copyright 2012-2015 Phoenix Systems
# Copyright 2001, 2005-2006 Pawel Pisarczyk
#
ifeq (,$(TOPDIR))
$(error Only main Makefile can be used for compilation)
endif

SRCS = trig.c power.c exp.c common.c #hyper.c
OBJS := $(patsubst $(TOPDIR)/%,$(BUILD_DIR)/%,$(abspath $(SRCS:.c=.o)))

all: $(ARCH)

$(ARCH): $(OBJS)

# include after all dependencies are set
include $(TOPDIR)/Makefile.rules
