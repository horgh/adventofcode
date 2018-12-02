include ../../problems.mk

test: $(TARGETS)
	./day08-1 < input-sample-1 > got
	echo 1 > expected
	diff -u got expected
	@#
	./day08-1 < input > got
	echo 3612 > expected
	diff -u got expected
	@#
	./day08-2 < input-sample-1 > got
	echo 10 > expected
	diff -u got expected
	@#
	./day08-2 < input > got
	echo 3818 > expected
	diff -u got expected
	@#
	rm got expected
