include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./problem16-1 10111100110001111 272 > got
	echo 11100110111101110 > expected
	diff -ru got expected
	./problem16-1 10111100110001111 35651584 > got
	echo 10001101010000101 > expected
	diff -ru got expected
	rm got expected
