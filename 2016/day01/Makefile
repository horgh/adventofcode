include ../../problems.mk

test: $(TARGETS)
	./problem1-1 > got
	echo 161 > expected
	diff -ru got expected
	./problem1-2 > got
	echo 110 > expected
	diff -ru got expected
	rm got expected
