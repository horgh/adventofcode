#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <map.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static int
count_occurrences(char const * const, char const);

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	int twos = 0;
	int threes = 0;
	while (1) {
		char buf[4096] = {0};
		if (fgets(buf, 4096, stdin) == NULL) {
			break;
		}

		char const * ptr = buf;
		bool has_two = false;
		bool has_three = false;
		//printf("%s\n", buf);
		struct htable * const h = hash_init(1024);
		assert(h != NULL);
		while (*ptr != '\n' && *ptr != '\0') {
			if (hash_has_key_i(h, (int) *ptr)) {
				ptr++;
				continue;
			}

			int const count = count_occurrences(ptr, *ptr);
			assert(hash_set_i(h, (int) *ptr, NULL));
			//printf("%c occurs %d times\n", *ptr, count);
			if (count == 2) {
				has_two = true;
				ptr++;
				continue;
			}
			if (count == 3) {
				has_three = true;
				ptr++;
				continue;
			}
			ptr++;
		}
		assert(hash_free(h, NULL));
		if (has_two) {
			twos++;
		}
		if (has_three) {
			threes++;
		}
	}

	printf("%d\n", twos*threes);
	return 0;
}

__attribute__((pure))
static int
count_occurrences(char const * const s, char const c)
{
	char const * ptr = s;
	int count = 0;
	while (*ptr != '\n' && *ptr != '\0') {
		if (*ptr == c) {
			count++;
		}
		ptr++;
	}
	return count;
}
