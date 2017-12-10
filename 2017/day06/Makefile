include ../Makefile.mk

.PHONY: test

CFLAGS+=-I../lib

../lib/siphash.o:
	$(CC) -std=c11 -g -ggdb -c -o ../lib/siphash.o ../lib/siphash.c

day06-1: day06-1.c \
	../lib/map.c ../lib/map.h \
	../lib/siphash.o
	$(CC) $(CFLAGS) -o $@ $< ../lib/map.c ../lib/siphash.o

day06-2: day06-2.c \
	../lib/map.c ../lib/map.h \
	../lib/siphash.o
	$(CC) $(CFLAGS) -o $@ $< ../lib/map.c ../lib/siphash.o

test: $(TARGETS)
	./day06-1 < input-sample-1 > got
	echo 5 > expected
	diff -u got expected
	@#
	./day06-1 < input > got
	echo 6681 > expected
	diff -u got expected
	@#
	./day06-2 < input-sample-1 > got
	echo 4 > expected
	diff -u got expected
	@#
	./day06-2 < input > got
	echo 2392 > expected
	diff -u got expected
	@#
	rm got expected
