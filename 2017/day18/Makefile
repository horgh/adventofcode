include ../../problems.mk

test: $(TARGETS)
	./1 < input-sample-1 > got
	echo 4 > expected
	diff -u got expected
	@#
	./1 < input > got
	echo 3188 > expected
	diff -u got expected
	@#
	./2 < input-sample-2 > got
	echo 3 > expected
	diff -u got expected
	@#
	./2 < input > got
	echo 7112 > expected
	diff -u got expected
	@#
	rm got expected
