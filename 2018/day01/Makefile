include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 < input > got
	echo 497 > expected
	diff -u got expected
	./2 < input > got
	echo 558 > expected
	diff -u got expected
	rm got expected
