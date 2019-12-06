include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 1024 < input-0 > got
	echo 6 > expected
	diff -u got expected
	./1 1024 < input-1 > got
	echo 159 > expected
	diff -u got expected
	./1 1024 < input-2 > got
	echo 135 > expected
	diff -u got expected
	./1 102400 < input > got
	echo 273 > expected
	diff -u got expected
	./2 1024 < input-0 > got
	echo 30 > expected
	diff -u got expected
	./2 1024 < input-1 > got
	echo 610 > expected
	diff -u got expected
	./2 1024 < input-2 > got
	echo 410 > expected
	diff -u got expected
	./2 102400 < input > got
	echo 15622 > expected
	diff -u got expected
	rm got expected
