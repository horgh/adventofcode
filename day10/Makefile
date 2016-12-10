include ../Makefile.mk

CFLAGS+=-I../lib

problem10-1: problem10-1.c ../lib/map.c ../lib/map.h
	$(CC) $(CFLAGS) -I../lib -o $@ $< ../lib/map.c
