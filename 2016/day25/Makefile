include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./problem25-1 input.txt > got
	echo 175 > expected
	diff -ru got expected
	rm got expected
