include ../Makefile.mk

.PHONY: test

CFLAGS+=-I../lib

../lib/siphash24.o:
	$(CC) -std=c11 -g -ggdb -c -o ../lib/siphash24.o ../lib/siphash24.c

day04-1: day04-1.c \
	../lib/map.c ../lib/map.h \
	../lib/siphash24.o
	$(CC) $(CFLAGS) -o $@ $< ../lib/map.c ../lib/siphash24.o

day04-2: day04-2.c \
	../lib/map.c ../lib/map.h \
	../lib/siphash24.o
	$(CC) $(CFLAGS) -o $@ $< ../lib/map.c ../lib/siphash24.o

test: day04-1 day04-2
	./day04-1 < input-sample-1 > got
	echo 2 > expected
	diff -u got expected
	rm got expected
	@#
	./day04-1 < input > got
	echo 386 > expected
	diff -u got expected
	rm got expected
	@#
	./day04-2 < input-sample-2 > got
	echo 3 > expected
	diff -u got expected
	rm got expected
