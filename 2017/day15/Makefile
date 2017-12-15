include ../problems.mk

test: $(TARGETS)
	./1 65 8921 > got
	echo 588 > expected
	diff -u got expected
	@#
	./1 634 301 > got
	echo 573 > expected
	diff -u got expected
	@#
	./2 65 8921 > got
	echo 309 > expected
	diff -u got expected
	@#
	./2 634 301 > got
	echo 294 > expected
	diff -u got expected
	@#
	rm got expected
