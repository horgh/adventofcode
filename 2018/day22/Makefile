include ../../problems.mk

CFLAGS+=-O3
CFLAGS+=-lm

test: $(TARGETS)
	./1 < input > got
	echo 11810 > expected
	diff -u got expected
	@#
	./2 < input-sample > got
	echo 45 > expected
	diff -u got expected
	@#
	rm got expected
