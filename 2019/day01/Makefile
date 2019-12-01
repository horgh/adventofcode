include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./1 < input-sample > got
	echo 34241 > expected
	diff -u got expected
	./1 < input > got
	echo 3267890 > expected
	diff -u got expected
	echo 1969 | ./2 > got
	echo 966 > expected
	diff -u got expected
	echo 100756 | ./2 > got
	echo 50346 > expected
	diff -u got expected
	./2 < input > got
	echo 4898972 > expected
	diff -u got expected
	rm got expected
