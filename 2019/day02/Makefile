include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 < input-sample > got
	diff -u got want-sample
	rm got
	./1 1 < input > got
	diff -u got want-1
	rm got
	./2 < input > got
	echo 7912 > expected
	diff -u got expected
	rm got expected
