include ../../problems.mk

test: $(TARGETS)
	./problem9-1 input9-1.txt > got
	echo 98135 > expected
	diff -ru got expected
	./problem9-2 input9-1.txt > got
	echo 10964557606 > expected
	diff -ru got expected
	rm got expected
