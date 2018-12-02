include ../../problems.mk

test: $(TARGETS)
	./1 < input-sample-1 > got
	echo 3 > expected
	diff -u got expected
	@#
	./1 < input-sample-2 > got
	echo 0 > expected
	diff -u got expected
	@#
	./1 < input-sample-3 > got
	echo 2 > expected
	diff -u got expected
	@#
	./1 < input-sample-4 > got
	echo 3 > expected
	diff -u got expected
	@#
	./1 < input > got
	echo 824 > expected
	diff -u got expected
	@#
	./2 < input > got
	echo 1548 > expected
	diff -u got expected
	@#
	rm got expected
