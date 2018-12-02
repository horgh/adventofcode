include ../../problems.mk

test: $(TARGETS)
	./problem2-1 > got
	echo 84452 > expected
	diff -ru got expected
	./problem2-2 > got
	echo D65C3 > expected
	diff -ru got expected
	rm got expected
