include ../problems.mk

test: $(TARGETS)
	./day02-1 < input-sample-1 > got
	diff -u got output-sample-1
	rm got
	@#
	./day02-1 < input > got
	diff -u got output-1
	rm got
	@#
	./day02-2 < input > got
	diff -u got output-2
	rm got
