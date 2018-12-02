include ../../problems.mk

test: $(TARGETS)
	./problem3-1 > got
	echo 993 > expected
	diff -ru got expected
	./problem3-2 > got
	echo 1849 > expected
	diff -ru got expected
	rm got expected
