include ../../problems.mk

test: $(TARGETS)
	./1 10 < input-sample-1 > got
	echo 0 > expected
	diff -u got expected
	@#
	./1 300 < input > got
	echo 243 > expected
	diff -u got expected
	@#
	./2 10 < input-sample-2 > got
	echo 1 > expected
	diff -u got expected
	@#
	./2 50 < input > got
	echo 648 > expected
	diff -u got expected
	@#
	rm got expected
