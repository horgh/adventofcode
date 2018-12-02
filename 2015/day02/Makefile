include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 < input-sample > got
	echo 101 > expected
	diff -u got expected
	./1 < input > got
	echo 1586300 > expected
	diff -u got expected
	./2 < input-sample > got
	echo 48 > expected
	diff -u got expected
	./2 < input > got
	echo 3737498 > expected
	diff -u got expected
	rm got expected
