include ../../problems.mk

test: $(TARGETS)
	./day07-1 < input-sample-1 > got
	echo tknk > expected
	diff -u got expected
	@#
	./day07-1 < input > got
	echo eqgvf > expected
	diff -u got expected
	@#
	./day07-2 < input-sample-1 > got
	echo 60 > expected
	diff -u got expected
	@#
	./day07-2 < input > got
	echo 757 > expected
	diff -u got expected
	@#
	rm got expected
