include ../../problems.mk

CFLAGS+=-O3

test: $(TARGETS)
	./1 < input > got
	echo 410 > expected
	diff -u got expected
	@#
	./2 < input > got
	echo 119188816 > expected
	diff -u got expected
	@#
	rm got expected
