include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 d < input-sample > got
	echo 72 > expected
	diff -u got expected
	@#
	./1 e < input-sample > got
	echo 507 > expected
	diff -u got expected
	@#
	./1 f < input-sample > got
	echo 492 > expected
	diff -u got expected
	@#
	./1 g < input-sample > got
	echo 114 > expected
	diff -u got expected
	@#
	./1 h < input-sample > got
	echo 65412 > expected
	diff -u got expected
	@#
	./1 i < input-sample > got
	echo 65079 > expected
	diff -u got expected
	@#
	./1 x < input-sample > got
	echo 123 > expected
	diff -u got expected
	@#
	./1 y < input-sample > got
	echo 456 > expected
	diff -u got expected
	@#
	./1 a < input > got
	echo 16076 > expected
	diff -u got expected
	@#
	./2 a < input > got
	echo 2797 > expected
	diff -u got expected
	@#
	rm got expected
