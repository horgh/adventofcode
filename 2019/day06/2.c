#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <map.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void count_orbits(struct htable const * const,
		char const * const,
		struct htable * const,
		int const);

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

	struct htable * const my_counts = hash_init(1024);
	assert(my_counts != NULL);
	count_orbits(h, "YOU", my_counts, 0);

	struct htable * const san_counts = hash_init(1024);
	assert(san_counts != NULL);
	count_orbits(h, "SAN", san_counts, 0);

	void * * const keys = hash_get_keys(my_counts);
	int min = -1;
	for (size_t i = 0; keys[i]; i++) {
		if (!hash_has_key(san_counts, keys[i])) {
			continue;
		}

		int const * my_count = hash_get(my_counts, keys[i]);
		int const * san_count = hash_get(san_counts, keys[i]);
		int const total = *my_count+*san_count;
		if (min == -1 || total < min) {
			min = total;
		}
	}

	printf("%d\n", min);
	return 0;
}

static void count_orbits(struct htable const * const h,
		char const * const key,
		struct htable * const counts,
		int const steps)
{
	char const * const next_key = hash_get(h, key);
	if (!next_key) {
		return;
	}

	int * const count = calloc(1, sizeof(int));
	assert(count != NULL);

	*count = steps;

	assert(hash_set(counts, next_key, count));

	count_orbits(h, next_key, counts, steps+1);
}
