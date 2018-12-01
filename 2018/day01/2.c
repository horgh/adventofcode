#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <map.h>
#include <queue.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int
main(int const argc, char const * const * const argv)
{
	(void) argc;
	(void) argv;

	int input[4096] = {0};
	int i = 0;
	while (1) {
		char buf[4096] = {0};
		if (fgets(buf, 4096, stdin) == NULL) {
			break;
		}

		char const * ptr = buf;
		while (*ptr == '+') {
			ptr++;
		}

		int const n = atoi(buf);
		input[i++] = n;
	}

	struct htable * const h = hash_init(1024);

	int freq = 0;
	hash_set_i(h, freq, NULL);
	int j = 0;
	while (1) {
		int const val = input[j];
		j++;
		if (j == i) {
			j = 0;
		}

		freq += val;

		if (hash_has_key_i(h, freq)) {
			printf("%d\n", freq);
			break;
		}

		hash_set_i(h, freq, NULL);
	}

	return 0;
}
