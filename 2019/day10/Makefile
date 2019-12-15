include ../../problems.mk

CFLAGS+=-O2
CFLAGS+=-lm

test: $(TARGETS)
	./1 < input-0 > got
	echo "8 at 3,4" > expected
	diff -u got expected
	./1 < input-1 > got
	echo "33 at 5,8" > expected
	diff -u got expected
	./1 < input-2 > got
	echo "35 at 1,2" > expected
	diff -u got expected
	./1 < input-3 > got
	echo "41 at 6,3" > expected
	diff -u got expected
	./1 < input-4 > got
	echo "210 at 11,13" > expected
	diff -u got expected
	./1 < input > got
	echo "280 at 20,18" > expected
	diff -u got expected
	./2 < input | grep '#200' > got
	echo "vaporize 7,6 (answer: 706) #200" > expected
	diff -u got expected
	rm -f got expected
