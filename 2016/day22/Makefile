include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./problem22-1 input.txt > got
	echo 892 > expected
	echo 227 >> expected
	diff -ru got expected
	rm got expected
