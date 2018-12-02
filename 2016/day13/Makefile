include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./problem13-1 1350 31 39 > got
	echo 92 > expected
	diff -ru got expected
	./problem13-2 1350 50 > got
	echo 124 > expected
	diff -ru got expected
	rm got expected
