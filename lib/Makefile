include ../Makefile.mk

.PHONY: test

all: hash libadvent.a map queue

clean:
	rm -f hash libadvent.a map queue *.o

hash: hash.c hash.c
	$(CC) $(CFLAGS) -DTEST_HASH -o $@ $< -lssl -lcrypto

libadvent.a: hash.o map.o queue.o siphash.o
	rm -f $@
	ar r $@ hash.o map.o queue.o siphash.o
	ranlib $@

map: map.c map.h siphash.o
	$(CC) $(CFLAGS) -DTEST_MAP -o $@ $< siphash.o

queue: queue.c queue.h
	$(CC) $(CFLAGS) -DTEST_QUEUE -o $@ $<

siphash.o: siphash.c siphash.h
	$(CC) -c -o $@ $<

test: hash map queue
	./hash
	./map
	./queue
