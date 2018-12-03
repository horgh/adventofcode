include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 < input-sample > got
	echo 998996 > expected
	diff -ru got expected
	@#
	./1 < input > got
	echo 377891 > expected
	diff -ru got expected
	@#
	./2 < input-sample-1 > got
	echo 2000001 > expected
	diff -ru got expected
	@#
	./2 < input > got
	echo 14110788 > expected
	diff -ru got expected
	@#
	rm got expected
