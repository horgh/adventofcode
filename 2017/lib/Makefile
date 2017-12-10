include ../Makefile.mk

siphash.o: siphash.c siphash.h
	$(CC) -c -o $@ $<

map: map.c map.h siphash.o
	$(CC) $(CFLAGS) -DTEST_MAP -o $@ $< siphash.o

queue: queue.c queue.h
	$(CC) $(CFLAGS) -DTEST_QUEUE -o $@ $<
