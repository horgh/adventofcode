include ../../problems.mk

CFLAGS+=-O3

test: $(TARGETS)
	./1 < input-sample-0 > got
	echo 3 > expected
	diff -u got expected
	@#
	./1 < input-sample-1 > got
	echo 10 > expected
	diff -u got expected
	@#
	./1 < input-sample-2 > got
	echo 18 > expected
	diff -u got expected
	@#
	./1 < input-sample-3 > got
	echo 23 > expected
	diff -u got expected
	@#
	./1 < input-sample-4 > got
	echo 31 > expected
	diff -u got expected
	@#
	./1 < input > got
	echo 3314 > expected
	diff -u got expected
	@#
	./2 < input > got
	echo 8550 > expected
	diff -u got expected
	@#
	rm got expected
