#
# Makefile for libphoenix (test)
#
# Copyright 2017 Phoenix Systems
# Author: Pawel Pisarczyk
#
ifeq (,$(SRCDIR))
$(error Only main Makefile can be used for compilation)
endif

PROGS := posixsrv
SRCS  := $(PROGS:=.c)

-include .depend

all: check $(PROGS)

# include after all dependencies are set
include $(SRCDIR)/Makefile.rules
