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
OBJS := $(patsubst $(TOPDIR)/%,$(BUILD_DIR)/%,$(abspath $(SRCS:.c=.o)))
PROGS_OUT := $(patsubst $(TOPDIR)/%,$(BUILD_DIR)/%,$(abspath $(PROGS)))
PROGS_OUT_UNSTRIPPED := $(PROGS_OUT:=_unstripped)

all: $(PROGS_OUT)

$(PROGS_OUT_UNSTRIPPED): $(OBJS)

# include after all dependencies are set
include $(TOPDIR)/Makefile.rules
