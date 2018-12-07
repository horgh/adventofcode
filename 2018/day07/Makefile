include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 < input-sample > got
	echo CABDFE > expected
	diff -u got expected
	@#
	./1 < input > got
	echo BFKEGNOVATIHXYZRMCJDLSUPWQ > expected
	diff -u got expected
	@#
	./2 2 0 < input-sample > got
	echo 15 > expected
	diff -u got expected
	@#
	./2 5 60 < input > got
	echo 1020 > expected
	diff -u got expected
	@#
	rm got expected
