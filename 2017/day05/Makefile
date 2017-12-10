include ../problems.mk

test: $(TARGETS)
	./day05-1 < input-sample-1 > got
	echo 5 > expected
	diff -u got expected
	@#
	./day05-1 < input > got
	echo 378980 > expected
	diff -u got expected
	@#
	./day05-2 < input-sample-1 > got
	echo 10 > expected
	diff -u got expected
	@#
	./day05-2 < input > got
	echo 26889114 > expected
	diff -u got expected
	@#
	rm got expected
