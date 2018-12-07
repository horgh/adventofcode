include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 < input-sample > got
	echo 17 > expected
	diff -u got expected
	@#
	./1 < input > got
	echo 3871 > expected
	diff -u got expected
	@#
	./2 32 < input-sample > got
	echo 16 > expected
	diff -u got expected
	@#
	./2 10000 < input > got
	echo 44667 > expected
	diff -u got expected
	@#
	rm got expected
