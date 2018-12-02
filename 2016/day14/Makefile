include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./problem14-1 ihaygndm > got
	echo 15035 > expected
	diff -ru got expected
	./problem14-2 ihaygndm > got
	echo 19968 > expected
	diff -ru got expected
	rm got expected
