include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	echo ugknbfddgicrmopn | ./1 > got
	echo 1 > expected
	diff -u got expected
	@#
	echo aaa | ./1 > got
	echo 1 > expected
	diff -u got expected
	@#
	echo jchzalrnumimnmhp | ./1 > got
	echo 0 > expected
	diff -u got expected
	@#
	echo haegwjzuvuyypxyu | ./1 > got
	echo 0 > expected
	diff -u got expected
	@#
	echo dvszwmarrgswjxmb | ./1 > got
	echo 0 > expected
	diff -u got expected
	@#
	./1 < input.txt > got
	echo 258 > expected
	diff -u got expected
	@#
	echo qjhvhtzxzqqjkmpb | ./2 > got
	echo 1 > expected
	diff -u got expected
	@#
	echo xxyxx | ./2 > got
	echo 1 > expected
	diff -u got expected
	@#
	echo uurcxstgmygtbstg | ./2 > got
	echo 0 > expected
	diff -u got expected
	@#
	echo ieodomkazucvgmuy | ./2 > got
	echo 0 > expected
	diff -u got expected
	@#
	./2 < input.txt > got
	echo 53 > expected
	diff -ru got expected
	@#
	rm got expected
