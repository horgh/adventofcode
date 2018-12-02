include ../../problems.mk

CFLAGS+=-O3

test: $(TARGETS)
	./problem19-1 3005290 > got
	echo 1816277 > expected
	diff -ru got expected
	./problem19-2 3005290 > got
	echo 1410967 > expected
	diff -ru got expected
	rm got expected
