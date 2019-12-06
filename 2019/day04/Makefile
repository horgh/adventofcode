include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	echo 111111-111111 | ./1 > got
	echo 1 > expected
	diff -u got expected
	echo 223450-223450 | ./1 > got
	echo 0 > expected
	diff -u got expected
	echo 123789-123789 | ./1 > got
	echo 0 > expected
	diff -u got expected
	./1 < input > got
	echo 1330 > expected
	diff -u got expected
	echo 112233-112233 | ./2 > got
	echo 1 > expected
	diff -u got expected
	echo 123444-123444 | ./2 > got
	echo 0 > expected
	diff -u got expected
	echo 111122-111122 | ./2 > got
	echo 1 > expected
	diff -u got expected
	./2 < input > got
	echo 876 > expected
	diff -u got expected
	rm -f got expected
