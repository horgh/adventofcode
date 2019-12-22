include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 10 < input-0 > got
	echo 179 > expected
	diff -u got expected
	./1 100 < input-1 > got
	echo 1940 > expected
	diff -u got expected
	./1 1000 < input > got
	echo 8362 > expected
	diff -u got expected
	./2 < input-0 > got
	echo 2772 > expected
	diff -u got expected
	./2 < input-1 > got
	echo 4686774924 > expected
	diff -u got expected
	rm -f got expected
