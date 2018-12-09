include ../../problems.mk

CFLAGS+=-O3

test: $(TARGETS)
	./1 < input-sample-0 > got
	echo 32 > expected
	diff -u got expected
	@#
	./1 < input-sample-1 > got
	echo 8317 > expected
	diff -u got expected
	@#
	./1 < input-sample-2 > got
	echo 146373 > expected
	diff -u got expected
	@#
	./1 < input-sample-3 > got
	echo 2764 > expected
	diff -u got expected
	@#
	./1 < input-sample-4 > got
	echo 54718 > expected
	diff -u got expected
	@#
	./1 < input-sample-5 > got
	echo 37305 > expected
	diff -u got expected
	@#
	./1 < input > got
	echo 423717 > expected
	diff -u got expected
	@#
	./2 < input-sample-0 > got
	echo 32 > expected
	diff -u got expected
	@#
	./2 < input-sample-1 > got
	echo 8317 > expected
	diff -u got expected
	@#
	./2 < input-sample-2 > got
	echo 146373 > expected
	diff -u got expected
	@#
	./2 < input-sample-3 > got
	echo 2764 > expected
	diff -u got expected
	@#
	./2 < input-sample-4 > got
	echo 54718 > expected
	diff -u got expected
	@#
	./2 < input-sample-5 > got
	echo 37305 > expected
	diff -u got expected
	@#
	./2 < input > got
	echo 423717 > expected
	diff -u got expected
	@#
	rm got expected
