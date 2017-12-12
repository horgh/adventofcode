include ../problems.mk

test: $(TARGETS)
	./1 < input-sample-1 > got
	echo 6 > expected
	diff -u got expected
	@#
	./1 < input > got
	echo 128 > expected
	diff -u got expected
	@#
	./2 < input-sample-1 > got
	echo 2 > expected
	diff -u got expected
	@#
	./2 < input > got
	echo 209 > expected
	diff -u got expected
	@#
	rm got expected
