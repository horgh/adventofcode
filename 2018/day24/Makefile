include ../../problems.mk

CFLAGS+=-O3

test: $(TARGETS)
	./1 < input > got
	echo 21199 > expected
	diff -u got expected
	@#
	rm got expected
