include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./problem20-1 input.txt > got
	echo 14975795 > expected
	diff -ru got expected
	./problem20-2 input.txt > got
	echo 101 > expected
	diff -ru got expected
	rm got expected
