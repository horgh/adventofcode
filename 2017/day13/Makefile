include ../problems.mk

test: $(TARGETS)
	./1 < input-sample-1 > got
	echo 24 > expected
	diff -u got expected
	@#
	./1 < input > got
	echo 1588 > expected
	diff -u got expected
	@#
	./2 < input-sample-1 > got
	echo 10 > expected
	diff -u got expected
	@#
	./2 < input > got
	echo 3865118 > expected
	diff -u got expected
	@#
	rm got expected
