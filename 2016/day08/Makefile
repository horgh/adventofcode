include ../../problems.mk

test: $(TARGETS)
	./problem8-1 input8-1.txt 0 > got
	echo 115 > expected
	diff -ru got expected
	./problem8-1 input8-1.txt 1 > got
	diff -ru got part2-answer
	rm got expected
