include ../../problems.mk

test: $(TARGETS)
	./1 2 < input-sample-1 > got
	echo 12 > expected
	diff -u got expected
	@#
	./1 5 < input > got
	echo 171 > expected
	diff -u got expected
	@#
	./1 18 < input > got
	echo 2498142 > expected
	diff -u got expected
	@#
	rm got expected
