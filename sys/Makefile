#
# Makefile for libphoenix (sys)
#
# Copyright 2017 Phoenix Systems
# Author: Pawel Pisarczyk
#
ifeq (,$(TOPDIR))
$(error Only main Makefile can be used for compilation)
endif

SRCS = rb.c stat.c time.c times.c list.c semaphore.c socket.c select.c ioctl.c
OBJS = $(SRCS:.c=.o)

all: $(ARCH)

$(ARCH): $(OBJS)

# include after all dependencies are set
include $(TOPDIR)/Makefile.rules
