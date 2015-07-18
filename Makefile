CC = gcc
CFLAGS = -std=gnu99 -O0 -g -Wall -Wextra -Werror -DNDEBUG
LDFLAGS = $(CFLAGS)
LDLIBS = -lrt -lc

.PHONY: all clean
all: vqueue.o vqueue_test
clean:
	rm -f *.o vqueue_test

vqueue_test: vqueue_test.o vqueue.o
