include ../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 5 < input-sample-1 > got
	echo baedc > expected
	diff -u got expected
	@#
	./1 16 < input > got
	echo doeaimlbnpjchfkg > expected
	diff -u got expected
	@#
	./2 16 < input > got
	echo agndefjhibklmocp > expected
	diff -u got expected
	@#
	rm got expected
