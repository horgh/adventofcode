include ../../problems.mk

test: $(TARGETS)
	./problem10-1 input10-1.txt > got
	echo 157 > expected
	echo 1085 >> expected
	diff -ru got expected
	rm got expected
