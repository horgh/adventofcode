include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 < input-0 > got
	echo 31 > expected
	diff -u got expected
	./1 < input-1 > got
	echo 165 > expected
	diff -u got expected
	./1 < input-2 > got
	echo 13312 > expected
	diff -u got expected
	./1 < input-3 > got
	echo 180697 > expected
	diff -u got expected
	./1 < input-4 > got
	echo 2210736 > expected
	diff -u got expected
	./1 < input > got
	echo 387001 > expected
	diff -u got expected
	rm -f got expected
