include ../../problems.mk

test: $(TARGETS)
	./day06-1 < input-sample-1 > got
	echo 5 > expected
	diff -u got expected
	@#
	./day06-1 < input > got
	echo 6681 > expected
	diff -u got expected
	@#
	./day06-2 < input-sample-1 > got
	echo 4 > expected
	diff -u got expected
	@#
	./day06-2 < input > got
	echo 2392 > expected
	diff -u got expected
	@#
	rm got expected
