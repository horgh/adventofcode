include ../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 < input-sample-1 > got
	echo 31 > expected
	diff -u got expected
	@#
	./1 < input > got
	echo 1940 > expected
	diff -u got expected
	@#
	./2 < input-sample-1 > got
	echo 19 > expected
	diff -u got expected
	@#
	./2 < input > got
	echo 1928 > expected
	diff -u got expected
	@#
	rm got expected
