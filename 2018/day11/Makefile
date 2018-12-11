include ../../problems.mk

CFLAGS+=-O3

test: $(TARGETS)
	./1 18 > got
	echo 33,45 > expected
	diff -u got expected
	@#
	./1 42 > got
	echo 21,61 > expected
	diff -u got expected
	@#
	./1 9995 > got
	echo 33,45 > expected
	diff -u got expected
	@#
	./2 18 > got
	echo 90,269,16 > expected
	diff -u got expected
	@#
	./2 42 > got
	echo 232,251,12 > expected
	diff -u got expected
	@#
	./2 9995 > got
	echo 233,116,15 > expected
	diff -u got expected
	@#
	rm got expected
