include ../../problems.mk

test: $(TARGETS)
	./problem7-1 input7-1.txt > got
	echo 105 > expected
	diff -ru got expected
	./problem7-2 input7-1.txt > got
	echo 258 > expected
	diff -ru got expected
	rm got expected
