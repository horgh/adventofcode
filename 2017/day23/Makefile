include ../../problems.mk

#CFLAGS+=-Ofast

test: $(TARGETS)
	./1 < input > got
	echo 3025 > expected
	diff -u got expected
	@#
	./input-translated < input > got
	echo 915 > expected
	diff -u got expected
	@#
	rm got expected
