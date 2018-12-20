include ../../problems.mk

CFLAGS+=-O1

test: $(TARGETS)
	./1 < input > got
	echo 2160 > expected
	diff -u got expected
	@#
	./3 > got
	echo 25945920 > expected
	diff -u got expected
	@#
	rm got expected
