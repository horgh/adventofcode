CC=gcc

# Reviewed warnings for gcc 6.2.1
CFLAGS = \
	-std=c11 -O1 -g -ggdb -pedantic -pedantic-errors \
	-Werror -Wall -Wextra \
	-Wformat=2 \
	-Wformat-signedness \
	-Wnull-dereference \
	-Winit-self \
	-Wmissing-include-dirs \
	-Wshift-overflow=2 \
	-Wswitch-default \
	-Wswitch-enum \
	-Wunused-const-variable=2 \
	-Wuninitialized \
	-Wunknown-pragmas \
	-Wstrict-overflow=5 \
	-Wsuggest-attribute=pure \
	-Wsuggest-attribute=const \
	-Wsuggest-attribute=noreturn \
	-Wsuggest-attribute=format \
	-Warray-bounds=2 \
	-Wduplicated-cond \
	-Wfloat-equal \
	-Wundef \
	-Wshadow \
	-Wbad-function-cast \
	-Wcast-qual \
	-Wcast-align \
	-Wwrite-strings \
	-Wconversion \
	-Wjump-misses-init \
	-Wlogical-op \
	-Waggregate-return \
	-Wcast-align \
	-Wstrict-prototypes \
	-Wold-style-definition \
	-Wmissing-prototypes \
	-Wmissing-declarations \
	-Wpacked \
	-Wredundant-decls \
	-Wnested-externs \
	-Winline \
	-Winvalid-pch \
	-Wstack-protector

INCLUDE=

TARGETS=problem1 problem1-2

all: $(TARGETS)

problem1: problem1.c
	$(CC) $(CFLAGS) -o $@ problem1.c

problem1-2: problem1-2.c ../lib/map.c ../lib/map.h
	$(CC) $(CFLAGS) -I../lib -o $@ problem1-2.c ../lib/map.c

clean:
	rm -fv $(TARGETS)
