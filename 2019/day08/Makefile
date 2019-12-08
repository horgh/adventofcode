include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 25 6 < input > got
	echo 1224 > expected
	diff -u got expected
	rm -f got expected
