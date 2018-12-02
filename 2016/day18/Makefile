include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./problem18-1 input.txt 40 > got
	echo 1913 > expected
	diff -ru got expected
	./problem18-1 input.txt 400000 > got
	echo 19993564 > expected
	diff -ru got expected
	rm got expected
