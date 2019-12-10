include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 < input-0 > got
	diff -u got expected-0
	./1 < input-1 > got
	echo 1219070632396864 > expected
	diff -u got expected
	./1 < input-2 > got
	echo 1125899906842624 > expected
	diff -u got expected
	./1 < input > got
	echo 2682107844 > expected
	diff -u got expected
	./2 < input > got
	echo 34738 > expected
	diff -u got expected
	rm -f got expected
