CC=gcc

CFLAGS = \
	-std=c11 -O1 -g -ggdb -pedantic -pedantic-errors \
	-Werror -Wall -Wextra \
	-Wformat-y2k -Wformat-nonliteral -Wformat-security \
	-Wformat=2 \
	-Winit-self \
	-Wmissing-include-dirs -Wswitch-default \
	-Wunused-local-typedefs \
	-Wunused-parameter \
	-Wunused-result \
	-Wunused \
	-Wuninitialized \
	-Wfloat-equal -Wundef -Wshadow -Wpointer-arith \
	-Wbad-function-cast \
	-Wempty-body \
	-Wsign-conversion -Wlogical-op -Waggregate-return \
	-Wcast-align -Wstrict-prototypes -Wmissing-prototypes \
	-Wold-style-definition -Wpacked \
	-Wredundant-decls \
	-Wnested-externs -Winline -Winvalid-pch \
	-Wwrite-strings -Waggregate-return \
	-Wswitch-enum -Wconversion -Wunreachable-code

INCLUDE=

TARGETS=map_test

all: $(TARGETS)

map_test: map.c map.h
	$(CC) $(CFLAGS) -DTEST_MAP -o $@ map.c

clean:
	@rm -fv $(TARGETS)
