include ../../problems.mk

CFLAGS+=-O3

test: $(TARGETS)
	./1 9 > got
	echo 5158916779 > expected
	diff -u got expected
	@#
	./1 5 > got
	echo 0124515891 > expected
	diff -u got expected
	@#
	./1 18 > got
	echo 9251071085 > expected
	diff -u got expected
	@#
	./1 2018 > got
	echo 5941429882 > expected
	diff -u got expected
	@#
	./1 430971 > got
	echo 5715102879 > expected
	diff -u got expected
	@#
	./2 > got
	echo 20225706 > expected
	diff -u got expected
	@#
	rm got expected
