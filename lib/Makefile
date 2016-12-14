include ../Makefile.mk

map: map.c map.h siphash24.c siphash24.h
	$(CC) $(CFLAGS) -DTEST_MAP -o $@ map.c siphash24.c
