include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./problem23-1 input.txt 1 > got
	echo 11683 > expected
	diff -ru got expected
	./problem23-1 input.txt 2 > got
	echo 479008243 > expected
	diff -ru got expected
	rm got expected
