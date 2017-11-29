include ../Makefile.mk

CFLAGS+=-I../lib

problem17-1: problem17-1.c ../lib/map.c ../lib/map.h \
	../lib/siphash24.c ../lib/siphash24.h ../lib/hash.c ../lib/hash.h \
	../lib/queue.c ../lib/queue.h
	$(CC) $(CFLAGS) -o $@ $< ../lib/map.c ../lib/siphash24.c ../lib/hash.c \
		../lib/queue.c -lssl -lcrypto

problem17-2: problem17-2.c ../lib/map.c ../lib/map.h \
	../lib/siphash24.c ../lib/siphash24.h ../lib/hash.c ../lib/hash.h \
	../lib/queue.c ../lib/queue.h
	$(CC) $(CFLAGS) -o $@ $< ../lib/map.c ../lib/siphash24.c ../lib/hash.c \
		../lib/queue.c -lssl -lcrypto
