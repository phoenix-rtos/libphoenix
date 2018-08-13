#
# Makefile for libphoenix (test)
#
# Copyright 2017 Phoenix Systems
# Author: Pawel Pisarczyk
#

PROGS := posixsrv
# additional sources to be linked into program
PROGS_SRCS := pipe.c posixsrv.c pty.c special.c event.c

LDLIBS := -ltty

# include after all dependencies are set
include $(TOPDIR)/Makefile.rules
