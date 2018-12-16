include ../../problems.mk

CFLAGS+=-O3

test: $(TARGETS)
	./1 < input-sample-0 > got
	echo 1 > expected
	diff -u got expected
	@#
	./1 < input > got
	echo 646 > expected
	diff -u got expected
	@#
	./2 < input > got
	echo 681 > expected
	diff -u got expected
	@#
	rm got expected
