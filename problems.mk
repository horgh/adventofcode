# This Makefile is suitable for including in problem directories.
#
# It makes some assumptions that are not suitable for the lib directory.

include ../../Makefile.mk

CFLAGS+=-I../../lib
LDFLAGS=-L../../lib -ladvent -lssl -lcrypto

# Assume all .c files correspond to a program to build.
TARGETS_ALL=$(patsubst %.c,%,$(wildcard *.c))
TARGETS=$(filter-out siphash, $(TARGETS_ALL))

.PHONY: test

all: $(TARGETS)

# This is a static pattern rule to build all .c programs (TARGETS).
# Don't define a recipe. Instead, rely on implicit rules to build these. If we
# need to give more specific recipes, we can override the implicit rule by
# writing a recipe for the target.
$(TARGETS): %: %.c ../../lib/libadvent.a

../../lib/libadvent.a:
	$(MAKE) -C ../../lib libadvent.a

clean:
	rm -f $(TARGETS)
