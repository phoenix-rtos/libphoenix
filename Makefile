#
#

CC = i386-pc-phoenix-gcc
CFLAGS = -c -Wall -I. -I../../libphoenix -O2 \
  -nostartfiles -nostdlib -fomit-frame-pointer -fno-strength-reduce -ffreestanding

AR = i386-pc-phoenix-ar

SRCS = _start.c process.c printf.c memcpy.c memset.c strcmp.c strcpy.c strlen.c \
  strtol.c strtoul.c syscall.c msg.c mutex.c thread.c time.c malloc.c list.c

OBJS = $(SRCS:.c=.o)


all: libphoenix

.c.o:
	$(CC) $(CFLAGS) $<

libphoenix: $(OBJS)
	$(AR) rc libphoenix.a $(OBJS)

clean:
	rm -f *.o *~ core
