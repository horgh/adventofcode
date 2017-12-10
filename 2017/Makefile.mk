CC=gcc

# Reviewed warnings for gcc 6.3.0
CFLAGS = \
	-std=c11 -g -ggdb -pedantic -pedantic-errors \
	-Werror -Wall -Wextra \
	-Wchkp \
	-Wdouble-promotion \
	-Wformat=2 \
	-Wformat-signedness \
	-Wnull-dereference \
	-Winit-self \
	-Wmissing-include-dirs \
	-Wshift-overflow=2 \
	-Wswitch-default \
	-Wswitch-enum \
	-Wunused-parameter \
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
	-Wdate-time \
	-Wjump-misses-init \
	-Wlogical-op \
	-Waggregate-return \
	-Wstrict-prototypes \
	-Wold-style-definition \
	-Wmissing-prototypes \
	-Wmissing-declarations \
	-Wpacked \
	-Wpadded \
	-Wredundant-decls \
	-Wnested-externs \
	-Winline \
	-Winvalid-pch \
	-Wstack-protector

# Assume all .c files correspond to a program to build.
TARGETS_ALL=$(patsubst %.c,%,$(wildcard *.c))
TARGETS=$(filter-out siphash, $(TARGETS_ALL))

all: $(TARGETS)

# This is a static pattern rule to build all .c programs (TARGETS).
# Don't define a recipe. Instead, rely on implicit rules to build these. If we
# need to give more specific recipes, we can override the implicit rule by
# writing a recipe for the target.
$(TARGETS): %: %.c

clean:
	rm -f $(TARGETS) *.o
