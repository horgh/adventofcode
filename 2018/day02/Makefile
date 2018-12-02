include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 < input-sample > got
	echo 12 > expected
	diff -u got expected
	./1 < input > got
	echo 7470 > expected
	diff -u got expected
	./2 < input-sample-1 > got
	echo fgij > expected
	diff -u got expected
	./2 < input > got
	echo kqzxdenujwcstybmgvyiofrrd > expected
	diff -u got expected
	rm got expected
