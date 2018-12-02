include ../../problems.mk

test: $(TARGETS)
	./1 5 < input-sample-1 > got
	echo 12 > expected
	diff -u got expected
	@#
	./1 256 < input > got
	echo 46600 > expected
	diff -u got expected
	@#
	./2 256 < input-sample-2 > got
	echo a2582a3a0e66e6e86e3812dcb672a272 > expected
	diff -u got expected
	@#
	./2 256 < input-sample-3 > got
	echo 33efeb34ea91902bb2f59c9920caa6cd > expected
	diff -u got expected
	@#
	./2 256 < input-sample-4 > got
	echo 3efbe78a8d82f29979031a4aa0b16a9d> expected
	diff -u got expected
	@#
	./2 256 < input-sample-5 > got
	echo 63960835bcdc130f0b66d7ff4f6a5a8e > expected
	diff -u got expected
	@#
	rm got expected
