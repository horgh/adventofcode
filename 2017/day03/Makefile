include ../Makefile.mk

.PHONY: test

test:
	echo 1 | ./day03-1 > got
	echo 0 > expected
	diff -u got expected
	@#
	echo 12 | ./day03-1 > got
	echo 3 > expected
	diff -u got expected
	@#
	echo 23 | ./day03-1 > got
	echo 2 > expected
	diff -u got expected
	@#
	echo 1024 | ./day03-1 > got
	echo 31 > expected
	diff -u got expected
	@#
	echo 25 | ./day03-1 > got
	echo 4 > expected
	diff -u got expected
	@#
	echo 22 | ./day03-1 > got
	echo 3 > expected
	diff -u got expected
	@#
	echo 21 | ./day03-1 > got
	echo 4 > expected
	diff -u got expected
	@#
	echo 18 | ./day03-1 > got
	echo 3 > expected
	diff -u got expected
	@#
	echo 16 | ./day03-1 > got
	echo 3 > expected
	diff -u got expected
	@#
	echo 15 | ./day03-1 > got
	echo 2 > expected
	diff -u got expected
	@#
	echo 14 | ./day03-1 > got
	echo 3 > expected
	diff -u got expected
	@#
	echo 11 | ./day03-1 > got
	echo 2 > expected
	diff -u got expected
	@#
	echo 10 | ./day03-1 > got
	echo 3 > expected
	diff -u got expected
	@#
	./day03-1 < input > got
	echo 371 > expected
	diff -u got expected
	rm got expected
	@#
	echo 806 > expected
	./day03-2 800 > got
	diff -u got expected
	rm got expected
	@#
	echo 122 > expected
	./day03-2 100 > got
	diff -u got expected
	rm got expected
	@#
	echo 304 > expected
	./day03-2 300 > got
	diff -u got expected
	rm got expected
	@#
	./day03-2 < input > got
	echo 369601 > expected
	diff -u got expected
	rm got expected
