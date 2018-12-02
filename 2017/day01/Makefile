include ../../problems.mk

test: $(TARGETS)
	echo 1122 | ./day01-1 > got
	echo 1111 | ./day01-1 >> got
	echo 1234 | ./day01-1 >> got
	echo 91212129 | ./day01-1 >> got
	diff -u got output-sample-1
	rm got
	@#
	./day01-1 < input > got
	diff -u got output-1
	rm got
	@#
	echo 1212 | ./day01-2 > got
	echo 1221 | ./day01-2 >> got
	echo 123425 | ./day01-2 >> got
	echo 123123 | ./day01-2 >> got
	echo 12131415 | ./day01-2 >> got
	diff -u got output-sample-2
	rm got
	@#
	./day01-2 < input > got
	diff -u got output-2
	rm got
