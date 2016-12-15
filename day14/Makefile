include ../Makefile.mk

CFLAGS+=-I../lib -O3

problem14-1: problem14-1.c ../lib/hash.c ../lib/hash.h
	$(CC) $(CFLAGS) -o $@ $< ../lib/hash.c -lssl -lcrypto

problem14-2: problem14-2.c ../lib/hash.c ../lib/hash.h \
	../lib/map.c ../lib/map.h ../lib/siphash24.c ../lib/siphash24.h
	$(CC) $(CFLAGS) -o $@ $< ../lib/hash.c ../lib/map.c ../lib/siphash24.c -lssl \
		-lcrypto
