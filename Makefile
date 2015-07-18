CC = gcc
CFLAGS = -lrt -O0 -g -Wall -Wextra -Werror
LDFLAGS = $(CFLAGS)

.PHONY: all clean
all: vqueue.o vqueue_test
clean:
	rm -f *.o vqueue_test

vqueue_test: vqueue_test.o vqueue.o
