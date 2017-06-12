#
#

CC = i386-pc-phoenix-gcc
CFLAGS = -c -Wall -I. -I../../libphoenix -O2 -g

AR = i386-pc-phoenix-ar

SRCS = _start.c process.c memcpy.c memset.c strcmp.c strcpy.c strlen.c \
  strtol.c strtoul.c syscall.c

OBJS = $(SRCS:.c=.o)


all: libphoenix

.c.o:
	$(CC) $(CFLAGS) $<

libphoenix: $(OBJS)
	$(AR) rc libphoenix.a $(OBJS)

clean:
	rm -f *.o *~ core
