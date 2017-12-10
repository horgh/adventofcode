include ../Makefile.mk

.PHONY: test

CFLAGS+=-I../lib

../lib/siphash.o:
	$(CC) -std=c11 -g -ggdb -c -o ../lib/siphash.o ../lib/siphash24.c

day07-1: day07-1.c \
	../lib/map.c ../lib/map.h \
	../lib/siphash.o
	$(CC) $(CFLAGS) -o $@ $< ../lib/map.c ../lib/siphash.o

day07-2: day07-2.c \
	../lib/map.c ../lib/map.h \
	../lib/siphash.o
	$(CC) $(CFLAGS) -o $@ $< ../lib/map.c ../lib/siphash.o

test: $(TARGETS)
	./day07-1 < input-sample-1 > got
	echo tknk > expected
	diff -u got expected
	@#
	./day07-1 < input > got
	echo eqgvf > expected
	diff -u got expected
	@#
	./day07-2 < input-sample-1 > got
	echo 60 > expected
	diff -u got expected
	@#
	./day07-2 < input > got
	echo 757 > expected
	diff -u got expected
	@#
	rm got expected
