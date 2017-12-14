include ../problems.mk

test: $(TARGETS)
	./1 < input-sample-1 > got
	echo 8108 > expected
	diff -u got expected
	@#
	./1 < input > got
	echo 8222 > expected
	diff -u got expected
	@#
	./2 < input-sample-1 > got
	echo 1242 > expected
	diff -u got expected
	@#
	./2 < input > got
	echo 1086 > expected
	diff -u got expected
	@#
	rm got expected
