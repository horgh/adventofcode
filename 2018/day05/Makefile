include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 < input-sample > got
	echo 10 > expected
	diff -u got expected
	@#
	./1 < input > got
	echo 9390 > expected
	diff -u got expected
	@#
	./2 < input-sample > got
	echo 4 > expected
	diff -u got expected
	@#
	./2 < input > got
	echo 5898 > expected
	diff -u got expected
	@#
	rm got expected
