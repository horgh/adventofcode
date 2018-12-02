include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./problem17-1 rrrbmfta > got
	echo RLRDRDUDDR > expected
	diff -ru got expected
	./problem17-2 rrrbmfta > got
	echo 420 > expected
	diff -ru got expected
	rm got expected
