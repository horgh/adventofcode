include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./problem21-1 input21-1.txt abcdefgh > got
	echo dgfaehcb > expected
	diff -ru got expected
	./problem21-2 input21-1.txt fbgdceah > got
	echo fdhgacbe > expected
	diff -ru got expected
	rm got expected
