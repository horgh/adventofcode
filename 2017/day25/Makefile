include ../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 12667664 > got
	echo 4769 > expected
	diff -u got expected
	rm got expected
