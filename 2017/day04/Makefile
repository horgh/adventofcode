include ../problems.mk

test: $(TARGETS)
	./day04-1 < input-sample-1 > got
	echo 2 > expected
	diff -u got expected
	rm got expected
	@#
	./day04-1 < input > got
	echo 386 > expected
	diff -u got expected
	rm got expected
	@#
	./day04-2 < input-sample-2 > got
	echo 3 > expected
	diff -u got expected
	rm got expected
