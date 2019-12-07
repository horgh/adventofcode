#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <map.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int count_orbits(struct htable const * const,
		char const * const);

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	struct htable * const h = hash_init(1024);
	assert(h != NULL);

	while (1) {
		char buf[8192] = {0};
		if (fgets(buf, sizeof(buf), stdin) == NULL) {
			break;
		}

		char const * ptr = buf;

		char * const target = calloc(128, sizeof(char));
		assert(target != NULL);
		size_t n = 0;
		while (*ptr != ')') {
			target[n++] = *ptr;
			ptr++;
		}
		ptr++;

		char key[128] = {0};
		n = 0;
		while (*ptr != '\n') {
			key[n++] = *ptr;
			ptr++;
		}

		assert(hash_set(h, key, target));
	}

	void * * const keys = hash_get_keys(h);
	assert(keys != NULL);
	int count = 0;
	for (size_t i = 0; keys[i]; i++) {
		count += count_orbits(h, keys[i]);
	}

	printf("%d\n", count);
	return 0;
}

static int count_orbits(struct htable const * const h,
		char const * const key)
{
	char const * const next_key = hash_get(h, key);
	if (!next_key) {
		return 0;
	}

	return 1+count_orbits(h, next_key);
}
