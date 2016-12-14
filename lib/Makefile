include ../Makefile.mk

map: map.c map.h siphash24.c siphash24.h
	$(CC) $(CFLAGS) -DTEST_MAP -o $@ $< siphash24.c

queue: queue.c queue.h
	$(CC) $(CFLAGS) -DTEST_QUEUE -o $@ $<
