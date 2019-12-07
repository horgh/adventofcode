include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 < input-0 > got
	echo 42 > expected
	diff -u got expected
	./1 < input > got
	echo 194721 > expected
	diff -u got expected
	./2 < input-1 > got
	echo 4 > expected
	diff -u got expected
	./2 < input > got
	echo 316 > expected
	diff -u got expected
	rm -f got expected
