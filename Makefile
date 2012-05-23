CFLAGS=-g -Wall
CC=gcc
SRCS=tokenizer.c token-shell.c
OBJS=tokenizer.o token-shell.o
LDFLAGS=
LIBS=

all:    token-shell

$(SRCS):
	$(CC) $(CFLAGS) -c $*.c

token-shell: $(OBJS)
	$(CC) $(LDFLAGS) $(LIBS) -o token-shell $(OBJS)

clean:
	rm -f *.o token-shell
