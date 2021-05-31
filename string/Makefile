#
# Makefile for libphoenix/string
#
# Copyright 2017, 2020 Phoenix Systems
# Author: Pawel Pisarczyk
#

INCS := errno.str.inc errno.tab.inc gaierr.str.inc gaierr.tab.inc


ERR_EXTRACT := $(SED) -En -e '/^\s*\#\s*define\s+E\w+\s+[[:digit:]]+/{s/^[^d]*define\s+(\w+)\s+([[:digit:]]+).*/\2\t\1/;p;d}' \
	-e '/^\s*E\w+\s*=\s*-?[[:digit:]]+/{s/^\s*(\w+)\s*=\s*-?([[:digit:]]+).*/\2\t\1/;p;d}'


%/errno.list: $(SYSROOT)/usr/include/phoenix/errno.h include/errno.h
	@(printf "GEN %-24s  \n" "$(@F)")
	$(SIL)$(ERR_EXTRACT) $^ | sort -n > $@


%/gaierr.list: include/netdb.h Makefile
	@(printf "GEN %-24s  \n" "$(@F)")
	$(SIL)$(ERR_EXTRACT) $^ | sort -n > $@


%.str.inc: %.list Makefile
	@(printf "GEN %-24s  \n" "$(@F)")
	$(SIL)$(SED) -e 's/^.*\t\(.*\)$$/"\1\\0"/' $< > $@


%.tab.inc: %.list Makefile
	@(printf "GEN %-24s  \n" "$(@F)")
	$(SIL)bash -c 'o=0; while read num name; do echo "{ $$num, $$o },"; o=$$((o+$${#name}+1)); done' < $< > $@


$(PREFIX_O)string/strerror.o: $(patsubst %,string/%,$(INCS))


OBJS += $(addprefix $(PREFIX_O)string/, string.o strdup.o strerror.o strsignal.o)
