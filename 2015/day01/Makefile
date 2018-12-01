include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	@# Sample
	echo '(())' | ./1 > got
	echo 0 > expected
	diff -u got expected
	@# Sample
	echo '()()' | ./1 > got
	echo 0 > expected
	diff -u got expected
	@# Sample
	echo '(((' | ./1 > got
	echo 3 > expected
	diff -u got expected
	@# Sample
	echo '(()(()(' | ./1 > got
	echo 3 > expected
	diff -u got expected
	@# Sample
	echo '))(((((' | ./1 > got
	echo 3 > expected
	diff -u got expected
	@# Sample
	echo '())' | ./1 > got
	echo -1 > expected
	diff -u got expected
	@# Sample
	echo '))(' | ./1 > got
	echo -1 > expected
	diff -u got expected
	@# Sample
	echo ')))' | ./1 > got
	echo -3 > expected
	diff -u got expected
	@# Sample
	echo ')())())' | ./1 > got
	echo -3 > expected
	diff -u got expected
	@# Part 1
	./1 < input > got
	echo 74 > expected
	diff -u got expected
	@# Sample
	echo ')' | ./2 > got
	echo 1 > expected
	diff -u got expected
	@# Sample
	echo '()())' | ./2 > got
	echo 5 > expected
	diff -u got expected
	@# Part 2
	./2 < input > got
	echo 1795 > expected
	diff -u got expected
	@# Clean up
	rm got expected
