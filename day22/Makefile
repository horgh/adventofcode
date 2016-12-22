include ../Makefile.mk

CFLAGS+=-I../lib

problem22-1: problem22-1.c ../lib/queue.c ../lib/queue.h \
	../lib/map.c ../lib/map.h ../lib/siphash24.c ../lib/siphash24.h
	$(CC) $(CFLAGS) -o $@ $< ../lib/queue.c ../lib/map.c ../lib/siphash24.c
