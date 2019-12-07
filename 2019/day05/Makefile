include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 < input > got
	echo 16489636 > expected
	diff -u got expected
	./2 < input > got
	echo 9386583 > expected
	diff -u got expected
	rm -f got expected
