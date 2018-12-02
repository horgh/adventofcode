include ../../problems.mk

test: $(TARGETS)
	./problem4-1 > got
	echo 173787 > expected
	diff -ru got expected
	./problem4-2 > got
	echo 548 > expected
	diff -ru got expected
	rm got expected
