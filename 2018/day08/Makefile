include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 < input-sample > got
	echo 138 > expected
	diff -u got expected
	@#
	./1 < input > got
	echo 36307 > expected
	diff -u got expected
	@#
	./2 < input-sample > got
	echo 66 > expected
	diff -u got expected
	@#
	./2 < input > got
	echo 25154 > expected
	diff -u got expected
	@#
	rm got expected
