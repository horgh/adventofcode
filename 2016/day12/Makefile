include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./problem12-1 input12-1.txt 1 > got
	echo 318117 > expected
	diff -ru got expected
	./problem12-1 input12-1.txt 2 > got
	echo 9227771 > expected
	diff -ru got expected
	rm got expected
