#define _POSIX_C_SOURCE 200809L

#include <map.h>
#include <stdio.h>
#include <stdlib.h>

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

		int const n = atoi(buf);
		input[i++] = n;
	}

	struct htable * const h = hash_init(1024);
	if (!h) {
		fprintf(stderr, "hash_init()\n");
		return 1;
	}

	int freq = 0;
	if (!hash_set_i(h, freq, NULL)) {
		fprintf(stderr, "hash_set_i()\n");
		hash_free(h, NULL);
		return 1;
	}

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

		if (!hash_set_i(h, freq, NULL)) {
			fprintf(stderr, "hash_set_i()\n");
			hash_free(h, NULL);
			return 1;
		}
	}

	if (!hash_free(h, NULL)) {
		fprintf(stderr, "hash_free()\n");
		return 1;
	}

	return 0;
}
