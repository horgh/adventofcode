include ../Makefile.mk

hash: hash.c hash.c
	$(CC) $(CFLAGS) -DTEST_HASH -o $@ $< -lssl -lcrypto

map: map.c map.h siphash.o
	$(CC) $(CFLAGS) -DTEST_MAP -o $@ $< siphash.o

queue: queue.c queue.h
	$(CC) $(CFLAGS) -DTEST_QUEUE -o $@ $<

siphash.o: siphash.c siphash.h
	$(CC) -c -o $@ $<
