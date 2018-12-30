include ../Makefile.mk

.PHONY: test

all: hash libadvent.a map queue util

clean:
	rm -f hash libadvent.a map queue *.o util

hash: hash.c hash.c
	$(CC) $(CFLAGS) -DTEST_HASH -o $@ $< -lssl -lcrypto

libadvent.a: a_star.o hash.o map.o queue.o siphash.o util.o
	rm -f $@
	ar r $@ a_star.o hash.o map.o queue.o siphash.o util.o
	ranlib $@

map: map.c map.h siphash.o
	$(CC) $(CFLAGS) -DTEST_MAP -o $@ $< siphash.o

queue: queue.c queue.h
	$(CC) $(CFLAGS) -DTEST_QUEUE -o $@ $<

util: util.c util.h
	$(CC) $(CFLAGS) -DTEST_UTIL -o $@ $<

siphash.o: siphash.c siphash.h
	$(CC) -c -o $@ $<

test: hash map queue util
	./hash
	./map
	./queue
	./util
