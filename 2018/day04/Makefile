include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 < input-sample > got
	echo 240 > expected
	diff -u got expected
	@#
	./1 < input-sample-1 > got
	echo 240 > expected
	diff -u got expected
	@#
	rm got expected
