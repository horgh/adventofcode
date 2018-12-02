include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./problem11-1 > got
	echo 33 > expected
	diff -ru got expected
	./problem11-2 > got
	echo 57 > expected
	diff -ru got expected
	rm got expected
