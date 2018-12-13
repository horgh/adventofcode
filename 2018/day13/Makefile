include ../../problems.mk

CFLAGS+=-O3

test: $(TARGETS)
	./1 < input-sample-0 > got
	echo 0,3 > expected
	diff -u got expected
	@#
	./1 < input-sample-1 > got
	echo 7,3 > expected
	diff -u got expected
	@#
	./1 < input > got
	echo 5,102 > expected
	diff -u got expected
	@#
	./2 < input > got
	echo 46,45 > expected
	diff -u got expected
	@#
	rm got expected
