include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 < input > got
	echo 1785 > expected
	diff -u got expected
	rm -f got expected
