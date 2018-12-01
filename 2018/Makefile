TARGETS_ALL=$(wildcard *)
TARGETS=$(filter-out Makefile Makefile.mk problems.mk, $(TARGETS_ALL))

# https://stackoverflow.com/questions/11206500/compile-several-projects-with-makefile-but-stop-on-first-broken-build/11206700#11206700

.PHONY: all $(TARGETS) clean

all: $(TARGETS) clean

$(TARGETS):
	$(MAKE) -C $@ clean test
	$(MAKE) -C $@ clean

clean:
	@# We'll get this rebuilt due to it being a dependency
	$(MAKE) -C lib clean
