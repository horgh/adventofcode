include ../../problems.mk

CFLAGS+=-O3

test: $(TARGETS)
	./1 < input-sample > got
	echo 1147 > expected
	diff -u got expected
	@#
	./1 < input > got
	echo 394420 > expected
	diff -u got expected
	@#
	./2 < input > got
	echo 174420 > expected
	diff -u got expected
	@#
	rm got expected
