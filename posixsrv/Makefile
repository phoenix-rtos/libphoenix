#
# Makefile for libphoenix (test)
#
# Copyright 2017 Phoenix Systems
# Author: Pawel Pisarczyk
#
ifeq (,$(TOPDIR))
$(error Only main Makefile can be used for compilation)
endif

PROGS := posixsrv
SRCS  := pipe.c posixsrv.c pty.c special.c
OBJS  := $(SRCS:.c=.o)

posixsrv_unstripped: $(OBJS)

all: check $(PROGS) $(OBJS)

# include after all dependencies are set
include $(TOPDIR)/Makefile.rules
