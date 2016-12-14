include ../Makefile.mk

CFLAGS+=-I../lib

problem11-1: problem11-1.c ../lib/map.c ../lib/map.h \
	../lib/siphash24.c ../lib/siphash24.h ../lib/queue.c ../lib/queue.h
	$(CC) $(CFLAGS) -DMAP_DEBUG -O3 -I../lib -o $@ $< ../lib/map.c \
		../lib/siphash24.c ../lib/queue.c -lm
