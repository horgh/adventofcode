include ../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 3 9 > got
	echo 5 > expected
	diff -u got expected
	@#
	./1 348 2017 > got
	echo 417 > expected
	diff -u got expected
	@#
	./2 3 9 > got
	echo 9 > expected
	diff -u got expected
	@#
	./2 3 8 > got
	echo 5 > expected
	diff -u got expected
	@#
	./2 348 50000000 > got
	echo 34334221 > expected
	diff -u got expected
	@#
	rm got expected
