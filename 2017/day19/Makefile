include ../../problems.mk

test: $(TARGETS)
	./1 < input-sample-1 > got
	echo ABCDEF  > expected
	diff -u got expected
	@#
	./1 < input > got
	echo DTOUFARJQ > expected
	diff -u got expected
	@#
	./2 < input-sample-1 > got
	echo 38 > expected
	diff -u got expected
	@#
	./2 < input > got
	echo 16642 > expected
	diff -u got expected
	@#
	rm got expected
