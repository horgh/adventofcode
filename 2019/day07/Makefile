include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 < input-0 > got
	echo 43210 > expected
	diff -u got expected
	./1 < input-1 > got
	echo 54321 > expected
	diff -u got expected
	./1 < input-2 > got
	echo 65210 > expected
	diff -u got expected
	./1 < input > got
	echo 116680 > expected
	diff -u got expected
	./2 < input-3 > got
	echo 139629729 > expected
	diff -u got expected
	./2 < input-4 > got
	echo 18216 > expected
	diff -u got expected
	./2 < input > got
	echo 89603079 > expected
	diff -u got expected
	rm -f got expected
