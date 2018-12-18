include ../../problems.mk

CFLAGS+=-O3

test: $(TARGETS)
	./1 < input-sample > got
	echo 57 > expected
	echo 29 >> expected
	diff -u got expected
	@#
	./1 < input > got
	echo 34379 > expected
	echo 28015 >> expected
	diff -u got expected
	@#
	rm got expected
