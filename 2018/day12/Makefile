include ../../problems.mk

CFLAGS+=-O3

test: $(TARGETS)
	./1 < input-sample > got
	echo 325 > expected
	diff -u got expected
	@#
	./1 < input > got
	echo 3276 > expected
	diff -u got expected
	@#
	./2 < input > got
	echo 3750000001113 > expected
	diff -u got expected
	@#
	rm got expected
