include ../problems.mk

test: $(TARGETS)
	./1 70 < input-sample-1 > got
	echo 41 > expected
	diff -u got expected
	@#
	./1 10000 < input-sample-1 > got
	echo 5587 > expected
	diff -u got expected
	@#
	./1 10000 < input > got
	echo 5399 > expected
	diff -u got expected
	@#
	./2 100 < input-sample-1 > got
	echo 26 > expected
	diff -u got expected
	@#
	./2 10000000 < input-sample-1 > got
	echo 2511944 > expected
	diff -u got expected
	@#
	./2 10000000 < input > got
	echo 2511776 > expected
	diff -u got expected
	@#
	rm got expected
