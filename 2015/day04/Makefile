include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	@#
	echo abcdef | ./1 > got
	echo 609043 > expected
	diff -u got expected
	@#
	echo pqrstuv | ./1 > got
	echo 1048970 > expected
	diff -u got expected
	@#
	echo iwrupvqb | ./1 > got
	echo 346386 > expected
	diff -u got expected
	@#
	echo iwrupvqb | ./2 > got
	echo 9958218 > expected
	diff -u got expected
	@#
	rm got expected
