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
	./2 9995 > got
	echo 233,116,15 > expected
	diff -u got expected
	@#
	rm got expected
