include ../../problems.mk

CFLAGS+=-O2

test: $(TARGETS)
	./problem15-1 input15-1.txt > got
	echo 317371 > expected
	diff -ru got expected
	./problem15-1 input15-2.txt > got
	echo 2080951 > expected
	diff -ru got expected
	rm got expected
