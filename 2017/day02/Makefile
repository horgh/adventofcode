include ../Makefile.mk

.PHONY: test

test:
	./day02-1 < input-sample-1 > got-sample-1
	diff -u output-sample-1 got-sample-1
	rm got-sample-1
	./day02-1 < input > got
	diff -u output-1 got
	rm got
	./day02-2 < input > got
	diff -u output-2 got
	rm got
