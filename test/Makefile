#
# Makefile for libphoenix (test)
#
# Copyright 2017 Phoenix Systems
# Author: Pawel Pisarczyk
#

# install tests in separate directory
PREFIX_PROG := $(PREFIX_BUILD)/prog/
PREFIX_PROG_STRIPPED := $(PREFIX_BUILD)/prog.stripped/

PROGS := test_mmap test_malloc test_threads test_str2num test_scanf test_msg\
   		test_env test_signals test_condwait test_mktime test_gmtime test_strftime\
		test_memmove

# include after all dependencies are set
include $(TOPDIR)/Makefile.rules
