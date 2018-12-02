include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 < input-sample-0 > got
	echo 2 > expected
	diff -u got expected
	./1 < input-sample-1 > got
	echo 4 > expected
	diff -u got expected
	./1 < input-sample-2 > got
	echo 2 > expected
	diff -u got expected
	./1 < input > got
	echo 2565 > expected
	diff -u got expected
	./2 < input-sample-3 > got
	echo 3 > expected
	diff -u got expected
	./2 < input-sample-1 > got
	echo 3 > expected
	diff -u got expected
	./2 < input-sample-2 > got
	echo 11 > expected
	diff -u got expected
	./2 < input > got
	echo 2639 > expected
	diff -u got expected
	rm got expected
