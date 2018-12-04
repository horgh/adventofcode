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
	./1 < input > got
	echo 125444 > expected
	diff -u got expected
	@#
	./2 < input-sample-1 > got
	echo 4455 > expected
	diff -u got expected
	@#
	./2 < input > got
	echo 18325 > expected
	diff -u got expected
	@#
	rm got expected
