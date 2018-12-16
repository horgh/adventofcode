include ../../problems.mk

CFLAGS+=-O3

test: $(TARGETS)
	./1 < input-sample-0 > got
	echo 27730 > expected
	diff -u got expected
	@#
	./1 < input-sample-1 > got
	echo 36334 > expected
	diff -u got expected
	@#
	./1 < input-sample-2 > got
	echo 39514 > expected
	diff -u got expected
	@#
	./1 < input-sample-3 > got
	echo 27755 > expected
	diff -u got expected
	@#
	./1 < input-sample-4 > got
	echo 28944 > expected
	diff -u got expected
	@#
	./1 < input-sample-5 > got
	echo 18740 > expected
	diff -u got expected
	@#
	./1 < input > got
	echo 196200 > expected
	diff -u got expected
	@#
	@#
	./2 < input-sample-0 > got
	echo 4988 > expected
	echo 15 >> expected
	diff -u got expected
	@#
	./2 < input-sample-2 > got
	echo 31284 > expected
	echo 4 >> expected
	diff -u got expected
	@#
	./2 < input-sample-3 > got
	echo 3478 > expected
	echo 15 >> expected
	diff -u got expected
	@#
	./2 < input-sample-4 > got
	echo 6474 > expected
	echo 12 >> expected
	diff -u got expected
	@#
	./2 < input-sample-5 > got
	echo 1140 > expected
	echo 34 >> expected
	diff -u got expected
	@#
	./2 < input > got
	echo 61750 > expected
	echo 17 >> expected
	diff -u got expected
	@#
	rm got expected
