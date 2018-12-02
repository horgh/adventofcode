include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./problem24-1 input.txt 1 > got
	echo 412 > expected
	diff -ru got expected
	./problem24-1 input.txt 2 > got
	echo 664 > expected
	diff -ru got expected
	rm got expected
