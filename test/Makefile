#
# Makefile for libphoenix (test)
#
# Copyright 2017 Phoenix Systems
# Author: Pawel Pisarczyk
#
ifeq (,$(TOPDIR))
$(error Only main Makefile can be used for compilation)
endif

PROGS := test_mmap test_malloc test_threads test_str2num test_scanf test_msg test_env test_signals test_condwait psh
PROGS_OUT := $(patsubst $(TOPDIR)/%,$(BUILD_DIR)/%,$(abspath $(PROGS)))

# prevent makefile thiking these are intermediate targets
OBJS := $(patsubst $(TOPDIR)/%,$(BUILD_DIR)/%,$(abspath $(PROGS:=.o)))

all: $(PROGS_OUT) $(OBJS)

# include after all dependencies are set
include $(TOPDIR)/Makefile.rules
