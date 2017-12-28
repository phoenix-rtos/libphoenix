#
# Makefile for libphoenix (test)
#
# Copyright 2017 Phoenix Systems
# Author: Pawel Pisarczyk
#

SRCS = test_malloc.c
OBJS = $(SRCS:.c=.o)


all: check test_mmap test_malloc test_threads test_str2num test_scanf psh


check:
	@(if test -z "$(ARCH)"; then\
		echo "Error: Only main Makefile can be used for compilation!";\
		exit 1;\
	fi;)


.c.o:
	@(printf " CC  %-24s  " "$<"; $(CC) -c $(CFLAGS) $<)

	@(file=`echo $< | sed "s/\.c/\.o/"`; \
	datasz=0;\
	textsz=0;\
	rodatasz=0;\
	for i in `$(OBJDUMP) -t $$file | grep -e " O " | awk '{ print $$4 }'`; do \
		datasz=`echo $$(($$datasz + 0x$$i))`;\
	done; \
	for i in `$(OBJDUMP) -t $$file | grep -e " O " | grep ".rodata" | awk '{ print $$5 }'`; do \
                rodatasz=`echo $$(($$rodatasz + 0x$$i))`;\
        done; \
	for i in `$(OBJDUMP) -t $$file | grep -e "F" | awk '{ print $$5 }'`; do \
		textsz=`echo $$(($$textsz + 0x$$i))`;\
	done;\
	echo "data=$$datasz\t text=$$textsz\t rodata=$$rodatasz")


-include .depend


test_mmap: test_mmap.o
	$(SIL)$(LD) $(LDFLAGS) -o test_mmap test_mmap.o ../$(LIB) $(GCCLIB)


test_malloc: test_malloc.o
	$(SIL)$(LD) $(LDFLAGS) -o test_malloc test_malloc.o ../$(LIB) $(GCCLIB)


test_threads: test_threads.o
	$(SIL)$(LD) $(LDFLAGS) -o test_threads test_threads.o ../$(LIB) $(GCCLIB)


test_str2num: test_str2num.o
	$(SIL)$(LD) $(LDFLAGS) -o test_str2num test_str2num.o ../$(LIB) $(GCCLIB)


test_scanf: test_scanf.o
	$(SIL)$(LD) $(LDFLAGS) -o test_scanf test_scanf.o ../$(LIB) $(GCCLIB)


psh: psh.o
	$(SIL)$(LD) $(LDFLAGS) -o psh psh.o ../$(LIB) $(GCCLIB)


depend: check
	$(SIL)$(MKDEP) $(MKDEPFLAGS) $(SRCS) >.depend


clean: check
	$(SIL)rm -f core *.o test_mmap test_malloc test_scanf


.PHONY: clean
# DO NOT DELETE
