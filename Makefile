CFLAGS=-g -Wall
CC=gcc
SRCS=tokenizer.c
OBJS=tokenizer.o
LDFLAGS=-g
LIBS=

all:reina-shell

$(SRCS):
	$(CC) $(CFLAGS) -c $*.c

reina-shell: $(OBJS)
	$(CC) $(LDFLAGS) $(LIBS) reina.c -o reina-shell $(OBJS)

clean:
	rm -f *.o token-shell reina-shell
