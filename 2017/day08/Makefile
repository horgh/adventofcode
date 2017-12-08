include ../Makefile.mk

.PHONY: test

CFLAGS+=-I../lib

../lib/siphash24.o:
	$(CC) -std=c11 -g -ggdb -c -o ../lib/siphash24.o ../lib/siphash24.c

day08-1: day08-1.c \
	../lib/map.c ../lib/map.h \
	../lib/siphash24.o
	$(CC) $(CFLAGS) -o $@ $< ../lib/map.c ../lib/siphash24.o

day08-2: day08-2.c \
	../lib/map.c ../lib/map.h \
	../lib/siphash24.o
	$(CC) $(CFLAGS) -o $@ $< ../lib/map.c ../lib/siphash24.o

test: $(TARGETS)
	./day08-1 < input-sample-1 > got
	echo 1 > expected
	diff -u got expected
	@#
	./day08-1 < input > got
	echo 3612 > expected
	diff -u got expected
	@#
	./day08-2 < input-sample-1 > got
	echo 10 > expected
	diff -u got expected
	@#
	./day08-2 < input > got
	echo 3818 > expected
	diff -u got expected
	@#
	rm got expected
