include ../../problems.mk

test: $(TARGETS)
	./problem6-1 input6-1.txt > got
	echo xdkzukcf > expected
	diff -ru got expected
	./problem6-2 input6-1.txt > got
	echo cevsgyvd > expected
	diff -ru got expected
	rm got expected
