#
# Makefile for libphoenix
#
# Copyright 2017 Phoenix Systems
#
# %LICENSE%
#

SIL ?= @

#TARGET = armv7-stm32-tiramisu
TARGET = ia32-qemu

VERSION = 0.2
SRCDIR := $(CURDIR)

SUBSYSTEMS = math stdio stdlib string sys ctype

LIB = libphoenix.a


############ IA32 ############

ifneq (, $(findstring ia32, $(TARGET)))
	CROSS ?= i386-pc-phoenix-
	SUBDIRS = arch/ia32  $(SUBSYSTEMS)

	MKDEP = $(CROSS)gcc -MM
	MKDEPFLAGS = $(CFLAGS)

	CC = $(CROSS)gcc

	CFLAGS += -Wall -Wstrict-prototypes -I$(SRCDIR) -nostartfiles -nostdlib\
		-m32 -mtune=generic -mno-mmx -mno-sse\
		-fomit-frame-pointer -fno-strength-reduce -ffreestanding\
		-DVERSION=\"$(VERSION)\" -DARCH=\"arch/ia32/arch.h\"

	AR = $(CROSS)ar
	ARFLAGS = -r

	LD = $(CROSS)ld
	LDFLAGS = -m elf_i386
	GCCLIB := $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

	OBJCOPY = $(CROSS)objcopy
	OBJDUMP = $(CROSS)objdump
endif

############ ARMv7 (Cortex-M3/M4) ############

ifneq (, $(findstring armv7, $(TARGET)))
	CROSS ?= arm-phoenix-
	SUBDIRS = arch/armv7 $(SUBSYSTEMS)
	
	MKDEP = $(CROSS)gcc -MM
	MKDEPFLAGS = $(CFLAGS)

	CC = $(CROSS)gcc

	CFLAGS += -Wall -Wstrict-prototypes -I$(SRCDIR) -nostartfiles -nostdlib\
		-mcpu=cortex-m3 -mthumb \
		-fomit-frame-pointer -ffreestanding\
		-fpic -fpie -msingle-pic-base\
		-DVERSION=\"$(VERSION)\" -DARCH=\"arch/armv7/arch.h\" -DNOMMU

	AR = $(CROSS)ar
	ARFLAGS = -r

	LD = $(CROSS)ld
	LDFLAGS = -nostdlib -e _start --section-start .init=8000000 -Tbss=20000000 -z max-page-size=0x10
	GCCLIB := $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

	OBJCOPY = $(CROSS)objcopy
	OBJDUMP = $(CROSS)objdump
endif


ARCH = code.a
ARCHS := $(shell for i in $(SUBDIRS); do echo "$$i/$(ARCH)"; done)

export SIL TARGET LIB CC CFLAGS MKDEP MKDEPFLAGS AR ARFLAGS LD LDFLAGS GCCLIB ARCH OBJDUMP


all: subsystems $(OBJS) $(LIB) tests


.c.o:
	@(printf "CC  %-24s  " "$<"; $(CC) -c $(CFLAGS) $<)

	@(file=`echo $< | sed "s/\.c/\.o/"`; \
	datasz=0;\
	textsz=0;\
	for i in `$(OBJDUMP) -t $$file | grep -e " O " | awk '{ print $$4 }'`; do \
		datasz=`echo $$(($$datasz + 0x$$i))`;\
	done; \
	for i in `$(OBJDUMP) -t $$file | grep -e "F" | awk '{ print $$5 }'`; do \
		textsz=`echo $$(($$textsz + 0x$$i))`;\
	done;\
	echo "data=$$datasz\t text=$$textsz")


subsystems:
	@for i in $(SUBDIRS); do\
		d=`pwd`;\
		echo "\033[1;32mCOMPILE $$i\033[0m";\
		if ! cd $$i; then\
			exit 1;\
		fi;\
		if ! make; then\
			exit 1;\
		fi;\
		cd $$d;\
	done;


$(LIB): $(ARCHS) $(OBJS)
	@echo "\033[1;34mLD $@\033[0m"
	
	$(AR) $(ARFLAGS) -o $(LIB) $(OBJS) $(shell for i in $(SUBDIRS); do k=`echo $$i | sed 's/\//\\\\\//g'`; $(AR) -t $$i/$(ARCH) | sed "s/^/$$k\//"; done;)

	@(echo "";\
	echo "=> libphoenix for [$(TARGET)] has been created";\
	echo "")
	

tests:
	@d=`pwd`;\
	echo "\033[1;32mCOMPILE test\033[0m";\
	if ! cd test; then\
		exit 1;\
	fi;\
	if ! make; then\
		exit 1;\
	fi;\
	cd $$d;\


depend:
	@for i in $(SUBDIRS) test; do\
		d=`pwd`;\
		echo "DEPEND $$i";\
		if ! cd $$i; then\
			exit 1;\
		fi;\
		if ! make -s depend; then\
			exit 1;\
		fi;\
		cd $$d;\
	done;


clean:
	@rm -f core *.o $(LIB)
	@rm -f ../*.elf ../*.img ../*.flash ../*.cmd
	@for i in $(SUBDIRS) test; do\
		d=`pwd`;\
		echo "CLEAN $$i";\
		if ! cd $$i; then\
			exit 1;\
		fi;\
		if ! make clean; then\
			exit 1;\
		fi;\
		cd $$d;\
	done;


.PHONY: clean
# DO NOT DELETE
